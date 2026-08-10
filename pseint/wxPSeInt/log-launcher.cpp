#include <string>
#include <shlobj.h>

std::string GetDesktopPath() {
	LPITEMIDLIST pidl;
	if (SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOP,&pidl)!=NOERROR) return "";
	char buf[MAX_PATH];
	SHGetPathFromIDList(pidl,buf);
	return buf;
}

std::string CatPaths(const std::string &p1, const std::string &p2) {
	return p1 + ((!p1.empty() && p1[p1.size()-1]!='\\') ? "\\" : "" ) + p2;
}

int main() {
	std::string cmd = "wxPSeInt --logger \"" + CatPaths(GetDesktopPath(),"pseint-log.txt") + "\"";
	return WinExec(cmd.c_str(),SW_SHOW);
}

