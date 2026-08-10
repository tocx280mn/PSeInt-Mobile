#include <csignal>
#include <wx/strconv.h>
#include <wx/utils.h>
#include "error_recovery.h"
#include "mxSource.h"
#include "mxMainWindow.h"
#include "Logger.h"
#include "string_conversions.h"
#include <wx/msgdlg.h>

std::string er_dir;
er_source_register *er_first_source;

#define ERR_REC_LOG_BOOL(what) fil1<<#what<<": "<<((what)?"true":"false")<<std::endl
#define ERR_REC_LOG_NORM(what) fil1<<#what<<": "<<what<<std::endl

#ifndef SIGPIPE
#define SIGPIPE 13
#endif

er_source_register::er_source_register(mxSource *s, er_source_register *p, er_source_register *n) {
	if ((next=n)) next->prev=this;
	if ((prev=p)) prev->next=this;
	src=s;
}

er_source_register::~er_source_register() {
	if (prev) prev->next=next;
	if (next) next->prev=prev;
}

void er_register_source(mxSource *src) {
	src->er_register = new er_source_register(src,er_first_source,er_first_source->next);
}
void er_unregister_source(mxSource *src) {
	delete src->er_register;
}

void er_sigsev(int sig) {
	
	std::ofstream fil2(er_get_recovery_fname(), std::ios::trunc);
	fil2 << "Error date: " << wxNow() << std::endl;
	er_source_register *sr = er_first_source->next;
	char kname[]="kabom-aaa.psc";
	while (sr) {
		if (!sr->src->IsExample()) {
			if (kname[6]=='z') {
				kname[6]='a';
				if (kname[7]=='z') {
					kname[7]='a';
					kname[8]++;
				} else
					kname[7]++;
				
			} else
				kname[6]++;
			fil2 << sr->src->GetPageText() << std::endl;
			fil2 << (sr->src->sin_titulo?"":sr->src->filename) << std::endl;
			fil2 << (er_dir+kname).c_str() << std::endl;
			sr->src->SaveFile(wxString((er_dir+kname).c_str(),wxConvLibc));
		}
		sr = sr->next;
	}
	fil2.close();
	
	_LOG("ERROR RECOVERY!!!");
	_LOG("Error date: " << wxNow());
	_LOG("signal: " << sig);
	
	if (main_window) {
		wxMessageDialog	(main_window,_Z("Ha ocurrido un error grave y PSeInt se cerrará. Por,\n"
									   "suerte se han podido guardar todos sus archivos para\n"
									   "ser recuperados la proxima vez que inicie el entorno."),
						_Z("OOOPS!!!"),wxOK|wxICON_ERROR).ShowModal();
	}
	exit(sig);
}

void er_init(const char *dir) {
	er_dir = dir;
	if (er_dir[er_dir.size()-1]!='/' && er_dir[er_dir.size()-1]!='\\') {
#ifdef __WIN32__
		er_dir+="\\";
#else
		er_dir+="/";
#endif
	}
	er_first_source = new er_source_register(NULL,NULL,NULL);
#ifndef DEBUG
	signal(SIGSEGV,er_sigsev);
	signal(SIGPIPE,er_sigsev);
#endif
}


void er_uninit() {
	signal(SIGSEGV,NULL);
	signal(SIGPIPE,NULL);
}

const char * er_get_recovery_fname( ) {
	static std::string rec_fname = er_dir+"recovery_log";
	return rec_fname.c_str();
}

