#include <sys/wait.h>
#include <sys/stat.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <dlfcn.h>
#include <ctime>
#include <fstream>

static const char LDLP[] = "LD_LIBRARY_PATH";

static bool log_on = false;

static std::string GetEnv(const char *varname) {
	const char *res = getenv(varname);
	return res?res:"";
}

static void FixArgv(char *argv[],const char *binname) {
	int n=strlen(argv[0])-1;
	char *argv0=new char[n+15];
	while (n>0&&argv[0][n]!='/') n--;
	if (argv[0][n]=='/') strncpy(argv0,argv[0],++n);
	strcpy(argv0+n,binname);
	argv[0]=argv0;
}

static void AddToLDLP(std::string ldlp_value, const std::string &additional_path) {
	if (!ldlp_value.empty() && ldlp_value[ldlp_value.size()-1]!=':') ldlp_value+=":";
	ldlp_value += additional_path;
	if (log_on) std::cerr << "Setting LDLP = " << ldlp_value << std::endl;
	setenv(LDLP,ldlp_value.c_str(),1);
}

static void FixMissingLib(const std::string &libdir, const std::string &libfile) {
	if (log_on) std::cerr << "Checking for " << libfile << std::endl;
	void *handle = dlopen (libfile.c_str(), RTLD_LAZY);
	if (!handle) {
		if (log_on) std::cerr << "Lib Not Found: " << libfile << std::endl;
		AddToLDLP(GetEnv(LDLP),libdir);
	} else {
		if (log_on) std::cerr << "Lib Found: " << libfile << std::endl;
		dlclose(handle);
	}
}

static void TryToRun(char *argv[], const char *binname) {
	std::time_t tm0 = std::time(NULL); 
	char *argv0_orig = argv[0];
	FixArgv(argv,binname);
	if(!std::ifstream(argv[0]).is_open()) {
		if (log_on) std::cerr << "File not found: " << binname << std::endl;
		delete argv[0]; 
		argv[0] = argv0_orig;
		return;
	}
	pid_t pid=fork();
	if (pid==0) {
		if (log_on) std::cerr << "Trying to run: " << argv[0] << std::endl;
		execvp(argv[0],argv);
		if (log_on) std::cerr << "execvp failed" << std::endl;
		// ver si es porque no tiene permiso de ejecución, en cuyo caso intentamos cambiarlos
		struct stat sb;
		if (stat(argv[0],&sb)!=-1 && !(sb.st_mode&S_IXUSR)) { 
			std::string cmd("chmod a+x \""); cmd+=argv[0]; cmd+="\"";
			if (log_on) std::cerr << "Running: " << cmd << std::endl;
			int ret = system(cmd.c_str());
			if (log_on) std::cerr << "Return value: " << ret << std::endl;
			if (log_on) std::cerr << "Trying to run: " << argv[0] << std::endl;
			execvp(argv[0],argv);
			if (log_on) std::cerr << "execvp failed again" << std::endl;
			std::cerr<<"Necesita darle permisos de ejecución al archivo wxPSeInt.bin"<<std::endl;
		}
		exit(0); // que este proceso no contiue, que siga el original
	} else {
		int ret=0;
		if (log_on) std::cerr << "Waiting for pid " << pid << std::endl;
		waitpid(pid,&ret,0);
		if (log_on) std::cerr << "Retval " << ret << " from pid " << pid << std::endl;
		std::time_t tm1 = std::time(NULL);
		// cuando pruebo con wx3, y en caso de fallar paso a wx2, para saber si 
		// fallo al lanzar o lanzo bien y mas tarde revento, mido el tiempo
		// desde que lanzo hasta que recibo el "fallo"
		if (ret==0 || tm1-tm0>5) exit(ret);
		// reestableces argv (lo cambio el FixArgv del principio) por si hay que probar con otro
		delete argv[0]; 
		argv[0] = argv0_orig;
	}
}

static std::string GetPwd(const std::string &argv0) {
	size_t p = argv0.find_last_of('/');
	if (p==std::string::npos) return ".";
	return argv0.substr(0,p);
}

static bool RemoveFirstArg(std::string arg1, int &argc, char *argv[]) {
	if (argc<2 || argv[1] != arg1) return false;
	for(int i=2;i<argc;i++)
		argv[i-1]=argv[i];
	argv[--argc] = NULL;
	return true;
}

int main(int argc, char *argv[]) {
	
	if (RemoveFirstArg("--log",argc,argv)) log_on = true;
	if (log_on) std::cerr << "Argv[0]: " << argv[0] << std::endl;
	
	std::string pwd = GetPwd(argv[0]);
	if (log_on) std::cerr << "PWD: " << pwd << std::endl;
	if (log_on) std::cerr << std::endl;
	
	std::string ldlp_orig = GetEnv(LDLP);
	
	if (log_on) std::cerr << "LDLP: " << ldlp_orig << std::endl;
	AddToLDLP(ldlp_orig,pwd+"/lib");
	TryToRun(argv,"bin/wxPSeInt");
	if (log_on) std::cerr << std::endl;
		
//	if (!RemoveFirstArg("--wx2",argc,argv)) {
//#ifdef __x86_64__ 
//	// probar primero con wx3
//		if (log_on) std::cerr << "LDLP: " << ldlp_orig << std::endl;
//		AddToLDLP(ldlp_orig,pwd+"/bin3/wx");
//		TryToRun(argv,"bin3/wxPSeInt");
//		if (log_on) std::cerr << std::endl;
//#endif
//	}
	
	// si falla, probar con wx2
//	if (log_on) std::cerr << "Disabling Ubuntu's Menu-proxy and Overlay Scrollbars" << std::endl;
//	setenv("UBUNTU_MENUPROXY","",1);
//	setenv("LIBOVERLAY_SCROLLBAR","0",1);
//	if (log_on) std::cerr << "Setting LANG=C, LC_TYPE=C" << std::endl;
//	setenv("LANG","C",1); // for fixing the problem with utf8 locale and ansi wx build
//	setenv("LC_CTYPE","C",1); // for fixing the problem with utf8 locale and ansi wx build
//	AddToLDLP(ldlp_orig,pwd+"/bin2/wx");
//	FixMissingLib(pwd+"/bin2/png","libpng12.so");
//	TryToRun(argv,"bin2/wxPSeInt");

	return 0;
}

