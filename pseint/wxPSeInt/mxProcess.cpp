#include "mxProcess.h"
#include "ConfigManager.h"
#include "mxUtils.h"

#include <wx/arrstr.h>
#include "mxMainWindow.h"
#include "mxStatusBar.h"
#include <wx/textfile.h>
#include "DebugManager.h"
#include <wx/filename.h>
#include "mxInputDialog.h"
#include "mxDebugWindow.h"
#include "CommunicationsManager.h"
#include "Logger.h"
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/txtstrm.h>

mxProcess *proc_list = NULL;

enum { mxPW_NULL=0, 
	mxPW_RUN, mxPW_DEBUG, 
	mxPW_CHECK, mxPW_CHECK_AND_RUN, mxPW_CHECK_AND_DEBUG, 
//	mxPW_DRAW, mxPW_CHECK_AND_DRAW, 
	mxPW_DRAWEDIT, mxPW_CHECK_AND_DRAWEDIT, 
	mxPW_EXPORT, mxPW_CHECK_AND_EXPORT, 
	mxPW_CHECK_AND_SAVEDRAW, mxPW_SAVEDRAW 
	};
	
mxProcess *proc_for_killing = NULL;

int mxProcess::cont = 0;

static void CheckDeps(wxString cmd) {
	_LOG("mxProcess::CheckDeps cmd="<<cmd);
	wxArrayString ostd,oerr,ofin;
	cmd=wxString("/usr/bin/ldd \"")+cmd+"\"";
	wxExecute(cmd,ostd,oerr);
	for(unsigned int i=0;i<ostd.GetCount();i++) if (ostd[i].Contains("not found")) ofin.Add(ostd[i]);
	for(unsigned int i=0;i<oerr.GetCount();i++) if (oerr[i].Contains("not found")) ofin.Add(oerr[i]);
	if (ofin.GetCount()==0) { 
		_LOG("mxProcess::CheckDeps ends no missing deps");
		return;
	}
	wxString msg;
	msg<<"Puede que su sistema no tenga todas las bibliotecas necesarias para ejecutar PSeInt.\n";
	msg<<"Instale las bibliotecas faltantes con el gestor de paquetes de su distribución.\n";
	msg<<"\nLas bibliotecas faltantes son:";
	_LOG("mxProcess::CheckDeps ends missing deps");
	for(unsigned int i=0;i<ofin.GetCount();i++) {
		_LOG("     "<<ofin[i]);
		msg<<"\n"<<ofin[i].BeforeFirst('.');
	}
	wxMessageBox(msg,"Error",wxOK|wxICON_ERROR);
}


mxProcess::mxProcess(mxSource *src) 
#ifdef __APPLE__
	// El redirect que sigue en teoría no debería ser necesario... pero sin esto
	// no funciona el wxExecute en Mac OS 10.13 en adelante con wx 2.8
	// Tal vez pueda sacarlo cuando migre a wx3??
	: wxProcess(wxPROCESS_REDIRECT) 
	// por otro lado, parece que a linux le es indiferente, pero en windows sí
	// molesta el redirect si no corresponde (por alguna extraña razón hay que 
	// lanzar 2 veces los procesos, solo la segunda funciona)
#endif
{
	
	_LOG("mxProcess::mxProcess this="<<this<<" src="<<src);
	
	prev=NULL;
	next=proc_list;
	proc_list=this;
	if (next)
		next->prev=this;
	filename = src->GetPageText();
	cont++;
	source = src;
	if (proc_for_killing) 
		delete proc_for_killing;
	what = mxPW_NULL;
	input = NULL;
	output = NULL;
}

mxProcess::~mxProcess() {
	
	_LOG("mxProcess::~mxProcess this="<<this);
	
	if (this==proc_for_killing) proc_for_killing=NULL;
	if (prev)
		prev->next=next;
	else
		proc_list=next;
	if (next)
		next->prev=prev;
}

static void lambda_ldd_psdraw() {
	CheckDeps(config->psdraw3_command);
}

void mxProcess::OnTerminate(int pid, int status) {
	_LOG("mxProcess::OnTerminate this="<<this<<" status="<<status);
	if (what==mxPW_DRAWEDIT) {
		// si psdraw3 sale con errores, ver si le faltaban dependencias
		if (status==127) main_window->SetLambda(lambda_ldd_psdraw);
//		else main_window->GetCurrentSource()->SetStatus(STATUS_BASE);
	}
	if (this==debug->process) {
		debug->debugging=false;
		debug_panel->SetState(DS_STOPPED);
		if (source) source->DebugMode(false);
		debug->process=NULL;
	}
	if (proc_for_killing) {
		delete proc_for_killing;
		proc_for_killing=NULL;
	}
	proc_for_killing=this;
}


static void Execute(const wxString &command, wxArrayString &output) {
	// ejecutar usando un wxProcess, y no pasandole el output a wxExecute
	// porque en ese caso no se puede controlar la codificación de la
	// salida y wxExecute simplemente se cuelga (al menos en GNU/Linux)
	// cuando la salida no es utf8
	wxProcess p; p.Redirect();
	wxExecute(command, wxEXEC_SYNC, &p);
	wxInputStream *i = p.GetInputStream();
	if(i) {
		wxTextInputStream t(*i, "\n",wxCSConv("ISO-8851"));
		while(!i->Eof()) {
			wxString s = t.ReadLine();
			if (!s.IsEmpty()) output.Add(s);
		}
	}
}

static bool IsError(const wxString &line) {
	return line.Find(": ERROR ")!=wxNOT_FOUND;
}

bool mxProcess::CheckSyntax(wxString file, wxString extra_args) {
	
	if (what==mxPW_NULL) what=mxPW_CHECK;
	
	wxString command;	
	command<<config->pseint_command<<_T(" --nouser --norun \"")<<file<<_T("\"");
	command<<" "<<GetProfileArgs();
	if (extra_args!=wxEmptyString) command<<" "<<extra_args;
	
	wxArrayString output;
	_LOG("mxProcess::CheckSyntax this="<<this);
	_LOG("    "<<command);
	
	
	Execute(command,output);
	
	main_window->RTreeReset();
	main_window->last_source=source;
	if (_avoid_results_tree) source->ClearErrorData();
	int errors_count = 0, warnings_count = 0;
	for (unsigned int i=0;i<output.GetCount();i++) {
		bool is_error = IsError(output[i]);
		main_window->RTreeAdd(output[i],mxMainWindow::RTAddType::Level1Node,source);
		++(is_error?errors_count:warnings_count);
	}
	if (errors_count!=0) {
		if (errors_count==1)
			main_window->RTreeAdd(filename+": Sintaxis Incorrecta: un error.",mxMainWindow::RTAddType::TreeRoot);
		else
			main_window->RTreeAdd(filename+wxString(": Sintaxis Incorrecta: ")<<output.GetCount()<<" errores.",mxMainWindow::RTAddType::TreeRoot);
		if (source) source->SetStatus(STATUS_SYNTAX_CHECK_ERROR);
		main_window->RTreeAdd("",mxMainWindow::RTAddType::OutOfTree);
		main_window->RTreeAdd("Las lineas con errores se marcan con una cruz sobre el margen izquierdo. Seleccione un error para ver su descripción:",mxMainWindow::RTAddType::OutOfTree);
		main_window->RTreeDone(true,true);
		proc_for_killing = this;
	} else {
		if (!source) return false; // si el fuente se cerro mientras se analizaba (muy poco probable)
		main_window->RTreeAdd(filename+": Sintaxis Correcta",mxMainWindow::RTAddType::TreeRoot);
		if (what==mxPW_CHECK_AND_RUN)
			return Run(file,false);
		else if (what==mxPW_CHECK_AND_DEBUG)
			return Debug(file,false);
//		else if (what==mxPW_CHECK_AND_DRAW)
//			return Draw(file,false);
		else if (what==mxPW_CHECK_AND_DRAWEDIT)
			return DrawAndEdit(file,false);
		else if (what==mxPW_CHECK_AND_SAVEDRAW)
			return SaveDraw(file,false);
		else if (what==mxPW_CHECK_AND_EXPORT)
			return ExportLang(file,export_lang,false);
		else if (what==mxPW_CHECK) {
			source->SetStatus(STATUS_SYNTAX_CHECK_OK);
			main_window->RTreeAdd("Presione F9 para ejecutar el algoritmo.",mxMainWindow::RTAddType::OutOfTree);
			main_window->RTreeDone(true,false);
		}
	}
	if (_avoid_results_tree) source->ClearErrorMarks();
	return errors_count==0;
}

bool mxProcess::Run(wxString file, bool check_first) {
	what = check_first?mxPW_CHECK_AND_RUN:mxPW_RUN;
	source->StopReloadTimer();
	if (check_first) return CheckSyntax(file);
	wxString command, tty_command=config->GetTermCommand();
	if (tty_command.Len()) {
		command<<tty_command<<_T(" ");
		command.Replace(_T("$name"),_T("Ejecucion"));
	}
	command<<_T(" --port=")<<comm_manager->GetServerPort()<<" --id="<<source->GetId()<<" ";
	temp = source->GetTempFilenameOUT();
	command<<"-- "<<config->pseint_command<<_T(" --nocheck \"")<<file<<_T("\" \"")<<temp<<_T("\"");
	if (config->use_colors) command<<_T(" --color");
	command<<_T(" --forpseintterminal --withioreferences");
	command<<" "<<GetProfileArgs()<<" "<<GetInputArgs();
	if (source)	source->SetStatus(STATUS_RUNNING);
	_LOG("mxProcess::Run this="<<this);
	_LOG("    "<<command);
	return wxExecute(command, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, this)!=0;
}

bool mxProcess::Debug(wxString file, bool check_first) {
	what = check_first?mxPW_CHECK_AND_DEBUG:mxPW_DEBUG;
	if (check_first) 
		return CheckSyntax(file);
	wxString command, tty_command=config->GetTermCommand();
	if (tty_command.Len()) {
		command<<tty_command<<_T(" ");
		command.Replace(_T("$name"),_T("Ejecucion"));
	}
	command<<_T(" --debugmode ");
	if (config->reorganize_for_debug) {
		int x,y,h,w, delta=10;
		main_window->notebook->GetScreenPosition(&x,&y);
		main_window->notebook->GetSize(&w,&h);
		command<<_T(" --right=")<<x+w-delta<<_T(" ");
		command<<_T(" --top=")<<y+delta<<_T(" ");
		if (w-500<400) command<<_T("--width=400 ");
	}
	command<<_T(" --alwaysontop ");
	temp = source->GetTempFilenameOUT();
	debug->Start(this,source);
	int port=comm_manager->GetServerPort();
	int delay=_calc_delay(config->stepstep_tspeed);
	debug_panel->SetSpeed(debug->GetSpeed(delay));
	command<<"-- "<<config->pseint_command<<_T(" --port=")<<port<<_T(" --delay=")<<delay<<_T(" --nocheck \"")<<file<<_T("\" \"")<<temp<<_T("\"");
	if (config->use_colors) command<<_T(" --color");
	command<<_T(" --forpseintterminal");
	command<<" "<<GetProfileArgs()<<" "<<GetInputArgs();
//	was_readonly = source->GetReadOnly();
//	if (pid) source->SetReadOnly(true);
	source->DebugMode(true);
	_LOG("mxProcess::Debug this="<<this);
	_LOG("    "<<command);
	pid = wxExecute(command, wxEXEC_ASYNC|wxEXEC_MAKE_GROUP_LEADER, this);
	return pid!=0;
}

bool mxProcess::DrawAndEdit(wxString file, bool check_first) {
	what = check_first?mxPW_CHECK_AND_DRAWEDIT:mxPW_DRAWEDIT;
	if (check_first) return CheckSyntax(file,GetDrawPreArgs()+" --writepositions \""+source->GetTempFilenamePSD()+"\"");
	wxString command;
	command<<config->psdraw3_command;
	command<<" --port="<<comm_manager->GetServerPort()<<" --id="<<source->GetId();
	if (config->big_icons) command<<" --bigicons";
	if (config->use_dark_psdraw) command<<" --darktheme";
	if (source->GetReadOnly()) command<<" --noedit";
	command<<" "<<GetDrawPostArgs();
	command<<_T(" \"")<<source->GetTempFilenamePSD()<<_T("\"");
	_LOG("mxProcess::DrawAndEdit this="<<this);
	_LOG("    "<<command);
	return wxExecute(command, wxEXEC_ASYNC, this)!=0;
}

bool mxProcess::SaveDraw(wxString file, bool check_first) {
	what = check_first?mxPW_CHECK_AND_SAVEDRAW:mxPW_SAVEDRAW;
	if (check_first) return CheckSyntax(file,GetDrawPreArgs()+" \""+source->GetTempFilenamePSD()+"\"");
	wxString command;
	command<<config->psdrawe_command<<_T(" \"")<<source->GetTempFilenamePSD()<<_T("\" ");
	command<<_("\"")<<DIR_PLUS_FILE(source->GetPathForExport(),source->GetNameForExport()+_T(".png"))<<_T("\"");
	command<<" "<<GetDrawPostArgs();
	_LOG("mxProcess::SaveDraw this="<<this);
	_LOG("    "<<command);
	return wxExecute(command, wxEXEC_ASYNC, this)!=0;
}

bool mxProcess::ExportLang(wxString file, wxString lang, bool check_first) {
	what = check_first?mxPW_CHECK_AND_EXPORT:mxPW_EXPORT; export_lang=lang;
	if (check_first) return CheckSyntax(file,wxString("--export --preservecomments z\"")<<source->GetTempFilenamePSD()<<"\"");
	wxString extension=export_lang; while(extension.Last()>='0'&&extension.Last()<='9') extension.RemoveLast();
	wxFileDialog dlg (main_window, _T("Guardar Cpp"),source->GetPathForExport(),source->GetNameForExport()+"."+extension, wxString("Archivo ")+extension+"|*."+extension, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() != wxID_OK) return false;
	config->last_dir=wxFileName(dlg.GetPath()).GetPath();
	wxString command;
	command<<config->psexport_command<<_T(" \"")<<source->GetTempFilenamePSD()<<_T("\" \"")<<dlg.GetPath()<<_T("\"");
	if (cfg_lang[LS_BASE_ZERO_ARRAYS]) command<<_T(" --base_zero_arrays=1");
	if (lang.size()) command<<" --lang="<<lang;
	_LOG("mxProcess::ExportCpp this="<<this);
	_LOG("    "<<command);
	return wxExecute(command, wxEXEC_ASYNC|wxEXEC_HIDE_CONSOLE, this)!=0;
}

wxString mxProcess::GetProfileArgs() {
	return wxString("--binprofile=")+cfg_lang.GetAsSingleString().c_str();
}

wxString mxProcess::GetInputArgs() {
	if (!source->input || !source->input->HasInput()) return "";
	wxString args(source->input->IsPartial()?"":"--noinput ");
	wxString values=source->input->GetInput();
	if (!values.Len()) return args;
	values.Replace("\r","");
	if (values.EndsWith("\n")) values.RemoveLast();
	values.Replace("\\","\\\\");
	values.Replace("\"","\\\"");
	values.Replace("\n","\" \"--input=");
	args<<"\"--input="<<values<<"\"";
	return args;
}

void mxProcess::SetSourceDeleted ( ) {
	source=NULL;
}

wxString mxProcess::GetDrawPostArgs ( ) {
	wxString command;
	if (config->psdraw_nocrop) command<<" --nocroplabels";
	if (config->shape_colors) command<<" --shapecolors";
	command<<" "<<GetProfileArgs();
//	if (config->lang[LS_USE_NASSI_SHNEIDERMAN]) command<<" --use_nassi_shneiderman=1";
//	if (config->lang[LS_USE_ALTERNATIVE_IO_SHAPES]) command<<" --use_alternative_io_shapes=1";
//	if (config->lang[LS_FORCE_SEMICOLON]) command<<" --force_semicolon=1";
//	if (!config->lang[LS_WORD_OPERATORS]) command<<" --word_operators=0";
//	if (!config->lang[LS_ENABLE_USER_FUNCTIONS]) command<<" --enable_user_functions=0";
	return command;
}

wxString mxProcess::GetDrawPreArgs ( ) {
	wxString command;
	command << "--preservecomments --draw --lazy_syntax=1 --force_semicolon=0 --allow_dinamyc_dimensions=1";
	return command;
}

