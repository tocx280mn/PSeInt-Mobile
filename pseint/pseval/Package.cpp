#include <wx/filename.h>
#include <wx/zipstrm.h>
#include <cstddef>
#include <wx/wfstream.h>
#include <iostream>
#include <wx/sstream.h>
#include <wx/dir.h>
#include <wx/textfile.h>
#include "version.h"
#include <wx/stream.h>
#include <wx/mstream.h>
#include <wx/fs_mem.h>
#include <wx/image.h>
#include <wx/filesys.h>
#include <wx/msgdlg.h>
#include "Package.h"
#include "../wxPSeInt/string_conversions.h"
#ifdef ALLOW_MARKDOWN
#	include "../hoewrap/hoewrap.h"
#endif

class mxFilterInputStream : public wxFilterInputStream {
	wxString m_key; 
	bool m_old_cypher;
	int pos, len, base_pos, base_delta;
public:
	mxFilterInputStream(wxInputStream &stream, const wxString &key, bool old_cypher) 
		: wxFilterInputStream(stream), m_key(key), m_old_cypher(old_cypher), len(key.Len())
	{
		pos=0; for(int i=0;i<len;i++) pos+=_C(m_key[i]); 
		if (len) base_delta=base_pos=pos=(pos%len); 
		else base_delta=base_pos=0;
	}
protected:
	size_t OnSysRead(void *buffer, size_t size) {
		size_t count = m_parent_i_stream->Read(buffer, size).LastRead();
		if (!len) return count;
		unsigned char *auxb = reinterpret_cast<unsigned char*>(buffer);
		for(unsigned int i=0;i<count;i++) { 
			int x = auxb[i];
			base_delta = ((m_old_cypher?0:base_delta) + _C(m_key[pos]))%256;
			x = ( x + 256 - base_delta) % 256;
			pos=(pos+1)%len;
			auxb[i] = x;
		}
		return count;
	}
};

class mxFilterOutputStream : public wxFilterOutputStream {
	wxString m_key;
	bool m_old_cypher;
	int pos, len, base_pos, base_delta;
public:
	mxFilterOutputStream(wxOutputStream &stream, const wxString &key, bool old_cypher) 
		: wxFilterOutputStream(stream), m_key(key), m_old_cypher(old_cypher), len(key.Len())
	{ 
		pos=0; for(int i=0;i<len;i++) pos+=_C(m_key[i]);
		if (len) base_delta=base_pos=pos=(pos%len); 
		else base_delta=base_pos=0;
	}
protected:
	virtual wxOutputStream& Write(const void *buffer, size_t size) {
		if (!len) return m_parent_o_stream->Write(buffer, size);
		static size_t last_size=0; static unsigned char *auxbuf=NULL;
		if (last_size<size) {
			if (last_size) delete [] auxbuf;
			auxbuf=new unsigned char[size+1];
			last_size=size;
		}
		const unsigned char *ubuf = reinterpret_cast<const unsigned char*>(buffer);
		unsigned char *uaux = reinterpret_cast<unsigned char*>(auxbuf);
		for(unsigned int i=0;i<size;i++) { 
			int x = ubuf[i]; 
			base_delta = ((m_old_cypher?0:base_delta)+_C(m_key[pos]))%256;
			x = ( x + base_delta ) % 256;
			pos=(pos+1)%len;
			uaux[i] = x;
		}
		return m_parent_o_stream->Write(uaux, size);
	}
};

static int cyper_version_int = 20160401;
static const char cypher_version_str[] = "ver20160401";

static void WriteTextFile(wxZipOutputStream &zip, const wxString &name, const wxString &content) {
	zip.PutNextEntry(name);
	static wxCSConv cs("ISO-8859-1");
	const auto data = content.mb_str(cs);
	wxMemoryInputStream is(data,data.length());
	zip<<is;
}

bool Package::Load (const wxString & fname, const wxString &passkey) {
	if (!wxFileName::FileExists(fname)) return false;
	wxZipEntry *entry = NULL;
	wxFileInputStream in_orig(fname);
	
	// detectar el tipo de encriptacion del ejercicio, y ver si esta versión de pseint la soporta, o el ejercicio viene de una futura
	bool old_cypher = false;
	if (passkey.Len()) {
		char version_buff[] = "xxx11112233";
		in_orig.Read(version_buff,12);
		if (version_buff[0]=='v'&&version_buff[1]=='e' && version_buff[2]=='r' && version_buff[11]=='\0') {
			int req_ver = 0;
			for(int i=10,mult=1;i>2;i--,mult*=10) req_ver+=(version_buff[i]-'0')*mult;
			if (req_ver>cyper_version_int) {
				wxMessageBox(wxString(_Z("Este ejercicio no se puede abrir porque fue\n"
									  "creado para una versión de PSeInt más reciente\n"
									  "que esta. Actualice PSeInt para poder continuar.\n\n"
									  "La versión mínima necesaria es: "))<<req_ver<<_Z("\n\n"
									  "Puede descargar las actualizaciones desde:\n"
									  "http://pseint.sourceforge.net"));
				return false;
			}
		} else {
			old_cypher = true;
			in_orig.SeekI(0);
		}
	}
		
	mxFilterInputStream in(in_orig,passkey,old_cypher);
	if (!in) return false;
	wxZipInputStream zip(in);
	bool read_something=false;
	while ((entry=zip.GetNextEntry())) {
		read_something=true;
		wxString name = entry->GetName();
		if (entry->IsDir()) {
			std::cerr<<"ERROR in Package::Load: Found directory "<<name<<std::endl;
			continue;
		}
		zip.OpenEntry(*entry);
		if (!zip.CanRead()) return false;
		if (name.Lower().EndsWith(".png")) {
			wxMemoryOutputStream os;
			zip.Read(os);
#ifdef FOR_WXPSEINT
			m_images.Add(name);
			wxMemoryInputStream is(os);
			wxImage img(is,wxBITMAP_TYPE_PNG);
			wxMemoryFSHandler::AddFile(name,img,wxBITMAP_TYPE_PNG);
#endif
		} else {
			wxString content;
			wxStringOutputStream os(&content,wxConvISO8859_1);
			zip.Read(os);
			ProcessFile(name,content);
		}
	}
//	SetConfigStr("mostrar casos fallidos","todos");
	return m_is_ok=read_something;
}

bool Package::Load (const wxString &dir) {
	wxString mdir=dir; if (mdir.Last()=='\\'||mdir.Last()=='/') mdir.RemoveLast();
	Reset();
	wxDir wxdir(mdir);
	if ( wxdir.IsOpened() ) {
		wxString filename;
		bool cont = wxdir.GetFirst(&filename, "*" , wxDIR_FILES);
		while ( cont ) {
			if(filename.Lower().EndsWith(".png")) {
				m_images.Add(mdir+wxFileName::GetPathSeparator()+filename);
			} else {
				static wxCSConv cs("ISO-8859-1");
				wxFile file(mdir+wxFileName::GetPathSeparator()+filename);
				wxString content; wxStringOutputStream sos(&content,cs);
				wxFileInputStream fis(file); fis.Read(sos);
				ProcessFile(filename,content);
			}
			cont = wxdir.GetNext(&filename);
		}
	}
	return m_is_ok=true;
}

TestCase & Package::GetTest (const wxString &name) {
	return m_tests.find(name)->second;
}

int Package::GetNames (wxArrayString & names) {
	names.Clear();
	for(std::map<wxString,TestCase>::iterator it = m_tests.begin();it!=m_tests.end();++it)
		names.Add(it->first);
	return names.GetCount();
}

void Package::ProcessFile (wxString name, wxString &content) {
	name.MakeLower();
	if (name=="base.psc") {
		m_base_psc = content;
	} else if (name=="help.html") {
		m_help_text = content;
#ifdef ALLOW_MARKDOWN
	} else if (name=="help.md") {
		static wxCSConv cs("ISO-8859-1");
		const auto data = content.mb_str(cs);
		m_help_text = wxString(markdown2html(data,data.length()),cs);
#endif
	} else if (name=="config.ini") {
		content.Replace("\r","",true);
		content+="\n";
		while (content.Contains('\n')) {
			wxString key=content.BeforeFirst('\n');
			content=content.AfterFirst('\n');
			if (!key.IsEmpty() && !key.StartsWith("#") && key.Contains("=")) 
				m_config[key.BeforeFirst('=').Trim(true).Trim(false).Lower()]=key.AfterFirst('=').Trim(true).Trim(false);
		}
	} else if (name.StartsWith("input") && name.EndsWith(".txt")) {
		wxString test_name = name.Mid(5,name.Len()-9);
		if (test_name.StartsWith("_")||test_name.StartsWith("-")) test_name=test_name.Mid(1);
		m_tests[test_name].input=content;
	} else if (name.StartsWith("output") && name.EndsWith(".txt")) {
		wxString test_name = name.Mid(6,name.Len()-10);
		if (test_name.StartsWith("_")||test_name.StartsWith("-")) test_name=test_name.Mid(1);
		m_tests[test_name].solution=content;
	} else if (name!="solution.psc") {
		std::cerr<<"Error in Package::ProcessFile: Unknown filename "<<name<<std::endl;
	}
}

bool Package::IsInConfig (const wxString & key) {
	return m_config.find(key.Lower())!=m_config.end();
}

wxString &Package::GetConfigStr(const wxString &key) {
	return m_config[key.Lower()];
}

long Package::GetConfigInt(const wxString & key) {
	long retval; 
	if (!GetConfigStr(key).ToLong(&retval)) return 0;
	return retval;
}

bool Package::GetConfigBool(const wxString & key) {
	wxString val = GetConfigStr(key).Lower();
	return val=="si"||val=="1"||val=="verdadero"||val=="yes"||val=="true"||val==_Z("sí");
}

Package::Package ( ) {
	Reset();
}

void Package::Reset ( ) {
	m_is_ok=false;
	m_help_text.Clear();
	m_tests.clear();
	m_base_psc.Clear();
	m_config.clear();
	m_images.Clear();
	SetConfigStr  ( "mensaje exito" , "El algoritmo es correcto" );
	SetConfigStr  ( "mensaje error" , "El algoritmo no es correcto" );
	SetConfigBool ( "mostrar soluciones" , true );
	SetConfigStr  ( "mostrar casos fallidos" , "primero" );
	SetConfigBool ( "mezclar casos", true );
	SetConfigInt  ( "version requerida" , PACKAGE_VERSION );
}

void Package::SetConfigStr (const wxString & key, const wxString & value) {
	m_config[key] = value;
}

void Package::SetConfigBool (const wxString & key, bool value) {
	m_config[key] = (value?"si":"no");
}

void Package::SetConfigInt (const wxString & key, long value) {
	m_config[key] = (wxString()<<value);
}

bool Package::SaveConfig (const wxString & fname) {
	wxTextFile fil(fname);
	if (fil.Exists()) fil.Open();
	else fil.Create();
	fil.Clear();
	std::map<wxString,wxString>::iterator it = m_config.begin();
	fil.AddLine(wxString("# generado por psEval ")<<VERSION<<"-" ARCHITECTURE ARCH_EXTRA);
	while (it!=m_config.end()) {
		fil.AddLine(wxString()+it->first+" = "+it->second);
		++it;
	}
	static wxCSConv cs("ISO-8859-1");
	fil.Write(wxTextFileType_None,cs);
	fil.Close();
	return true;
}

wxString Package::GetFullConfig() {
	SetConfigInt  ( "version requerida" , PACKAGE_VERSION );
	wxString ret = wxString("# generado por psEval ")<<VERSION<<"-" ARCHITECTURE ARCH_EXTRA<<"\n";
	std::map<wxString,wxString>::iterator it = m_config.begin();
	while (it!=m_config.end()) {
		ret<<wxString()<<it->first<<" = "<<it->second<<"\n";
		++it;
	}
	return ret;
}

bool Package::Save (const wxString & fname, const wxString & passkey, bool old_cypher) {
	wxFFileOutputStream out(fname);
	if (!old_cypher) out.Write(cypher_version_str,12);
	mxFilterOutputStream fout(out,passkey,old_cypher);
	wxZipOutputStream zip(fout);
	WriteTextFile(zip,"config.ini",GetFullConfig());
	if (!m_base_psc.IsEmpty()) WriteTextFile(zip,"base.psc",m_base_psc);
	if (!m_help_text.IsEmpty()) WriteTextFile(zip,"help.html",m_help_text);
	for(std::map<wxString,TestCase>::iterator it = m_tests.begin();it!=m_tests.end();++it) {
		WriteTextFile(zip,wxString("input-")+it->first+".txt",it->second.input);
		WriteTextFile(zip,wxString("output-")+it->first+".txt",it->second.solution);
	}
	for(unsigned int i=0;i<m_images.GetCount();i++) {
		wxFileInputStream is(m_images[i]);
		zip.PutNextEntry(m_images[i].AfterLast(wxFileName::GetPathSeparator()));
		zip<<is;
	}
	return true;
}

void Package::UnloadImages( ) {
	for(unsigned int i=0;i<m_images.GetCount();i++)
		wxMemoryFSHandler::RemoveFile(m_images[i]);
	m_images.Clear();
}

