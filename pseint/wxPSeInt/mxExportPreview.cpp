#include "mxExportPreview.h"
#include <wx/sizer.h>
#include "ids.h"
#include "mxUtils.h"
#include <wx/combobox.h>
#include <wx/stattext.h>
#include "mxMainWindow.h"
#include "string_conversions.h"
#include "ConfigManager.h"
#include "mxProcess.h"
#include "Logger.h"
#include <wx/bmpbuttn.h>
#include <wx/settings.h>
#include <wx/filedlg.h>

mxExportPreview *export_preview = nullptr;

BEGIN_EVENT_TABLE(mxExportPreview,wxFrame)
	EVT_BUTTON(mxID_EXPPREV_SAVE,mxExportPreview::OnButtonSave)
	EVT_BUTTON(mxID_EXPPREV_UPDATE,mxExportPreview::OnButtonUpdate)
	EVT_BUTTON(mxID_EXPPREV_COPY,mxExportPreview::OnButtonCopy)
	EVT_COMBOBOX(mxID_EXPPREV_LANG,mxExportPreview::OnComboLang)
	EVT_CLOSE(mxExportPreview::OnClose)
	EVT_END_PROCESS(wxID_ANY,mxExportPreview::OnProcTerminate)
END_EVENT_TABLE()

mxExportPreview::mxExportPreview():wxFrame(main_window,wxID_ANY,_Z("Exportar - Vista previa"),wxDefaultPosition,wxDefaultSize) {
	
	export_preview = this;
	
	pid=0; state=mxEP_NONE;
	
//	static int tid=0; 
	temp_filename = DIR_PLUS_FILE(config->temp_dir,wxString("temp_ep_")/*<<tid++*/);
	
	wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizerFlags sz; sz.Center();
	top_sizer->Add( new wxStaticText(this,wxID_ANY,_Z(" Lenguaje: ")),sz );
	wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
	
	wxArrayString langs_list;
	for (int id = mxID_FILE_EXPORT_LANG_FIRST+1; id < int(mxID_FILE_EXPORT_LANG_LAST); id++)
		langs_list.Add(utils->GetExportLangName(id).AfterFirst(' '));
	combo_lang = new wxComboBox(this,mxID_EXPPREV_LANG,"",wxDefaultPosition,wxDefaultSize,langs_list,wxCB_READONLY);
	combo_lang->Select(mxID_FILE_EXPORT_LANG_CPP03-mxID_FILE_EXPORT_LANG_FIRST-1);
	
	wxString img_path = DIR_PLUS_FILE_2(config->images_path,"tools",config->big_icons?"32":"24");
	top_sizer->Add(combo_lang,sz);
	top_sizer->Add(new wxButton(this,mxID_EXPPREV_UPDATE,_Z("Actualizar")),sz);
	top_sizer->AddStretchSpacer();
	top_sizer->Add(	new wxBitmapButton(this,mxID_EXPPREV_COPY,wxBitmap(DIR_PLUS_FILE(img_path,"copiar.png"),wxBITMAP_TYPE_PNG)), sz);
	top_sizer->Add(	new wxBitmapButton(this,mxID_EXPPREV_SAVE,wxBitmap(DIR_PLUS_FILE(img_path,"guardar.png"),wxBITMAP_TYPE_PNG)), sz);
	main_sizer->Add(top_sizer,wxSizerFlags().Proportion(0).Expand());
	
	code_ctrl = new wxStyledTextCtrl(this,wxID_ANY);
	code_ctrl->SetMarginType (0, wxSTC_MARGIN_NUMBER);
	code_ctrl->SetMarginWidth (0, code_ctrl->TextWidth (wxSTC_STYLE_LINENUMBER, " XXXXX"));

	wxFont font(wxFontInfo(config->wx_font_size).Family(wxFONTFAMILY_MODERN));
	
	const char *CL_REG_BACK  = config->use_dark_theme ? "#333333" : "#FFFFFF" ;
	const char *CL_REG_FORE  = config->use_dark_theme ? "#FAFAFA" : "#000000" ;
	const char *CL_ALT_FORE  = config->use_dark_theme ? "#999999" : "#969696" ;
	const char *CL_ALT_BACK  = config->use_dark_theme ? "#606060" : "#D3D3D3" ;
	for(int s=0;s<wxSTC_STYLE_MAX;++s) {
		code_ctrl->StyleSetFont (s, font);
		code_ctrl->StyleSetBackground (s, CL_REG_BACK);
		code_ctrl->StyleSetForeground (s, CL_REG_FORE);
	}
	code_ctrl->StyleSetForeground (wxSTC_STYLE_LINENUMBER, CL_ALT_FORE);
	code_ctrl->StyleSetBackground (wxSTC_STYLE_LINENUMBER, CL_REG_BACK);
	code_ctrl->SetSelBackground(true,CL_ALT_BACK);
	code_ctrl->SetTabWidth(4);
	
	main_sizer->Add(code_ctrl,wxSizerFlags().Proportion(1).Expand());
	
	SetSizer(main_sizer);
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	Show();
	UpdatePrev();
}

void mxExportPreview::OnButtonUpdate (wxCommandEvent & event) {
	UpdatePrev();
}

void mxExportPreview::OnComboLang (wxCommandEvent & event) {
	UpdatePrev();
}

void mxExportPreview::OnClose (wxCloseEvent & event) {
	export_preview=nullptr; Destroy();
}

void mxExportPreview::UpdatePrev ( ) {
	if (state!=mxEP_NONE && pid>0) {
		state=mxEP_UPDATE;
		the_process->Kill(pid,wxSIGKILL);
		return;
	}
	the_process = new wxProcess(this->GetEventHandler());
	
	mxSource * src = main_window->GetCurrentSource();
	if (!src) { SetMessage(_Z("No hay pseudocódigo para exportar")); state=mxEP_NONE; return; }
	
	wxString command;
	command<<config->pseint_command<<_T(" --preservecomments --nouser --norun \"")<<src->SaveTemp()<<_T("\" ");
	command<<mxProcess::GetProfileArgs();
	command<<" --export \""<<temp_filename<<".psd"<<"\"";
	
	_LOG("mxExportPreview, command="<<command);
#ifdef __APPLE__
	the_process->Redirect(); // no necesito la salida, pero sin esto en mac y con wx8 execute no funciona
#endif
	pid = wxExecute(command,wxEXEC_ASYNC|wxEXEC_HIDE_CONSOLE,the_process);
	if (pid<=0) { SetMessage(_Z("Error al intentar exportar.")); return; }
	SetMessage("Actualizando...");
	state=mxEP_CHECK;
}

mxExportPreview::~mxExportPreview ( ) {
	if (wxFileName::FileExists(temp_filename+".psd")) wxRemoveFile(temp_filename+".psd");
	if (wxFileName::FileExists(temp_filename+".exp")) wxRemoveFile(temp_filename+".exp");
}

void mxExportPreview::OnProcTerminate (wxProcessEvent & event) {
_LOG("mxExportPreview::OnProcessTerminate");
	if (pid<=0||!the_process||event.GetPid()!=pid) return;
	int exit_code = event.GetExitCode(); delete the_process; the_process=NULL;
	if (exit_code!=0) { SetMessage(_Z("Error al intentar exportar.\nSi el pseudocódigo es correcto, intente nuevamente\nhaciendo click en el botón \"Actualizar\".")); state=mxEP_NONE; return; }
	
	switch(state) {
		
	case mxEP_NONE: { // no deberia pasar nunca
		return; 
	} break;
		
	case mxEP_CHECK: { // estaba armando el psd, ahora exportar
		wxString command;
		command<<config->psexport_command<<_T(" \"")<<temp_filename<<".psd"<<_T("\" \"")<<temp_filename<<".exp"<<_T("\"");
		if (cfg_lang[LS_BASE_ZERO_ARRAYS]) command<<_T(" --base_zero_arrays=1");
		int lang_id = mxID_FILE_EXPORT_LANG_FIRST+1+combo_lang->GetCurrentSelection();
		command<<" --lang="<<utils->GetExportLangCode(lang_id);
		
		the_process = new wxProcess(this->GetEventHandler());
#ifdef __APPLE__
		the_process->Redirect(); // no necesito la salida, pero sin esto en mac y con wx8 execute no funciona
#endif
		_LOG("mxExportPreview, command="<<command);
		pid = wxExecute(command,wxEXEC_ASYNC|wxEXEC_HIDE_CONSOLE,the_process);
		state = mxEP_EXP;
		
	} break;
		
	case mxEP_EXP: { // estaba exportando, mostrar
		state = mxEP_NONE;
		code_ctrl->LoadFile(temp_filename+".exp");
	} break;
		
	case mxEP_UPDATE: {  // se interrumpio lo que estaba haciendo para empezar otra vez
		pid = 0;
		state = mxEP_NONE;
		UpdatePrev();
	} break;
		
	default: break;
	}
}

void mxExportPreview::SetMessage (const wxString & msg) {
	code_ctrl->SetText(msg);
}

void mxExportPreview::OnButtonSave (wxCommandEvent & event) {
	int lang_id = mxID_FILE_EXPORT_LANG_FIRST+1+combo_lang->GetCurrentSelection();
	
	wxString extension = utils->GetExportLangCode(lang_id);
	while(extension.Last()>='0'&&extension.Last()<='9') extension.RemoveLast();
	
	wxString filename = "sin_titulo."+extension;
	mxSource *src = main_window->GetCurrentSource();
	if (src) filename = src->GetNameForExport()+extension;
		
	wxFileDialog dlg (main_window, _Z("Guardar Código Exportado"),
					  config->last_dir, filename,
					  wxString("Archivo ")+extension+"|*."+extension,
					  wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() != wxID_OK) return;
	config->last_dir=wxFileName(dlg.GetPath()).GetPath();
	
	code_ctrl->SaveFile(dlg.GetPath());
}

void mxExportPreview::OnButtonCopy (wxCommandEvent & event) {
	code_ctrl->SelectAll();
	code_ctrl->Copy();
}

