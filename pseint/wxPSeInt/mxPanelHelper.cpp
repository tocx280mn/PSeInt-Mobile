#include "mxPanelHelper.h"
#include "mxMainWindow.h"
#include <wx/settings.h>
#include <wx/dcclient.h>
#include "ConfigManager.h"

BEGIN_EVENT_TABLE(mxPanelHelper,wxPanel)
	EVT_ENTER_WINDOW(mxPanelHelper::OnEnter)
	EVT_LEAVE_WINDOW(mxPanelHelper::OnLeave)
	EVT_PAINT(mxPanelHelper::OnPaint)
	EVT_LEFT_DOWN(mxPanelHelper::OnClick)
END_EVENT_TABLE()
	
mxPanelHelper::mxPanelHelper(wxWindow *parent, wxWindowID id, wxString bitmap, wxString label)
	: wxPanel(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize),
	  font(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT))
{
	
//	if (config->big_icons) font.SetPointSize(font.GetPointSize()*1.4);
	this->label=label;
	mid=id; selected=false;
	
	bmp.LoadFile(bitmap,wxBITMAP_TYPE_PNG);
	
	wxClientDC dc(this);
	dc.SetFont(font);
	wxSize text_size = dc.GetTextExtent(label);
	
	wxSize size ( bmp.GetWidth()+6, bmp.GetHeight()+8+text_size.GetWidth()+10 );
	text_x = (size.GetWidth()+text_size.GetHeight())/2;
#ifdef __WIN32__
	text_x+=2;
#endif
	
	SetMinSize(size);
	SetSize(size);
}

void mxPanelHelper::OnEnter(wxMouseEvent &evt) { 
	selected=true; Refresh(); 
}

void mxPanelHelper::OnLeave(wxMouseEvent &evt) { 
	selected=false; Refresh(); 
}

void mxPanelHelper::OnClick(wxMouseEvent &evt) {
	wxCommandEvent cmd(wxEVT_COMMAND_BUTTON_CLICKED,mid);
	main_window->GetEventHandler()->ProcessEvent(cmd);
}

void mxPanelHelper::OnPaint(wxPaintEvent &evt) {
	wxPaintDC dc(this);
	PrepareDC(dc);
	dc.SetBackground(wxSystemSettings::GetColour(selected?wxSYS_COLOUR_BTNHILIGHT:wxSYS_COLOUR_FRAMEBK));
	dc.Clear();
	dc.DrawBitmap(bmp,3,4,true);
	dc.SetFont(font);
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	dc.DrawRotatedText(label,text_x,bmp.GetHeight()+10,270);
}
