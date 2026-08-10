#ifndef STRING_CONVERSIONS_H
#define STRING_CONVERSIONS_H
#include <wx/string.h>

// _C:  wxchar a char
// _S2W std::string a wxSring
// _W2S wxString a cons char *
// _Z/_ZZ char* a wxString

#define _FixW(x) x
#define _S2W(x) wxString::From8BitData(x.c_str())
#define _Z(x) wxString::From8BitData(x)
#define _ZZ(x) wxString::From8BitData(x)
//#define _W2S(wxstring) ((const char*)(wxstring.mb_str()))
#define _W2S(wxstring) ((const char*)w2s_cs.cWX2MB(wxstring))
#define _C(wxchar) ((wxchar).GetValue())
#define _wxEvtTimer_to_wxTimerPtr(evt) &(evt.GetTimer())

extern wxCSConv w2s_cs;

#endif
