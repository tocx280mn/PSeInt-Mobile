#ifndef MXUTILS_H
#define MXUTILS_H
#include <wx/menuitem.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/treebase.h>

#define DIR_PLUS_FILE(dir,file) utils->JoinDirAndFile(dir,file)
#define DIR_PLUS_FILE_2(dir1,dir2,file) utils->JoinDirAndFile(dir1,utils->JoinDirAndFile(dir2,file))
#define DIR_PLUS_FILE_3(dir1,dir2,dir3,file) utils->JoinDirAndFile(dir1,utils->JoinDirAndFile(dir2,utils->JoinDirAndFile(dir3,file)))

WX_DECLARE_STRING_HASH_MAP( wxString, mxHashStringString );
WX_DECLARE_STRING_HASH_MAP( wxTreeItemId, HashStringTreeItem );

class wxToolBar;
class wxSizer;
class wxWindow;
class wxBoxSizer;
class wxButton;

class mxUtils {
private:
public:
	wxMenuItem *AddItemToMenu(wxMenu *menu, wxWindowID id,wxString caption, wxString help, wxString filename);
	wxString JoinDirAndFile(wxString dir, wxString file);
	void AddTool(wxToolBar *toolbar, wxWindowID id, wxString caption, wxString filename, wxString status_text);
	void AddCheckTool(wxToolBar *toolbar, wxWindowID id, wxString caption, wxString filename, wxString status_text, bool checked);
	wxButton *AddImgButton(wxSizer *sizer, wxWindow *parent, wxWindowID id, wxString file, wxString text);
	wxCheckBox *AddCheckBox (wxBoxSizer *sizer, wxWindow *panel, wxString text, bool value);
	bool IsTrue(wxString value) const;
	wxMenuItem *AddCheckToMenu(wxMenu *menu, wxWindowID id,wxString caption, wxString help, bool value);
	wxString GetOutput(wxString command);
	wxString ToHtml(wxString text);
	wxString FixTooltip(wxString tooltip);
	wxString GetVersion(wxString exe);
	wxString GetExportLangName(int id);
	wxString GetExportLangCode(int id);
	wxString GetClipboardText();
	void SetClipboardText(const wxString &text);
};

extern mxUtils *utils;
#endif

