#include <wx/clipbrd.h>
#include <wx/sizer.h>
#include <wx/stc/stc.h>
#include <wx/stattext.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/app.h>
#include "Application.h"
#include "mxSingleCaseWindow.h"
#include "Package.h"
#include "../wxPSeInt/string_conversions.h"
#include <wx/button.h>

BEGIN_EVENT_TABLE(mxSingleCaseWindow,wxFrame)
	EVT_LIST_ITEM_SELECTED(wxID_ANY,mxSingleCaseWindow::OnList)
	EVT_BUTTON(wxID_OK,mxSingleCaseWindow::OnCopy)
	EVT_BUTTON(wxID_CANCEL,mxSingleCaseWindow::OnCancel)
	EVT_CLOSE(mxSingleCaseWindow::OnClose)
END_EVENT_TABLE()

mxSingleCaseWindow::mxSingleCaseWindow (wxWindow *parent, bool only_first, bool show_solution) : wxFrame(parent,wxID_ANY,_Z("Resultados"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE) {
	
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	wxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
	
	input=output=solution=NULL;
	
	wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	
	if (show_solution) {
		wxSizer *solution_sizer = new wxBoxSizer(wxVERTICAL);
		solution = new wxStyledTextCtrl(this); solution->SetMinSize(wxSize(300,200));
		solution_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Solución correcta:")),wxSizerFlags().Border(wxALL,5));
		solution_sizer->Add(solution,wxSizerFlags().Proportion(1).Expand().Border(wxALL,5));
		sizer->Add(solution_sizer,wxSizerFlags().Proportion(1).Expand()/*.Border(wxALL,5)*/);
	} else {
		wxSizer *input_sizer = new wxBoxSizer(wxVERTICAL);
		input = new wxStyledTextCtrl(this); input->SetMinSize(wxSize(300,200));
		input_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Datos de entrada:")),wxSizerFlags().Border(wxALL,5));
		input_sizer->Add(input,wxSizerFlags().Proportion(1).Expand().Border(wxALL,5));
		sizer->Add(input_sizer,wxSizerFlags().Proportion(1).Expand()/*.Border(wxALL,5)*/);
	}
	
	wxSizer *output_sizer = new wxBoxSizer(wxVERTICAL);
	output = new wxStyledTextCtrl(this); output->SetMinSize(wxSize(300,200));
	output_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Tu solución:")),wxSizerFlags().Border(wxALL,5));
	output_sizer->Add(output,wxSizerFlags().Proportion(1).Expand().Border(wxALL,5));
	sizer->Add(output_sizer,wxSizerFlags().Proportion(1).Expand()/*.Border(wxALL,5)*/);
	
	
	list = new wxListCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxLC_LIST|wxLC_NO_HEADER|wxLC_SINGLE_SEL);
//	list->InsertColumn(0,_Z("Casos")); // solo para listas tipo report
	
	wxSizer *but_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *but_copy = new wxButton(this,wxID_OK,_Z("Copiar Entrada"));
	wxButton *but_close  = new wxButton(this,wxID_CANCEL,_Z("Cerrar"));
	but_sizer->Add(but_copy,wxSizerFlags().Left());
	but_sizer->AddStretchSpacer();
	but_sizer->Add(but_close,wxSizerFlags().Left());
	
	if (!only_first) 
		main_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Seleccione un caso de prueba:")),wxSizerFlags().Border(wxALL,5));
	main_sizer->Add(list,wxSizerFlags().Proportion(0).Expand().Border(wxALL,5));
	if (only_first) list->Hide();
	else main_sizer->Add(new wxStaticLine(this,wxID_ANY),wxSizerFlags().Proportion(0).Expand().Border(wxALL,5));
	main_sizer->Add(sizer,wxSizerFlags().Proportion(1).Expand());
	main_sizer->Add(but_sizer,wxSizerFlags().Proportion(0).Expand().Border(wxALL,5));
	
	SetSizerAndFit(main_sizer);
	CentreOnParent();
	
}

void mxSingleCaseWindow::AddCaso (wxString & nombre) {
	list->InsertItem(list->GetItemCount(),nombre);
}

void mxSingleCaseWindow::OnList (wxListEvent & event) {
	long item = -1;
	item = list->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	current_test = list->GetItemText(item);
	TestCase &t = pack.GetTest(current_test);
	
	if (output)   output  ->SetReadOnly(false);
	if (input)    input   ->SetReadOnly(false);
	if (solution) solution->SetReadOnly(false);
	
	if (solution) solution->SetText(t.solution);
	if (output)   output  ->SetText(t.output);
	if (input)    input   ->SetText(t.input);
	if (output && solution) Diff();
	
	if (output)   output  ->SetReadOnly(true);
	if (input)    input   ->SetReadOnly(true);
	if (solution) solution->SetReadOnly(true);
}

void mxSingleCaseWindow::OnClose (wxCloseEvent & event) {
	Destroy();
	GetParent()->Destroy();
}

void mxSingleCaseWindow::OnCancel (wxCommandEvent & event) {
	Close();
}

void mxSingleCaseWindow::Show ( ) {
	wxFrame::Show();
//	if (list->GetItemCount()==1) { 
		list->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		wxListEvent event; OnList(event);
//	}
}

void mxSingleCaseWindow::OnCopy (wxCommandEvent & event) {
	TestCase &t = pack.GetTest(current_test);
	if (wxTheClipboard->Open()) {
		wxTheClipboard->SetData(new wxTextDataObject(t.input));
		wxTheClipboard->Close();
	}
}













class DiffResultsProcessor {
public:
	virtual void OnAdd(int file, int line_num, int line_count) = 0;
	virtual void OnDel(int file, int line, int count) = 0;
	virtual void OnChange(int file, int line_from, int line_to, int lines_count) = 0;
	
	void Run(const wxArrayString &output) {
		for (unsigned int i=0;i<output.GetCount();i++) {
			int n1=0,n2=0,n3=0,n4=0,p=0;
			while (output[i][p]>='0' && output[i][p]<='9')
				n1 = n1*10 + _C(output[i][p++])-'0' ;
			if (output[i][p]==',') {
				p++;
				while (output[i][p]>='0' && output[i][p]<='9')
					n2 = n2*10 + _C(output[i][p++])-'0' ;
			} else
				n2=n1;
			char c=output[i][p++];
			while (output[i][p]>='0' && output[i][p]<='9')
				n3 = n3*10 + _C(output[i][p++])-'0' ;
			if (output[i][p]==',') {
				p++;
				while (output[i][p]>='0' && output[i][p]<='9')
					n4 = n4*10 + _C(output[i][p++])-'0' ;
			} else
				n4=n3;
			n1--; n2--; n3--; n4--;
			int skip=0;
			if (c=='c') {
				OnChange(1, n1,n2, n4-n3+1);
				OnChange(2, n3,n4, n2-n1+1);
				skip= n2-n1+1 + n4-n3+1 + 1;
			} else if (c=='a') {
				OnAdd(1, n1+1, n4-n3+1);
				OnDel(2, n3,n4);
				skip = n4-n3+1;
			} else if (c=='d') {
				OnAdd(2, n3+1, n2-n1+1);
				OnDel(1, n1,n2);
				skip = n2-n1+1;
			}
			i+=skip;
		}
	}
	
	void Run(wxString fname1, wxString fname2, bool ignore_empty_lines, bool ignore_spaces, bool ignore_case) {
		wxString command("diff --strip-trailing-cr ");
		if (ignore_empty_lines) command<<"-B ";
		if (ignore_spaces) command<<"-E -b ";
		if (ignore_case) command<<"-i ";
		command<<"\""<<fname1<<"\"  \""<<fname2<<"\"";
		wxArrayString output;
		wxExecute(command,output,wxEXEC_SYNC);
		Run(output);
	}
};


#include "../dtl/dtl/dtl.hpp"  // https://github.com/cubicdaiya/dtl

void mxSingleCaseWindow::Diff ( ) {
	wxColour colour_change(255,200,200);
	output->MarkerDefine(0,wxSTC_MARK_BACKGROUND,colour_change,colour_change);
	solution->MarkerDefine(0,wxSTC_MARK_BACKGROUND,colour_change,colour_change);
	
	
	typedef wxString                 elem;
	typedef std::vector< elem >         sequence;
	typedef std::pair< elem, dtl::elemInfo > sesElem;
	
	sequence vout, vsol;
	
	int nout = output->GetLineCount(), nsol = solution->GetLineCount();
	for(int i=0;i<nout;i++) vout.push_back(output->GetLine(i));
	for(int i=0;i<nsol;i++) vsol.push_back(solution->GetLine(i));
	
	dtl::Diff< elem > diff(vsol, vout);
//	diff.onHuge();
	diff.compose();
	
	std::vector<sesElem> ses = diff.getSes().getSequence();
	
	bool first=true;
	for(size_t i=0;i<ses.size();i++) { 
		if (!ses[i].second.type) continue;
		if (first) {
			int fb = ses[i].second.beforeIdx;
			int fa = ses[i].second.afterIdx;
			int line = fa==0 ? fb : ( fb==0 ? fa : std::min(fa,fb) );
			solution->GotoLine(line-1);
			output->GotoLine(line-1);
			first=false;
		}
		
		if (ses[i].second.beforeIdx) {
			solution->MarkerAdd(ses[i].second.beforeIdx-1,0);
		}
		if (ses[i].second.afterIdx) {
			output->MarkerAdd(ses[i].second.afterIdx-1,0);
		}
	}
}


