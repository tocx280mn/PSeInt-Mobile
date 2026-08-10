#include "mxVarWindow.h"
#include <wx/listbox.h>
#include "mxMainWindow.h"
#include <wx/imaglist.h>
#include "ConfigManager.h"
#include "ids.h"
#include "mxUtils.h"
#include "DebugManager.h"
#include "mxDebugWindow.h"
#include <wx/choicdlg.h>
#include <wx/menu.h>
#include "string_conversions.h"
#include "mxDesktopTestGrid.h"

mxVarWindow *vars_window = NULL;

BEGIN_EVENT_TABLE(mxVarWindow,wxPanel) 
	EVT_TREE_SEL_CHANGED(wxID_ANY,mxVarWindow::OnTreeClick)
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY,mxVarWindow::OnTreeDClick)
	EVT_TREE_ITEM_MIDDLE_CLICK(wxID_ANY,mxVarWindow::OnTreeClick2)
	EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY,mxVarWindow::OnTreeClick2)
	EVT_TREE_ITEM_GETTOOLTIP(wxID_ANY,mxVarWindow::OnTreeTooltip)
	EVT_MENU(mxID_VARS_DEFINE,mxVarWindow::OnDefinir)
	EVT_MENU(mxID_VARS_RENAME,mxVarWindow::OnRenombrar)
	EVT_MENU(mxID_VARS_ADD_ONE_TO_DESKTOP_TEST,mxVarWindow::OnAgregarUnaAPruebaDeEscritorio)
	EVT_MENU(mxID_VARS_ADD_ALL_TO_DESKTOP_TEST,mxVarWindow::OnAgregarTodasAPruebaDeEscritorio)
END_EVENT_TABLE()
	
static wxString tooltip;

mxVarWindow::mxVarWindow(wxWindow *parent):wxPanel(parent,wxID_ANY,wxDefaultPosition,wxSize(150,100)) {
	wxSizer *sizer=new wxBoxSizer(wxVERTICAL);
	tree = new wxTreeCtrl(this,wxID_ANY,wxDefaultPosition,wxSize(150,50),wxTR_FULL_ROW_HIGHLIGHT|wxTR_NO_LINES|wxTR_HIDE_ROOT|wxTR_SINGLE|wxTR_NO_BUTTONS/*|wxTR_ROW_LINES*/);
	
	wxArrayString img_files(LV_MAX+2,""); // +2 por proc y sub
	// tipos conocidos
	img_files[             LV_LOGICA                        ] = "log.png";
	img_files[LV_DEFINIDA| LV_LOGICA                        ] = "log_def.png";
	img_files[                       LV_NUMERICA            ] = "num.png";
	img_files[LV_DEFINIDA|           LV_NUMERICA            ] = "num_def.png";
	img_files[                                   LV_CARACTER] = "car.png";
	img_files[LV_DEFINIDA|                       LV_CARACTER] = "car_def.png";
	
	// tipos desconocidos, pero factibles
	img_files[0                                             ] = "desc.png";
	img_files[             LV_LOGICA|LV_NUMERICA|LV_CARACTER] = "desc.png";
	img_files[             LV_LOGICA|LV_NUMERICA            ] = "log_num.png";
	img_files[             LV_LOGICA|            LV_CARACTER] = "log_car.png";
	img_files[                       LV_NUMERICA|LV_CARACTER] = "car_num.png";
	
	// tipos imposibles (error del intérprete)
	img_files[LV_DEFINIDA                                   ] = "error.png";
	img_files[LV_DEFINIDA| LV_LOGICA|LV_NUMERICA            ] = "error.png";
	img_files[LV_DEFINIDA| LV_LOGICA            |LV_CARACTER] = "error.png";
	img_files[LV_DEFINIDA|           LV_NUMERICA|LV_CARACTER] = "error.png";
	img_files[LV_DEFINIDA| LV_LOGICA|LV_NUMERICA|LV_CARACTER] = "error.png";
	
	// proceso y subproceso
	img_files[LV_MAX]   = "proc.png";
	img_files[LV_MAX+1] = "sub.png";
	
	int isize = config->big_icons ? 24 : 16;
	wxImageList* imglist = new wxImageList(isize,isize,true,img_files.GetCount());
	for(unsigned int i=0;i<img_files.GetCount();i++)
		imglist->Add(wxBitmap(DIR_PLUS_FILE_3(config->images_path,"vars",config->big_icons?"24":"16",img_files[i]),wxBITMAP_TYPE_PNG));
	tree->AssignImageList(imglist);
	tree_root=tree_current=tree->AddRoot("");
	tree->SetToolTip(tooltip=utils->FixTooltip(_Z("En esta sección se listan las variables que utiliza un algoritmo. El ícono a la izquierda del nombre indica los potenciales tipos de datos que determina el intérprete en caso de que el tipo de variable pueda deducirse antes de ejecutar el algoritmo. Puede seleccionar una para resaltarla en el pseudocódigo.")));
	sizer->Add(tree,wxSizerFlags().Proportion(1).Expand());
	SetSizer(sizer);
}

mxVarWindow::~mxVarWindow() {
	
}

void mxVarWindow::BeginInput ( ) {
	all_vars.Clear();
	wxTreeItemId s=GetSelection(); 
	if (s.IsOk()) {
		last_sel=tree->GetItemText(s).BeforeFirst('['); 
		wxTreeItemId p=tree->GetItemParent(s);
		if (p.IsOk()) last_parent=tree->GetItemText(p).BeforeFirst('('); 
		else last_parent="";
	} else last_sel="";
	tree->DeleteChildren(tree_root);
}

void mxVarWindow::AddProc(wxString vname, bool main_process) {
	if (!main_process) RegisterAutocompKey(vname.BeforeFirst(':'));
	range *r=NULL;
	if (vname.Contains(":")) {
		r=new range;
		vname.AfterFirst(':').BeforeFirst(':').ToLong(&r->from);
		vname.AfterLast(':').ToLong(&r->to);
		vname=vname.BeforeFirst(':');
	}
	tree_current=tree->AppendItem(tree_root,vname,LV_MAX+(main_process?0:1),-1,r);
	if ((not last_sel.IsEmpty()) and vname==last_sel and last_parent.IsEmpty()) tree->SelectItem(tree_current);
}

void mxVarWindow::AddVar(wxString vname, wxChar type) {
	RegisterAutocompKey(vname.BeforeFirst('['));
	int icon = type-LV_BASE_CHAR;
	vname.Replace("-1","??",true);
	wxTreeItemId id=tree->AppendItem(tree_current,vname/*+stype*/,icon);
	if ((not last_sel.IsEmpty()) and vname.BeforeFirst('[')==last_sel && last_parent==tree->GetItemText(tree_current)) tree->SelectItem(id);
}

void mxVarWindow::EndInput ( ) {
	tree->ExpandAll();
}

void mxVarWindow::OnTreeClick (wxTreeEvent & evt) {
	OnTreeClick_aux(evt,false);
}

void mxVarWindow::OnTreeDClick (wxTreeEvent & evt) {
	OnTreeClick_aux(evt,true);
}

void mxVarWindow::OnTreeClick_aux (wxTreeEvent & evt, bool select) {
	evt.StopPropagation(); // para que no le llegue a main_window y crea que es el arbol de errores
//	evt.Skip(); // para que se seleccione en el arbol
	wxTreeItemId it=GetSelection();
	if (!it.IsOk()) return;
	mxSource *src=main_window->GetCurrentSource();
	if (!src) return;
	int from=-1, to=999999;
	wxTreeItemId parent=tree->GetItemParent(it);
	if (parent.IsOk() && parent!=tree->GetRootItem()) {
		range *r=(range*)tree->GetItemData(parent);
		if (r) { from=r->from; to=r->to; }
	} else {
		range *r=(range*)tree->GetItemData(it);
		if (r and select) {
			src->GotoLine(r->to); src->EnsureCaretVisible();
			src->GotoLine(r->from); src->EnsureCaretVisible();
		}
	}
	src->HighLight(tree->GetItemText(it).BeforeFirst('['),from,to);
	main_window->SetLambda([](){main_window->GetCurrentSource()->SetFocus();});
}

void mxVarWindow::OnTreeClick2 (wxTreeEvent & evt) {
	if (debug->debugging) { // cuando se depura, sirve para inspeccionar
		if (!debug->paused) return;
		wxTreeItemId it=evt.GetItem();
		if (it.IsOk() && tree->GetItemParent(it)!=tree_root) {
			tree->SelectItem(it);
			debug_panel->ShowInEvaluateDialog(tree->GetItemText(it).BeforeFirst('['));
		}
	} else { // cuando no se depura muestra un menu contextual
		wxMenu menu;
		if (tree->GetItemParent(evt.GetItem())==tree_root) {
			menu.Append(mxID_VARS_ADD_ALL_TO_DESKTOP_TEST,_Z("Agregar todas las variable a la Prueba de Escritorio"));
		} else {
			menu.Append(mxID_VARS_DEFINE,_Z("Definir variable..."));
			menu.Append(mxID_VARS_RENAME,_Z("Renombrar variable..."));
			menu.Append(mxID_VARS_ADD_ONE_TO_DESKTOP_TEST,_Z("Agregar la variable a la Prueba de Escritorio"));
		}
		PopupMenu(&menu);
	}
}

void mxVarWindow::OnTreeTooltip(wxTreeEvent &evt) {
	tree->SetToolTip(NULL); // si no pongo esto al mover el mouse por varios items y mostrar tooltip "sobre" (no se ve asi) tooltip despues no se cierra
	evt.SetToolTip(tooltip); // for windows only
}

wxTreeItemId mxVarWindow::GetSelection ( ) {
	return tree->GetSelection();
}


void mxVarWindow::OnDefinir (wxCommandEvent & evt) {
	wxTreeItemId it=GetSelection();
	if (!it.IsOk()) return; // ver que se seleccione un item del arbol
	mxSource *src = main_window->GetCurrentSource();
	if (!src) return; // ver que tengamos un fuente abierto
	wxTreeItemId parent = tree->GetItemParent(it);
	if (parent == tree_root) return; // ver que el item no sea el nombre de un proceso o subproceso
	range *r = (range*)tree->GetItemData(parent);
	if (!r) return; // ver que tengamos la informacion del scope del item
	src->DefineVar(r->from,tree->GetItemText(it),r->from,GetVarType(it));
}

void mxVarWindow::OnRenombrar (wxCommandEvent & evt) {
	wxTreeItemId it=GetSelection();
	if (!it.IsOk()) return; // ver que se seleccione un item del arbol
	mxSource *src=main_window->GetCurrentSource();
	if (!src) return; // ver que tengamos un fuente abierto
	wxTreeItemId parent=tree->GetItemParent(it);
	if (parent==tree_root) return; // ver que el item no sea el nombre de un proceso o subproceso
	range *r=(range*)tree->GetItemData(parent);
	if (!r) return; // ver que tengamos la informacion del scope del item
	src->RenameVar(r->from,tree->GetItemText(it),r->from,r->to);
}

bool mxVarWindow::FindVar(int line, wxString var_name, wxTreeItemId &it_parent, wxTreeItemId &it_child) {
	// normalizar identificador
	if (var_name.Contains("[")) var_name=var_name.BeforeFirst('[');
	var_name.MakeUpper();
	// buscar el scope
	wxTreeItemIdValue cookie;
	it_parent = tree->GetFirstChild(tree->GetRootItem(),cookie);
	while (it_parent.IsOk()) {
		range *r=(range*)tree->GetItemData(it_parent);
		if (line>=r->from && line<=r->to) break;
		it_parent = tree->GetNextSibling(it_parent);
	}
	if (!it_parent.IsOk()) return false;
	// buscar la variable
	it_child = tree->GetFirstChild(it_parent,cookie);
	while (it_child.IsOk()) {
		wxString it_text = tree->GetItemText(it_child);
		if (it_text.Contains("[")) it_text = it_text.BeforeFirst('[').Upper();
		else it_text.MakeUpper();
		if (it_text==var_name) return true;
		it_child = tree->GetNextSibling(it_child);
	}
	return false;	
}

int mxVarWindow::GetVarType (int &line, wxString var_name) {
	wxTreeItemId it_parent, it_child;
	if (!FindVar(line,var_name,it_parent,it_child)) return -1;
	line = ((range*)tree->GetItemData(it_parent))->from;
	return GetVarType(it_child);
}

bool mxVarWindow::GetVarScope (int line, wxString var_name, int &line_from, int &line_to) {
	wxTreeItemId it_parent, it_child;
	if (!FindVar(line,var_name,it_parent,it_child)) return false;
	line_from = ((range*)tree->GetItemData(it_parent))->from;
	line_to = ((range*)tree->GetItemData(it_parent))->to;
	return true;
}

int mxVarWindow::GetVarType (const wxTreeItemId & it) {
	return tree->GetItemImage(it);
}

void mxVarWindow::OnAgregarUnaAPruebaDeEscritorio (wxCommandEvent & evt) {
	wxTreeItemId it=GetSelection();
	if (!it.IsOk()) return; // ver que se seleccione un item del arbol
	wxTreeItemId parent=tree->GetItemParent(it);
	if (parent==tree_root) return; // ver que el item no sea el nombre de un proceso o subproceso
	desktop_test->AddDesktopVar(tree->GetItemText(it),false);
}

void mxVarWindow::OnAgregarTodasAPruebaDeEscritorio (wxCommandEvent & evt) {
	wxTreeItemId it=GetSelection();
	if (!it.IsOk()) return; // ver que se seleccione un item del arbol
	wxTreeItemId parent=tree->GetItemParent(it);
	if (parent!=tree_root) return; // ver que el item si sea el nombre de un proceso o subproceso
	wxTreeItemIdValue cookie;
	wxTreeItemId child = tree->GetFirstChild(it,cookie);
	while (child.IsOk()) {
		desktop_test->AddDesktopVar(tree->GetItemText(child),false);
		child = tree->GetNextChild(it,cookie);
	}
}

void mxVarWindow::RegisterAutocompKey (wxString vname) {
	all_vars.Add(vname);
}

