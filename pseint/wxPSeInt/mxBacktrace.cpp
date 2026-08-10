#include <wx/sizer.h>
#include <string>
#include "mxBacktrace.h"
#include "ConfigManager.h"
#include "mxUtils.h"

mxBacktrace *backtrace = nullptr;

mxBacktrace::mxBacktrace(wxWindow *parent) 
	: wxPanel(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize) 
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	m_html = new wxHtmlWindow(this,wxID_ANY,wxDefaultPosition,wxSize(50,50));
	m_html->SetToolTip(utils->FixTooltip( _Z("Aquí se muestra el encadenamiento de llamadas entre el proceso/algoritmo principal y los diferentes subprocesos/funciones, para saber cómo se llegó al punto actual de la ejecución, y qué funciones quedaron pausadas a la espera de la finalización de la función actual.")));
	SetText();
	sizer->Add(m_html,wxSizerFlags().Expand().Proportion(1));
	SetSizerAndFit(sizer);
}

void mxBacktrace::Reset ( ) {
	m_procs.clear();
	m_about_to_pop = false;
	SetText();
}

void mxBacktrace::Push (wxString func, int line) {
	if (m_about_to_pop) { Pop(); return; }
	if (not m_procs.empty()) m_procs.back().second = line;
	m_procs.push_back({func,0}); SetText();
}

void mxBacktrace::AboutToPop ( ) {
//	if (GetItemCount()>0) SetItemTextColour(0,m_colour_del);
	m_about_to_pop = true;
	SetText();
}

void mxBacktrace::Pop ( ) {
	m_about_to_pop = false;
	m_procs.pop_back();
	SetText();
//	if (GetItemCount()>0) DeleteItem(0);
//	if (GetItemCount()>0) SetItemTextColour(0,m_colour_new);
}

void mxBacktrace::SetText ( ) {
	
	const char *CL_REG_BACK  = config->use_dark_theme ? "#333333" : "#FFFFFF" ;
	const char *CL_REG_FORE  = config->use_dark_theme ? "#FAFAFA" : "#000000" ;
//	const char *CL_DIMM_FORE = config->use_dark_theme ? "#888888" : "#888888" ;
//	const char *CL_KEYWORD   = config->use_dark_theme ? "#9999FA" : "#000080" ;
	const char *CL_STRING    = config->use_dark_theme ? "#99FA99" : "#006400" ;
//	const char *CL_NUMBER    = config->use_dark_theme ? "#FAFA99" : "#A0522D" ;
	const char *CL_COMMENT_1 = config->use_dark_theme ? "#999999" : "#969696" ;
//	const char *CL_COMMENT_2 = config->use_dark_theme ? "#33FA33" : "#5050FF" ;
//	const char *CL_ALT_FORE  = config->use_dark_theme ? "#999999" : "#969696" ;
//	const char *CL_ALT_BACK  = config->use_dark_theme ? "#606060" : "#D3D3D3" ;
//	const char *CL_HLG_BACK  = config->use_dark_theme ? "#335050" : "#AACCFF" ;
	const char *CL_ANOT_FORE = config->use_dark_theme ? "#FA9999" : "#800000" ;
//	const char *CL_ANOT_BACK = config->use_dark_theme ? "#454545" : "#FAFAD7" ;
	
	
	m_html->SetStandardFonts(config->wx_font_size,config->wx_font_name,config->wx_font_name);
	wxString text = "<HTML><BODY bgcolor=\""; 
	text += CL_REG_BACK; text +="\">";
//	text += "<FONT color=\""; text += CL_REG_FORE; text +="\">Ejecutando: </FONT>";
	for(size_t i=0;i<m_procs.size();++i) {
		text += "<FONT color=\"";
		if (i+1==m_procs.size()) {
			if (m_about_to_pop) text += CL_ANOT_FORE;
			else text += CL_STRING;
		} else {
			text += CL_COMMENT_1;
		}
		text += "\">";
		if (i) { 
			if (m_about_to_pop and i+2!=m_procs.size()) text += _T("&nbsp;<FONT size=\"+1\">\u21d0</FONT>&nbsp;");
			else text += _T("&nbsp;<FONT size=\"+1\">\u21d2</FONT>&nbsp;");
		}
		text += m_procs[i].first;
		if (i+1<m_procs.size()) text += _T(" (\u23f8 linea ") + std::to_string(m_procs[i].second) + ")";
		else text += m_about_to_pop ? _T(" ( \u2716 )") : _T(" (\u23f5)");
		text += "</FONT>";
		
	}
	text += "</BODY></HTML>";
	m_html->SetPage(text);
}

