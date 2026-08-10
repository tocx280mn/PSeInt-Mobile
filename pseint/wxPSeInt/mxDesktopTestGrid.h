#ifndef MXDESKTOPTESTGRID_H
#define MXDESKTOPTESTGRID_H
#include <wx/grid.h>


class mxDesktopTestGrid: public wxGrid {
private:
	DECLARE_EVENT_TABLE();
	int selected_row, rows_num;
	int old_size, sel_col;
	float *cols_sizes;
	bool created, editable;
	wxArrayString variables;
public:
	mxDesktopTestGrid(wxWindow *parent, wxWindowID id);
	void OnLabelDblClick(wxGridEvent &event);
	void OnLabelRightClick(wxGridEvent &event);
	void OnCellDblClick(wxGridEvent &event);
	void OnCellRightClick(wxGridEvent &event);
	void OnSelectCell(wxGridEvent &evt);
	void OnResize(wxSizeEvent &evt);
	void OnColResize(wxGridSizeEvent &evt);
	const wxArrayString &GetDesktopVars();
	void AddDesktopVar(const wxString &val, bool force=true);
	void OnClearVars();
	void SetCols();
	void SetAutoevaluation(long l, wxString value);
	void SetLine(wxString proc, long line, long i=-1);
	void ResetTest();
	void SetEditable(bool can_edit);
	void OnCopyOne(wxCommandEvent &event);
	void OnCopyAll(wxCommandEvent &event);
	void OnGotoLine(wxCommandEvent &event);
};

extern mxDesktopTestGrid *desktop_test;

#endif

