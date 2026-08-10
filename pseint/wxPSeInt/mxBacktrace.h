#ifndef MXBACKTRACE_H
#define MXBACKTRACE_H
#include <wx/html/htmlwin.h>

class mxBacktrace : public wxPanel{
public:
	mxBacktrace(wxWindow *parent);
	void Reset();
	void Push(wxString func, int line);
	void AboutToPop();
	void Pop();
private:
	wxHtmlWindow *m_html;
	void SetText();
	std::vector<std::pair<wxString,int>> m_procs;
	bool m_about_to_pop = false;
};

extern mxBacktrace *backtrace;

#endif

