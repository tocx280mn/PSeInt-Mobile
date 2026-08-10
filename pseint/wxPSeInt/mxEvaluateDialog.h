#ifndef MXEVALUATEDIALOG_H
#define MXEVALUATEDIALOG_H
#include <wx/dialog.h>

class wxComboBox;
class wxCheckBox;
class wxTextCtrl;

class mxEvaluateDialog : public wxDialog {
private:
	wxString last_expression, act_expression;
	wxComboBox *combo;
	wxTextCtrl *result;
	wxTextCtrl *tipo;
public:
	mxEvaluateDialog(wxWindow *parent);
	void OnEvaluateButton(wxCommandEvent &evt);
	void OnCloseButton(wxCommandEvent &evt);
//	void OnClose(wxCloseEvent &evt);
	void EvaluateExpression(wxString exp);
	void SetEvaluationValue(wxString val);
	void OnCharHook(wxKeyEvent &evt);
	void Evaluate();
	void Show();
	static wxString Char2Tipo(char c);
	DECLARE_EVENT_TABLE();
	~mxEvaluateDialog();
};

#endif

