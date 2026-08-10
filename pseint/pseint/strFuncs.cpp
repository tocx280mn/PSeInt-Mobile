#include "strFuncs.hpp"
#include "debug.h"


//std::string FirstWord(const std::string & s) {
//	size_t i=0, l=s.size();
//	while (i<l && EsLetra(s[i],i!=0)) i++;
//	return s.substr(0,i);
//}

bool LastCharIs(const std::string & s, char c) {
	if (s.empty()) return false;
	return s[s.size()-1]==c;
}

void EraseLastChar(std::string &s) {
	_expects(not s.empty());
	s.erase(s.size()-1,1);
}

void RightTrim(std::string &str) {
	size_t len=str.size(), last=str.size();
	while (last>0 && (str[last-1]==' ' || str[last-1]=='\t')) --last;
	if (last!=len) str.erase(last);
}

void LeftTrim(std::string &str) {
	size_t ini=0, len=str.size();
	while (ini<len && (str[ini]==' ' || str[ini]=='\t')) ++ini;
	if (ini) str.erase(0,ini);
}

void Trim(std::string &str) {
	LeftTrim(str); RightTrim(str);
}

std::string ToUpper(std::string a) { 
	int l=a.size();
	for (int x=0;x<l;x++)
		a[x]=toupper(a[x]);
	return a; 
}

std::string ToLower(std::string a) { 
	int l=a.size();
	for (int x=0;x<l;x++)
		a[x]=tolower(a[x]);
	return a;
}

bool RightCompare(std::string a, std::string b) { 
	if (a.size()<b.size()) return false;
	return (a.substr(a.size()-b.size())==b); /// @todo: mejorar 
}

bool LeftCompare(std::string a, std::string b) { 
	if (a.size()<b.size()) return false;
	return (a.substr(0,b.size())==b); /// @todo: mejorar 
}

std::string MkErrorMsg(std::string msg, const std::string &arg, bool add_parentesis) {
	size_t p = msg.find('$'); _expects(p!=std::string::npos);
	if (arg.empty()) msg.erase(p,1);
	else {
		if (add_parentesis) {
			_expects(p!=0);
			bool space_pre = msg[p-1]!=' ';
			msg.insert(p,space_pre?" (":"(");
			p += space_pre?2:1;
			bool space_post = p+1<msg.size() and (msg[p+1]!=' ' and msg[p+1]!=',' and msg[p+1]!='.');
			msg.insert(p+1,space_post?") ":")");
		}
		msg.replace(p,1,arg);
	}
	return msg;
}

std::string MkErrorMsg(std::string msg, const std::string & arg1, const std::string & arg2) {
	size_t p1 = msg.find('$');      _expects(p1!=std::string::npos);
	size_t p2 = msg.find('$',p1+1); _expects(p2!=std::string::npos);
	msg.replace(p2,1,arg2);
	msg.replace(p1,1,arg1);
	return msg;
}

std::vector<std::string> splitArgsList(const std::string &args) {
	std::vector<std::string> out;
	int parentesis=0; bool comillas=false; int i0=0;
	for(int i=0,l=args.size();i<l;i++) {
		if (args[i]=='\''||args[i]=='\"') comillas=!comillas;
		else if (!comillas) {
			if (args[i]=='('||args[i]=='[') parentesis++;
			else if (args[i]==')'||args[i]==']') parentesis--;
			else if (parentesis==0 && args[i]==',') {
				out.push_back(args.substr(i0,i-i0));
				i0=i+1;
			}
		}
	}
	out.push_back(args.substr(i0));
	return out;
}

int matchParentesis(const std::string &src, int p) {
	_expects(src.size()>p and (src[p]=='(' or src[p]=='['));
	int parentesis=1; bool comillas=false; int i0=0;
	for(int i=p+1, len=src.size(); i<len; ++i) {
		if (src[i]=='\''||src[i]=='\"') comillas=!comillas;
		else if (!comillas) {
			if (src[i]=='('||src[i]=='[') parentesis++;
			else if (src[i]==')'||src[i]==']') {
				if (--parentesis==0) 
					return i;
			}
		}
	}
	return -1;
}

char Normalize(char c) {
	switch(c) {
	case 'á': case 'Á': return 'A';
	case 'é': case 'É': return 'E';
	case 'í': case 'Í': return 'I';
	case 'ó': case 'Ó': return 'O';
	case 'ú': case 'Ú': return 'U';
	case 'ü': case 'Ü': return 'U';
	case 'ñ': case 'Ñ': return 'N';
	default: return std::toupper(c);
	}
}

char NormalizeKA(char c) {
	switch(c) {
	case 'á': return 'Á';
	case 'é': return 'É';
	case 'í': return 'Í';
	case 'ó': return 'Ó';
	case 'ú': return 'Ú';
	case 'ü': return 'Ü';
	case 'ñ': return 'Ñ';
	default: return std::toupper(c);
	}
}
