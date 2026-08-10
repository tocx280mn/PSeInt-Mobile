#include <wx/textfile.h>
#include <wx/stattext.h>
#include <wx/textdlg.h>
#include <wx/utils.h>
#include <wx/bitmap.h>
#include "version.h"
#include "ids.h"
#include "string_conversions.h"
#include "Logger.h"
#include "mxUpdatesChecker.h"
#include "mxMainWindow.h"
#include "mxUtils.h"
#include "ConfigManager.h"
#include "mxHelpWindow.h"
#include "mxArt.h"
#include "mxStatusBar.h"
#include <wx/txtstrm.h>

BEGIN_EVENT_TABLE(mxUpdatesChecker, wxDialog)
	EVT_BUTTON(wxID_CANCEL,mxUpdatesChecker::OnCloseButton)
	EVT_BUTTON(wxID_OK,mxUpdatesChecker::OnChangesButton)
//	EVT_BUTTON(wxID_FIND,mxUpdatesChecker::OnProxyButton)
	EVT_CLOSE(mxUpdatesChecker::OnClose)
	EVT_END_PROCESS(wxID_ANY,mxUpdatesChecker::OnProcessEnds)
END_EVENT_TABLE()

wxProcess *mxUpdatesChecker::m_process = nullptr;
mxUpdatesChecker::Command mxUpdatesChecker::m_command = mxUpdatesChecker::Command::Autodetect;

static const wxString &getTmpFile() {
	static wxString fname = utils->JoinDirAndFile(config->temp_dir,"update.tmp");
	return fname;
}

static const long getCurrentVersion() {
	long ver = 0;
	if (ver!=0) return ver;
	
	wxTextFile fil("version");
	if ((not fil.Exists()) or (not fil.Open())
		or fil.GetLineCount()==0
		or (not fil.GetFirstLine().ToLong(&ver)) )
			return ver = -1;
	
	return ver;
}

	
mxUpdatesChecker::mxUpdatesChecker(bool show) : wxDialog(main_window, wxID_ANY, "Buscar Actualizaciones", wxDefaultPosition, wxSize(450,150) ,wxALWAYS_SHOW_SB | wxALWAYS_SHOW_SB | wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER) {

	_LOG("mxUpdatesChecker::mxUpdatesChecker show="<<(show?"true":"false"));
	
	m_shown = show;
	wxBoxSizer *mySizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	m_check = new wxCheckBox(this,wxID_ANY,_Z("Verificar periódicamente al iniciar"));
	m_check->SetValue(config->check_for_updates);
	
	m_text = new wxStaticText(this,wxID_ANY,"Consultando web...",wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	mySizer->AddStretchSpacer();
	mySizer->Add(m_text,wxSizerFlags().Border(wxALL,5).Expand().Proportion(0));
	mySizer->AddStretchSpacer();
	mySizer->Add(m_check,wxSizerFlags().Border(wxALL,5));
	
	m_close_button = new wxButton (this,wxID_CANCEL,"&Cerrar");
	m_close_button->SetBitmap(*bitmaps->buttons.cancel);
	m_changes_button = new wxButton (this,wxID_OK,"Ir al &sitio...");
	m_changes_button->SetBitmap(*bitmaps->buttons.ok);
//	proxy_button = new wxButton (this,wxID_FIND,"Conf. Proxy...");
//	proxy_button->SetBitmap(*bitmaps->buttons.options);
	buttonSizer->Add(m_changes_button,wxSizerFlags().Border(wxALL,5));
//	buttonSizer->Add(proxy_button,wxSizerFlags().Border(wxALL,5));
	buttonSizer->Add(m_close_button,wxSizerFlags().Border(wxALL,5));
	mySizer->Add(buttonSizer,wxSizerFlags().Right());
	
	SetSizer(mySizer);
	m_changes_button->Hide();
	if (show) { 
		Show();
		m_close_button->SetFocus();
		wxYield();
	}
	CheckNow();
}

bool mxUpdatesChecker::IsAvailable() {
#if defined(__WIN32__) || defined(__WIN64__)
	m_command = Command::None;
#else
	if (m_command==Command::Autodetect) {
		wxArrayString output; // no lo necesito, pero es para esconderlo y que no moleste en la terminal
		_LOG("mxUpdatesChecker::IsAvailable: detectando herramienta auxiliar...");
		if (wxExecute("dssacurl --help",output,wxEXEC_SYNC|wxEXEC_HIDE_CONSOLE)!=-1) {
			m_command = Command::CURL;
			_LOG("    resultado: curl");
		}
		else if (wxExecute("wget --help",output,wxEXEC_SYNC|wxEXEC_HIDE_CONSOLE)!=-1) {
			m_command = Command::WGET;
			_LOG("    resultado: wget");
		}
		else {
			m_command = Command::None;
			_LOG("    resultado: Ninguna");
		}
	}
#endif
	return m_command!=Command::None;
}

void mxUpdatesChecker::CheckNow() {
	
	IsAvailable();
	wxString cmd;
	if (m_command==Command::WGET) cmd = "wget -q -O \"$\" https://pseint.sourceforge.net/version";
	else if (m_command==Command::CURL) cmd = "curl https://pseint.sourceforge.net/version -o \"$\"";
	else return;
	
	cmd.Replace("$",getTmpFile());
	if (wxFileExists(getTmpFile())) wxRemoveFile(getTmpFile());
	
	m_text->SetLabel(_Z("Consultando web..."));
	if (m_process) m_process->Detach();
	m_process = new wxProcess(this->GetEventHandler(),wxID_ANY);
	m_process->Redirect(); // no requerido, pero para que la salida no moleste en el log
	
	_LOG("mxUpdatesChecker::CheckNow "<<cmd);
	if (wxExecute(cmd, wxEXEC_ASYNC|wxEXEC_HIDE_CONSOLE, m_process)<=0) {
		if (m_shown) {
			m_text->SetLabel(_Z("Error al conectarse al servidor."));
		} else 
			Destroy();
	}
}

void mxUpdatesChecker::OnClose(wxCloseEvent &evt) {
	config->check_for_updates = m_check->GetValue();
	if (m_process) { m_process->Detach(); m_process = nullptr; }
	Destroy();
}

//void mxUpdatesChecker::OnProxyButton(wxCommandEvent &evt) {
//	wxString res = wxGetTextFromUser(_Z("Ingrese la direccion del proxy ( ej: 192.168.0.120:3128 ):"),_Z("Buscar Actualizaciones"), config->proxy, this);
//	config->proxy=res;
//	CheckNow();
//}

void mxUpdatesChecker::OnCloseButton(wxCommandEvent &evt) {
	Close();
}

void mxUpdatesChecker::OnChangesButton(wxCommandEvent &evt) {
#ifdef __WIN3__
	bool do_exit = wxMessageBox(_Z("Si decide actualizar PSeInt ahora, deberá cerrar \n"
								   "esta instancia antes de ejecutar el nuevo instalador.\n\n"
								   "¿Desea cerrar PSeInt ahora?"),_Z("Actualización"),wxYES_NO|wxICON_QUESTION,this)==wxYES;
#else
	bool do_exit = false;
#endif
	wxLaunchDefaultBrowser("https://pseint.sourceforge.net?page=actualizacion.php&os=" ARCHITECTURE);
	Close();
	if (do_exit) main_window->OnFileClose(evt);
}

void mxUpdatesChecker::BackgroundCheck() {
	if (not config->check_for_updates) return;
	
	wxDateTime dt_now = wxDateTime::Now();
	int i_now = dt_now.GetYear()*10000 + (dt_now.GetMonth()+1)*100 + dt_now.GetDay();
	if (config->last_update_check<i_now+5) return; // +7 = no todos los días
	config->last_update_check = i_now;
	if (not mxUpdatesChecker::IsAvailable()) return;
	status_bar->SetStatus(STATUS_UPDATE_CHECKING);
	new mxUpdatesChecker(false);
}

void mxUpdatesChecker::OnProcessEnds(wxProcessEvent &evt) {
	_LOG("mxUpdatesChecker::OnProcessEnds");
	delete m_process; m_process = nullptr;
	
	wxTextFile fil(getTmpFile()); long newver;
	if ((not fil.Exists()) or (not fil.Open())
		or fil.GetLineCount()==0
		or (not fil.GetFirstLine().StartsWith("ZVERSION="))
		or (not fil.GetFirstLine().Mid(9,8).ToLong(&newver)) )
	{
			m_text->SetLabel(_Z("Error al conectarse al servidor."));
			status_bar->SetStatus(STATUS_UPDATE_ERROR);
	} else {
		
		if (newver>getCurrentVersion()) {
			status_bar->SetStatus(STATUS_UPDATE_FOUND);
			wxString str;
			str<<_Z("Hay una nueva version disponible en\nhttps://pseint.sourceforge.net (")<<newver<<_Z(")");
			m_text->SetLabel(str);
			//		proxy_button->Hide();
			m_changes_button->Show();
			GetSizer()->Layout();
			if (!m_shown) Show();
			m_changes_button->SetFocus();
		} else {
			status_bar->SetStatus(STATUS_UPDATE_NONEWS);
			m_text->SetLabel(_Z("No hay nuevas versiones disponibles."));
			GetSizer()->Layout();
			if (!m_shown) Destroy();
		}
	}
}

