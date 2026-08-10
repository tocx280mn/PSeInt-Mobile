#ifndef VERSION_COMMON_H
#define VERSION_COMMON_H

#if defined(__WIN32__)
#	define ARCHITECTURE "w32"
#elif defined(__WIN64__)
#	define ARCHITECTURE "w64"
#elif defined(__APPLE__)
#	define ARCHITECTURE "mac"
#elif defined(__linux__) && defined(__x86_64__)
#	define ARCHITECTURE "l64"
#elif defined(__linux__) && defined(__i386__)
#	define ARCHITECTURE "l32"
#elif defined(__x86_64__)
#	define ARCHITECTURE "?64"
#elif defined(__i386__)
#	define ARCHITECTURE "?32"
#else
#	define ARCHITECTURE "???"
#endif

#define ARCH_EXTRA "-wx3"

#include <iostream>
#include <fstream>
#define _print_version_info(what) \
	std::cout<<what<<" "<<VERSION<<"-"<<ARCHITECTURE<<ARCH_EXTRA<<"\n" \
		"  http://pseint.sourceforge.net\n" \
		"  by Pablo Novara (zaskar_84@yahoo.com.ar)"<<std::endl

#define _write_version_info(what,where) \
	std::ofstream fout((std::string(where))); \
	if (fout.is_open()) \
		fout<<what<<" "<<VERSION<<"-"<<ARCHITECTURE<<ARCH_EXTRA<<std::endl; \
	fout.close()

#define _handle_version_query(what,retval) \
	if (argc==2 && std::string(argv[1])=="--version") { \
		_print_version_info(what); return retval; \
	} else if (argc==3 && std::string(argv[1])=="--version") { \
		_write_version_info(what,argv[2]); return retval; \
	}

#endif
