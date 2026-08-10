#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/dataobj.h>
#include <wx/clipbrd.h>
#include <wx/textfile.h>
#include <wx/button.h>
#include "mxUtils.h"
#include "ConfigManager.h"
#include "string_conversions.h"
#include "ids.h"
#include "mxBitmapButton.h"

mxUtils *utils = NULL;

wxMenuItem *mxUtils::AddItemToMenu(wxMenu *menu, wxWindowID id,wxString caption, wxString help, wxString filename) {
	wxMenuItem *item = new wxMenuItem(menu,id,caption,help);
	filename = JoinDirAndFile(DIR_PLUS_FILE_2(config->images_path,"menu",config->big_icons?"24":"16"),filename);
	if (wxFileName::FileExists(filename))
		item->SetBitmap(wxBitmap(filename,wxBITMAP_TYPE_PNG));
	menu->Append (item);
	return item;
}

wxString mxUtils::JoinDirAndFile(wxString dir, wxString fil) {
	if (dir.Len()==0 || (fil.Len()>1 && (fil[0]=='\\' || fil[0]=='/' || fil[1]==':')))
		return fil;
	else if (dir.Last()==wxFileName::GetPathSeparator())
		return dir+fil;
	else
		return dir+wxFileName::GetPathSeparator()+fil;
}

void mxUtils::AddTool(wxToolBar *toolbar, wxWindowID id, wxString caption, wxString filename, wxString status_text) {
	filename = DIR_PLUS_FILE_3(config->images_path,"tools",config->big_icons?"32":"24",filename);
	if (wxFileName::FileExists(filename)) {
		toolbar->AddTool(id, caption, wxBitmap(filename,wxBITMAP_TYPE_PNG),caption);
		toolbar->SetToolLongHelp(id,status_text);
	}
}
void mxUtils::AddCheckTool(wxToolBar *toolbar, wxWindowID id, wxString caption, wxString filename, wxString status_text, bool checked) {
	filename = DIR_PLUS_FILE_3(config->images_path,"tools",config->big_icons?"32":"24",filename);
	if (wxFileName::FileExists(filename)) {
		toolbar->AddCheckTool(id, caption, wxBitmap(filename,wxBITMAP_TYPE_PNG), wxBitmap(),caption);
		toolbar->SetToolLongHelp(id,status_text);
		toolbar->ToggleTool(id,checked);
	}
}

wxButton *mxUtils::AddImgButton(wxSizer *sizer, wxWindow *parent, wxWindowID id, wxString file, wxString text) {
	wxString cfile = DIR_PLUS_FILE(config->images_path,file);
	wxButton *btn;
	if (wxFileName::FileExists(cfile))
		sizer->Add(btn=new mxBitmapButton(parent,id,new wxBitmap(cfile,wxBITMAP_TYPE_PNG),text,wxDefaultSize),wxSizerFlags().Proportion(1).Expand());
	else	
		sizer->Add(btn=new wxButton(parent,id,text),wxSizerFlags().Proportion(1).Expand());
//	sizer->Add(btn=new wxButton(parent,id,text),wxSizerFlags().Proportion(1).Expand());
//	if (wxFileName::FileExists(cfile))
//		btn->SetBitmap(wxBitmap(cfile,wxBITMAP_TYPE_PNG));
	return btn;
}

wxCheckBox *mxUtils::AddCheckBox (wxBoxSizer *sizer, wxWindow *panel, wxString text, bool value) {
	wxCheckBox *checkbox;
	sizer->Add(checkbox = new wxCheckBox(panel, wxID_ANY, text+"   "),wxSizerFlags().Border(wxALL, 5).Expand());
	checkbox->SetValue(value);
	return checkbox;	
}

bool mxUtils::IsTrue(wxString value) const {
	return (value[0]=='1' || value[0]=='V' || value[0]=='v' || value[0]=='s' || value[0]=='S' || value[0]=='T' || value[0]=='t');
}

wxMenuItem *mxUtils::AddCheckToMenu(wxMenu *menu, wxWindowID id,wxString caption, wxString help, bool value) {
	wxMenuItem *item = menu->AppendCheckItem (id, caption,help);
	item->Check(value);
	return item;
}

wxString mxUtils::GetOutput(wxString command) {
	wxString ret;
	wxArrayString output;
	wxExecute(command, output, wxEXEC_NODISABLE|wxEXEC_SYNC);
	for (unsigned int i=0;i<output.GetCount();i++)
		if (ret.Len()==0)
			ret<<output[i];
		else
			ret<<"\n"<<output[i];
	return ret;
}

/** 
* Devuelve una cadena convertida a HTML. Reemplaza los espacios y operadores por
* los codigos de escape adecuados (\&...).
* @param text La cadena a convertir
* @retval La cadena en formato HTML
**/
wxString mxUtils::ToHtml(wxString text) {
	wxString tabs="&nbsp;&nbsp;&nbsp;";
	text.Replace("&","&amp;");
	text.Replace("\t",tabs);
	text.Replace("<","&lt;");
	text.Replace(">","&gt;");
	text.Replace("\n","<BR>");
	wxString doce=" "; doce[0]=12;
	text.Replace(doce,"<BR>");
	text.Replace(" ","&nbsp;");
	return text;
}

wxString mxUtils::FixTooltip (wxString tooltip) {
#ifdef __WIN32__
	int p=0, l=tooltip.Len(),c=0;
	while (p<l) {
		if (c>50 && tooltip[p]==' ') {
			tooltip[p]='\n'; c=0;
		}
		c++; p++;
	}
#endif
	return tooltip;
}

wxString mxUtils::GetVersion(wxString exe) {
	wxArrayString out; // para que no muestre la consola (ver ayuda de wxExecute)
	wxString filename=DIR_PLUS_FILE(config->temp_dir,"version.tmp"), retval=exe+_Z(": error desconocido");
	if (wxFileName::FileExists(filename)) 
		wxRemoveFile(filename);
	if (wxFileName::FileExists(filename)) {
		retval=exe+_Z(": error 1: No se pudo determinar la versión.");
	} else {
		wxExecute(exe+" --version \""+filename+"\"",out,wxEXEC_SYNC|wxEXEC_NODISABLE);
		wxTextFile fil(filename);
		if (!fil.Exists()) {
			retval=exe+_Z(": error 2: No se pudo determinar la versión.");
		} else {
			fil.Open();
			if (!fil.GetLineCount()) {
				retval=exe+_Z(": error 3: No se pudo determinar la versión.");
			} else {
				retval=fil.GetFirstLine();
				fil.Close();
				if (!retval.Len()) retval=exe+_Z(": error 4: No se pudo determinar la versión");
			}
		}
	}
	return retval;
}

wxString mxUtils::GetExportLangName(int id) {
	switch(id) {
	case mxID_FILE_EXPORT_LANG_QBASIC: 	return _Z("Código QBasic (.bas)");
	case mxID_FILE_EXPORT_LANG_VB: 		return _Z("Código Visual Basic .NET (vb)");
	case mxID_FILE_EXPORT_LANG_C: 		return _Z("Código C (c)");
	case mxID_FILE_EXPORT_LANG_CSHARP: 	return _Z("Código C# (cs)");
	case mxID_FILE_EXPORT_LANG_CPP03: 	return _Z("Código C++ (cpp)");
	case mxID_FILE_EXPORT_LANG_PHP: 	return _Z("Código PHP (php)");
	case mxID_FILE_EXPORT_LANG_PY2: 	return _Z("Código Python 2 (py)");
	case mxID_FILE_EXPORT_LANG_PY3: 	return _Z("Código Python 3 (py)");
	case mxID_FILE_EXPORT_LANG_HTML: 	return _Z("Página HTML (JavaScript embebido) (html)");
	case mxID_FILE_EXPORT_LANG_JAVA: 	return _Z("Código Java (java)");
	case mxID_FILE_EXPORT_LANG_JS: 		return _Z("Código JavaScript (js)");
	case mxID_FILE_EXPORT_LANG_M: 		return _Z("Código MatLab (m)");
//	case mxID_FILE_EXPORT_LANG_PRG:		return _Z("Cisual Fox Pro (prg)");
	case mxID_FILE_EXPORT_LANG_PAS: 	return _Z("Código Pascal (pas)");
	default: return _Z("???");
	}
}

wxString mxUtils::GetExportLangCode(int id) {
	switch(id) {
	case mxID_FILE_EXPORT_LANG_QBASIC: 	return _T("bas");
	case mxID_FILE_EXPORT_LANG_VB: 		return _T("vb");
	case mxID_FILE_EXPORT_LANG_C: 		return _T("c");
	case mxID_FILE_EXPORT_LANG_CSHARP: 	return _T("cs");
	case mxID_FILE_EXPORT_LANG_CPP03: 	return _T("cpp03");
//	case mxID_FILE_EXPORT_LANG_CPP11: 	return _T("cpp11");
	case mxID_FILE_EXPORT_LANG_PHP: 	return _T("php");
	case mxID_FILE_EXPORT_LANG_PY2: 	return _T("py2");
	case mxID_FILE_EXPORT_LANG_PY3: 	return _T("py3");
	case mxID_FILE_EXPORT_LANG_HTML: 	return _T("html");
	case mxID_FILE_EXPORT_LANG_JAVA: 	return _T("java");
	case mxID_FILE_EXPORT_LANG_JS: 		return _T("js");
	case mxID_FILE_EXPORT_LANG_M: 		return _T("m");
//	case mxID_FILE_EXPORT_LANG_PRG:		return _T("prg");
	case mxID_FILE_EXPORT_LANG_PAS: 	return _T("pas");
	default: return _Z("???");
	}
}

wxString mxUtils::GetClipboardText() {
	wxString text;
	if (wxTheClipboard->Open()) {
		wxTextDataObject clip_data;
		if (wxTheClipboard->GetData(clip_data))
			text = clip_data.GetText();
		wxTheClipboard->Close();
	}
	return text;
}

void mxUtils::SetClipboardText(const wxString &text) {
	if (!wxTheClipboard->Open()) return;
	wxTheClipboard->SetData( new wxTextDataObject(text) );
	wxTheClipboard->Close();
}

