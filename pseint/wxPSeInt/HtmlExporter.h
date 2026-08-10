#ifndef HTMLEXPORTER_H
#define HTMLEXPORTER_H
#include <wx/string.h>

class mxSource;

/**
* @brief se encarga de exportar un fuente a HTML, formateandolo correctamente
**/
class HtmlExporter {
private:
	struct style_def {
		bool bold, italic, underline, used;
		wxString bgcolor, fgcolor;
		style_def() {
			bold=italic=underline=false;
			bgcolor=_T("#000000");
			fgcolor=_T("#000000");
			used=false;
		}
	} styles[128];
	void DefineStyles();
	wxString ToHtml(wxString text);
	void SetStyle(int idx, int fontSize, const wxChar *foreground, const wxChar *background, int fontStyle);
public:
	bool Export(mxSource *src, wxString title, wxString fname);
	
};

#endif

