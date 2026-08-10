#ifndef MXCONFIG_H
#define MXCONFIG_H
#include <wx/dialog.h>

class wxCheckBox;
class wxTextCtrl;
class wxComboBox;

class mxConfig : public wxDialog {
	wxTextCtrl *tx_ancho;
	wxTextCtrl *tx_alto;
	wxTextCtrl *tx_zoom;
	wxCheckBox *ch_comments;
	wxComboBox *cm_colors;
	wxCheckBox *ch_crop;
	wxCheckBox *ch_compact;
	wxComboBox *cm_style;
	wxComboBox *cm_proc;
	bool ignore_events;
	int iancho,ialto;
public:
	mxConfig();
	void SetZoom(float f=0, int noup=-1);
	void SetProceso(int i);
	DECLARE_EVENT_TABLE();
	void OnAncho(wxCommandEvent &evt);
	void OnAlto(wxCommandEvent &evt);
	void OnZoom(wxCommandEvent &evt);
	void OnColors(wxCommandEvent &evt);
	void OnComments(wxCommandEvent &evt);
	void OnCompact(wxCommandEvent &evt);
	void OnCrop(wxCommandEvent &evt);
	void OnProc(wxCommandEvent &evt);
	void OnStyle(wxCommandEvent &evt);
};

#endif

