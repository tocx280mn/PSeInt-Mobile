#include <wx/ffile.h>
#include "HtmlExporter.h"
#include "mxSource.h"
#include "mxMainWindow.h"
#include "ConfigManager.h"

bool HtmlExporter::Export(mxSource *src, wxString title, wxString fname) {
	
	wxFFile fil(fname,_T("w+"));
	if (!fil.IsOpened()) 
		return false;
	
	wxString header;
	header<<_T("<HTML><HEAD>\n\t<TITLE>")<<title<<_T("</TITLE>\n\t<STYLE type=\"text/css\">\n");
	fil.Write(header);
	
	header=_T("\t\t.SSD { color:#FFFFFF; background-color:#000000; }\n");
	fil.Write(header);
	
	
	DefineStyles();
	for (int i=0;i<128;i++) {
		if (styles[i].used) {
			header=_T("\t\t.SS");
			header<<i<<_T(" { ");
			if(styles[i].bold)
				header<<_T("font-weight: bold; ");
			if(styles[i].italic)
				header<<_T("font-style: italic; ");
			if(styles[i].underline)
				header<<_T("text-decoration: underline; ");
			header<<_T("color:")<<styles[i].fgcolor<<_T("; ")<<_T("background-color:")<<styles[i].bgcolor<<_T("; }\n");
			fil.Write(header);
		}
	}
	header=_T("\t</STYLE>\n</HEAD><BODY><PRE>\n");
	fil.Write(header);
	
	
	int len = src->GetLength();
	src->Colourise(0,len);
	int p0=0,p1=1;
	while (true) {
		int s=src->GetStyleAt(p0);
		while (p1<len && src->GetStyleAt(p1)==s)
			p1++;
		
		if (styles[s].used) {
			wxString code(_T("<SPAN class=\"SS"));
			code<<s<<_T("\">")<<ToHtml(src->GetTextRange(p0,p1))<<_T("</SPAN>");
			fil.Write(code);
		} else {
			wxString code(_T("<SPAN class=\"SSD"));
			code<<s<<_T("\">")<<ToHtml(src->GetTextRange(p0,p1))<<_T("</SPAN>");
			fil.Write(code);
		}
		
		if (p1==len) 
			break;
		p0=p1++;
	}
	
	wxString footer(_T("\n</PRE></BODY></HTML>"));
	fil.Write(footer);
	
	fil.Close();
	
	return true;
}

void HtmlExporter::DefineStyles() {
	int font_size = config->wx_font_size;
	SetStyle(wxSTC_C_DEFAULT,font_size,_T("BLACK"),_T("WHITE"),0); // default
	SetStyle(wxSTC_C_COMMENT,font_size,_T("BLACK"),_T("WHITE"),0); // comment
	SetStyle(wxSTC_C_COMMENTLINE,font_size,_T("Z DARK GRAY"),_T("WHITE"),mxSOURCE_ITALIC); // comment line
	SetStyle(wxSTC_C_COMMENTDOC,font_size,_T("BLUE"),_T("WHITE"),mxSOURCE_ITALIC); // comment doc
	SetStyle(wxSTC_C_NUMBER,font_size,_T("SIENNA"),_T("WHITE"),0); // number
	SetStyle(wxSTC_C_WORD,font_size,_T("DARK BLUE"),_T("WHITE"),mxSOURCE_BOLD); // keywords
	SetStyle(wxSTC_C_STRING,font_size,_T("RED"),_T("WHITE"),0); // string
	SetStyle(wxSTC_C_CHARACTER,font_size,_T("RED"),_T("WHITE"),0); // character
	SetStyle(wxSTC_C_UUID,font_size,_T("ORCHID"),_T("WHITE"),0); // uuid
	SetStyle(wxSTC_C_PREPROCESSOR,font_size,_T("FOREST GREEN"),_T("WHITE"),0); // preprocessor
	SetStyle(wxSTC_C_OPERATOR,font_size,_T("BLACK"),_T("WHITE"),mxSOURCE_BOLD); // operator 
	SetStyle(wxSTC_C_IDENTIFIER,font_size,_T("BLACK"),_T("WHITE"),0); // identifier 
	SetStyle(wxSTC_C_STRINGEOL,font_size,_T("RED"),_T("Z LIGHT GRAY"),0); // string eol
	SetStyle(wxSTC_C_VERBATIM,font_size,_T("BLACK"),_T("WHITE"),0); // default verbatim
	SetStyle(wxSTC_C_REGEX,font_size,_T("ORCHID"),_T("WHITE"),0); // regexp  
	SetStyle(wxSTC_C_COMMENTLINEDOC,font_size,_T("FOREST GREEN"),_T("WHITE"),0); // special comment 
	SetStyle(wxSTC_C_WORD2,font_size,_T("DARK BLUE"),_T("WHITE"),0); // extra words
	SetStyle(wxSTC_C_COMMENTDOCKEYWORD,font_size,_T("CORNFLOWER BLUE"),_T("WHITE"),0); // doxy keywords
	SetStyle(wxSTC_C_COMMENTDOCKEYWORDERROR,font_size,_T("RED"),_T("WHITE"),0); // keywords errors
	SetStyle(wxSTC_C_GLOBALCLASS,font_size,_T("BLACK"),_T("LIGHT BLUE"),0); // keywords errors
	SetStyle(wxSTC_STYLE_BRACELIGHT,font_size,_T("RED"),_T("Z LIGHT BLUE"),mxSOURCE_BOLD); 
	SetStyle(wxSTC_STYLE_BRACEBAD,font_size,_T("DARK RED"),_T("WHITE"),mxSOURCE_BOLD); 
}

void HtmlExporter::SetStyle(int idx, int fontSize, const wxChar *foreground, const wxChar *background, int fontStyle){
	if (foreground) styles[idx].fgcolor = wxColour(foreground).GetAsString(wxC2S_HTML_SYNTAX);
	if (background) styles[idx].bgcolor = wxColour(background).GetAsString(wxC2S_HTML_SYNTAX);
	styles[idx].bold = fontStyle&mxSOURCE_BOLD;
	styles[idx].italic = fontStyle&mxSOURCE_ITALIC;
	styles[idx].underline = fontStyle&mxSOURCE_UNDERL;
	styles[idx].used=true;
}

wxString HtmlExporter::ToHtml(wxString text) {
	static wxString tabs;
	static int tabs_n;
	if (tabs_n!=config->tabw) {
		tabs=_T("");
		for (int i=0;i<config->tabw;i++)
			tabs<<_T(" ");
		tabs_n=config->tabw;
	}
	text.Replace(_T("&"),_T("&amp;"));
	text.Replace(_T("\t"),tabs);
	text.Replace(_T("<"),_T("&lt;"));
	text.Replace(_T(">"),_T("&gt;"));
	text.Replace(_T("\r"),_T(""));
//	text.Replace(_T(" "),_T("&nbsp;"));
	return text;
}
