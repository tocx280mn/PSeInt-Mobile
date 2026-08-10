#include <wx/msgdlg.h>
#include <wx/datetime.h>
#include <wx/string.h>
#include "Logger.h"
#include "version.h"
#include "mxUtils.h"
#include "ConfigManager.h"

Logger *logger = NULL;

Logger::Logger(const char *where) {
	file.open(where,std::ios::app);
	if (file.is_open()) logger=this; else wxMessageBox(wxString("No se pudo abrir el archivo de log: ")<<where);
	file << std::endl << "LOG START" << std::endl;
	Write(wxString() << "version " << VERSION << '-' << ARCHITECTURE ARCH_EXTRA);
}

Logger::~Logger() {
	file << "LOG END" << std::endl << std::endl;
	file.close();
}



void Logger::Write (const wxString & s) {
	file << "   " << wxDateTime::Now().FormatISOTime().c_str() << "  " << s.c_str() << std::endl;
	file.flush();
}

void Logger::DumpVersions ( ) {
	_LOG("Logger::DumpVersions");
	_LOG("   pseint: "<<utils->GetVersion(config->pseint_command));
	_LOG("   psterm: "<<utils->GetVersion(config->psterm_command));
	_LOG("   psdraw3: "<<utils->GetVersion(config->psdraw3_command));
	_LOG("   psdrawe: "<<utils->GetVersion(config->psdrawe_command));
	_LOG("   psexport: "<<utils->GetVersion(config->psexport_command));	
}

