#ifndef MXVARWINDOW_H
#define MXVARWINDOW_H
#include <wx/panel.h>
#include <wx/treectrl.h>
#include "../pseint/new_memoria_inter.h"

class mxVarWindow : public wxPanel {
private:
	struct range:public wxTreeItemData { long from, to; };
	wxTreeCtrl *tree;
	wxTreeItemId tree_root,tree_current;
	wxString last_sel, last_parent;
	friend class mxSource;
	wxArrayString all_vars;
protected:
public:
	void BeginInput();
	void AddProc(wxString vname, bool main_process); ///< cuando el RTSyntaxManager recibe el nombre de un proceso o subproceso, lo registra en la lista con este método
	void RegisterAutocompKey(wxString vname); ///<  cuando el RTSyntaxManager recibe el nombre de una variable
	void AddVar(wxString vname, wxChar type); ///<  cuando el RTSyntaxManager recibe el nombre de una variable
	void EndInput();
	void OnTreeClick2(wxTreeEvent &evt);
	void OnTreeClick(wxTreeEvent &evt);
	void OnTreeDClick(wxTreeEvent &evt);
	void OnTreeClick_aux(wxTreeEvent &evt, bool select);
	void OnTreeTooltip(wxTreeEvent &evt);
	void OnDefinir(wxCommandEvent &evt);
	void OnRenombrar(wxCommandEvent &evt);
	void OnAgregarUnaAPruebaDeEscritorio(wxCommandEvent &evt);
	void OnAgregarTodasAPruebaDeEscritorio(wxCommandEvent &evt);
	wxTreeItemId GetSelection();
	mxVarWindow(wxWindow *parent);
	~mxVarWindow();
	
	bool FindVar(int line, wxString var_name, wxTreeItemId &it_parent, wxTreeItemId &it_child);
	int GetVarType(int &line, wxString var_name);
	int GetVarType(const wxTreeItemId &it);
	bool GetVarScope(int line, wxString var_name, int &line_from, int &line_to);
	
	DECLARE_EVENT_TABLE();
};

extern mxVarWindow *vars_window;

#endif

