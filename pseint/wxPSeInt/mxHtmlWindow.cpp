#include "mxHtmlWindow.h"
#include <wx/msgdlg.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/menu.h>

BEGIN_EVENT_TABLE(mxHtmlWindow,wxHtmlWindow)
	EVT_RIGHT_DOWN(mxHtmlWindow::OnRightClick)
	EVT_MENU(wxID_COPY,mxHtmlWindow::OnCopy)
END_EVENT_TABLE()
	


mxHtmlWindow::mxHtmlWindow(wxWindow *parent, wxWindowID id, wxPoint  pos, wxSize size): wxHtmlWindow (parent,id,pos,size ) {
	
}

void mxHtmlWindow::OnRightClick (wxMouseEvent & evt) {
	wxMenu menu; 
	menu.Append(wxID_COPY,"Copiar");
	PopupMenu(&menu);
}

void mxHtmlWindow::OnCopy (wxCommandEvent & evt) {
	if (SelectionToText()=="") SelectAll();
	wxTheClipboard->Open();
	wxTheClipboard->SetData(new wxTextDataObject(SelectionToText()));
	wxTheClipboard->Close();
}

