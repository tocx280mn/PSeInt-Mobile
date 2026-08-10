#include <wx/printdlg.h>
#include <wx/msgdlg.h>
#include "mxPrintOut.h"
#include "mxSource.h"
#include "mxMainWindow.h"
#include "string_conversions.h"
#include "ConfigManager.h"

//wxPrintDialogData *printDialogData = NULL;
//wxPageSetupDialogData *pageSetupData = NULL;

/**
* @brief Gestiona la impresión de un fuente
* 
* Gestiona la impresión de un fuente utilizando la infraestructura de impresión
* de wxWidgets. Se encarga de calcular la cantidad de paginas, donde comienza
* cada una, y dialogar con el mxSource para renderizarlas en el dc.
* el codigo de esta clase esta basado en el ejemplo de stc que viene con 
* wxWidgets, aunque tiene varias modificaciones.
**/
mxPrintOut::mxPrintOut (mxSource *src, wxString title) : wxPrintout(title) {
//	if (!printData) printData=new wxPrintData;
//	if (!pageSetupData) {
//		pageSetupData=new wxPageSetupDialogData;
////		wxPageSetupDialog pageSetupDialog(NULL);
////		*printData = pageSetupDialog.GetPageSetupData().GetPrintData();
////		*pageSetupData = pageSetupDialog.GetPageSetupData();	
//	}
	m_source = src;
	
	m_source->SetPrintMagnification(config->print_font_size-config->wx_font_size);
	m_source->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_START|wxSTC_WRAPVISUALFLAG_END);
	m_source->SetWrapVisualFlagsLocation(wxSTC_WRAPVISUALFLAGLOC_START_BY_TEXT|wxSTC_WRAPVISUALFLAGLOC_DEFAULT);
	m_source->SetWrapIndentMode(wxSTC_WRAPINDENT_INDENT);
//	m_source->SetMarginWidth (0,0); /// deshabilito los nros de linea porque no los imprime bien (solo en linux?)
	m_source->ClearErrorData();
	
}

mxPrintOut::~mxPrintOut() {
	m_source->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_NONE);
//	m_source->SetMarginWidth (0, m_source->TextWidth (wxSTC_STYLE_LINENUMBER, " XXX"));
	
}


bool mxPrintOut::OnPrintPage (int page) {
	wxDC *dc = GetDC();
	if (!dc) return false;
	PrintScaling (dc);
	m_source->FormatRange (true, m_pages[page-1], m_source->GetLength(),
		dc, dc, m_printRect, m_pageRect);
	return true;
}

bool mxPrintOut::OnBeginDocument (int startPage, int endPage) {
	if (!wxPrintout::OnBeginDocument (startPage, endPage))
		return false;
	m_source->ClearErrorData();
	if (config->use_dark_theme) { 
		config->use_dark_theme = false; 
		m_source->SetStyling(true);
		config->use_dark_theme = true; 
	}
	return true;
}

void mxPrintOut::OnEndDocument() {
	if (config->use_dark_theme) m_source->SetStyling(true);
}

//void mxPrintOut::OnEndDocument () {
//}

void mxPrintOut::GetPageInfo (int *minPage, int *maxPage, int *selPageFrom, int *selPageTo) {
	
	if (not m_pages.empty()) {
		*minPage = *selPageFrom = 1;
		*maxPage = *selPageTo = m_pages.size();
		return;
	}
	
	// initialize values
	*minPage = *maxPage = *selPageFrom = *selPageTo = 0;
	
	// scale DC if possible
	wxDC *dc = GetDC();
	if (!dc) return;
	PrintScaling (dc);
	
	// get print page informations and convert to printer pixels
	wxSize ppiScr;
	GetPPIScreen (&ppiScr.x, &ppiScr.y);
//	wxSize page = pageSetupData->GetPaperSize();
//	wxSize page = dc->GetSize();
	wxSize page; GetPageSizeMM(&page.x,&page.y);
	page.x = static_cast<int> (page.x * ppiScr.x / 25.4);
	page.y = static_cast<int> (page.y * ppiScr.y / 25.4);
	m_pageRect = wxRect (0,0,page.x,page.y);
	
	// get margins informations and convert to printer pixels
//	wxPoint pt = pageSetupData->GetMarginTopLeft();
//	int left = pt.x;
//	int top = pt.y;
//	pt = pageSetupData->GetMarginBottomRight();
//	int right = pt.x;
//	int bottom = pt.y;
	int print_margin=10;
	int top=print_margin,left=print_margin,right=print_margin,bottom=print_margin;
	
	top = static_cast<int> (top * ppiScr.y / 25.4);
	bottom = static_cast<int> (bottom * ppiScr.y / 25.4);
	left = static_cast<int> (left * ppiScr.x / 25.4);
	right = static_cast<int> (right * ppiScr.x / 25.4);
	
	m_printRect = wxRect (left,
		top,
		page.x - (left + right),
		page.y - (top + bottom));
	
	// count pages
	for(int printed=0; printed < m_source->GetLength();) {
		int last = printed;
		m_pages.push_back(printed);
		printed = m_source->FormatRange (0, printed, m_source->GetLength(), dc, dc, m_printRect, m_pageRect);
		*maxPage += 1;
		if (last==printed) {
			wxMessageBox(_Z("Debe configurar la pagina antes de imprimir"),_Z("Error"));
			break;
		}
	}
	if (*maxPage > 0) *minPage = 1;
	*selPageFrom = *minPage;
	*selPageTo = *maxPage;
}

bool mxPrintOut::HasPage (int page) {
	return page<=m_pages.size();
}

bool mxPrintOut::PrintScaling (wxDC *dc){
	
	// check for dc, return if none
	if (!dc) return false;
	
	// get printer and screen sizing values
	wxSize ppiScr;
	GetPPIScreen (&ppiScr.x, &ppiScr.y);
	if (ppiScr.x == 0) { // most possible guess 96 dpi
		ppiScr.x = 96;
		ppiScr.y = 96;
	}
	wxSize ppiPrt;
	GetPPIPrinter (&ppiPrt.x, &ppiPrt.y);
	if (ppiPrt.x == 0) { // scaling factor to 1
		ppiPrt.x = ppiScr.x;
		ppiPrt.y = ppiScr.y;
	}
	wxSize dcSize = dc->GetSize();
	wxSize pageSize;
	GetPageSizePixels (&pageSize.x, &pageSize.y);
	
	// set user scale
	float scale_x = (float)(ppiPrt.x * dcSize.x) /
		(float)(ppiScr.x * pageSize.x);
	float scale_y = (float)(ppiPrt.y * dcSize.y) /
		(float)(ppiScr.y * pageSize.y);
	dc->SetUserScale (scale_x, scale_y);
	
	return true;
}

