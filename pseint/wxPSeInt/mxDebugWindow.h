#ifndef MXDEBUGWINDOW_H
#define MXDEBUGWINDOW_H
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>

class wxButton;
class wxScrollBar;
class wxStaticText;
class mxSource;
class wxCheckBox;
class mxEvaluateDialog;

enum ds_enum {DS_NONE,DS_STEP,DS_STARTING,DS_PAUSED,DS_RESUMED,DS_FINALIZED,DS_STOPPED};

class mxDebugWindow : public wxPanel {
private:
	enum {BMP_COMENZAR,BMP_CONTINUAR,BMP_PAUSAR,BMP_PAUSAR_CONTINUAR,BMP_PRIMER_PASO,BMP_UN_PASO,BMP_FINALIZAR,BMP_CERRAR,BMP_COUNT};
	wxBitmap dp_bmps[BMP_COUNT];
	mxEvaluateDialog *evaluate_window;
	ds_enum ds_state;
	wxScrollBar *debug_speed;
	wxBitmapButton *dp_button_run, *dp_button_step, *dp_button_pause;
	wxButton *dp_button_evaluate;
	wxCheckBox *dp_check_step_in;
	wxCheckBox *dp_check_backtrace;
	wxCheckBox *dp_check_subtitles;
	wxCheckBox *dp_check_desktop_test;
public:
	mxDebugWindow(wxWindow *parent);
	void SetSpeed(int speed);
	void SetState(ds_enum state);
	ds_enum GetState();
	void DebugStartFromGui(bool from_psdraw=false);
	void OnDebugButton(wxCommandEvent &evt);
	void OnDebugPause(wxCommandEvent &evt);
	void OnDebugStep(wxCommandEvent &evt);
	void OnDebugDesktopTest(wxCommandEvent &evt);
	void OnDebugEvaluate(wxCommandEvent &evt);
	void OnDebugHelp(wxCommandEvent &evt);
	void OnDebugCheckStepIn(wxCommandEvent &evt);
	void OnDebugCheckSubtitles(wxCommandEvent &evt);
	void OnDebugCheckBacktrace(wxCommandEvent &evt);
	void OnDesktopTestPanelHide();
	bool IsDesktopTestEnabled();
	void StartDebugging(mxSource *source, bool paused);
	void ProfileChanged();
	void ShowInEvaluateDialog(wxString s);
	void SetSubtitles(bool on);
	DECLARE_EVENT_TABLE();
};

extern mxDebugWindow *debug_panel;

#endif

