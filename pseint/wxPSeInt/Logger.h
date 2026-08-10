#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <fstream>

class Logger {
private:
	std::ofstream file;
protected:
public:
	Logger(const char *where);
	void Write(const wxString &s);
	static void DumpVersions();
	~Logger();
};

inline wxString &operator<<(wxString &s, void *p) { return s<<reinterpret_cast<unsigned long long>(p); }

extern Logger *logger;
#ifdef DEBUG
#	include<iostream>
#	define _LOG(x) std::cerr<<x<<std::endl; if (logger) logger->Write(wxString()<<x)
#else
#	define _LOG(x) if (logger) logger->Write(wxString()<<x)
#endif

#endif

