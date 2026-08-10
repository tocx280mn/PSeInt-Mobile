#include "mxFindDialog.h"
#include <wx/sizer.h>
#include "mxUtils.h"
#include <wx/combobox.h>
#include <wx/arrstr.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include "mxSource.h"
#include "mxMainWindow.h"
#include "ids.h"
#include <wx/textfile.h>
#include "ConfigManager.h"
#include "mxArt.h"
#include <wx/msgdlg.h>
#include "string_conversions.h"

BEGIN_EVENT_TABLE(mxFindDialog, wxDialog)
	EVT_BUTTON(mxID_FIND_FIND_NEXT,mxFindDialog::OnFindNextButton)
	EVT_BUTTON(mxID_FIND_FIND_PREV,mxFindDialog::OnFindPrevButton)
	EVT_BUTTON(mxID_FIND_REPLACE,mxFindDialog::OnReplaceButton)
	EVT_BUTTON(mxID_FIND_REPLACE_ALL,mxFindDialog::OnReplaceAllButton)
END_EVENT_TABLE()

mxFindDialog::mxFindDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, _Z("Buscar"), wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB | wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER) 
{
	wxBoxSizer *mySizer= new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *optSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *butSizer = new wxBoxSizer(wxVERTICAL);
	
	optSizer->Add(new wxStaticText(this, wxID_ANY, _Z("Texto a buscar:"), wxDefaultPosition, wxDefaultSize, 0), wxSizerFlags().Border(wxALL,5));
	combo_find = new wxComboBox(this, wxID_ANY);
	optSizer->Add(combo_find,wxSizerFlags().Proportion(0).Expand().Border(wxLEFT|wxBOTTOM,5));
	
	optSizer->Add(replace_static = new wxStaticText(this, wxID_ANY, _Z("Reemplazar por:"), wxDefaultPosition, wxDefaultSize, 0), wxSizerFlags().Border(wxALL,5));
	combo_replace = new wxComboBox(this, wxID_ANY);
	optSizer->Add(combo_replace,wxSizerFlags().Proportion(0).Expand().Border(wxLEFT|wxBOTTOM,5));
	
	check_word = utils->AddCheckBox(optSizer,this,_Z("Solo palabras completas"),false);
	check_start = utils->AddCheckBox(optSizer,this,_Z("Solo al comienzo de la palabra"),false);
	check_case = utils->AddCheckBox(optSizer,this,_Z("Distinguir mayúsculas y minúsculas"),false);
	check_close = utils->AddCheckBox(optSizer,this,_Z("Cerrar este dialogo después de encontrar"),true);
	
	replace_button = new wxButton (this, mxID_FIND_REPLACE, _Z("Reemplazar"));
	replace_button->SetBitmap(*bitmaps->buttons.replace);
	replace_all_button = new wxButton (this, mxID_FIND_REPLACE_ALL, _Z("Reemplazar Todo"));
	replace_all_button->SetBitmap(*bitmaps->buttons.replace);
	next_button = new wxButton (this, mxID_FIND_FIND_NEXT, _Z("Buscar Siguiente"));
	next_button->SetBitmap(*bitmaps->buttons.find);
	wxButton *prev_button = new wxButton (this, mxID_FIND_FIND_PREV, _Z("Buscar Anterior"));
	prev_button->SetBitmap(*bitmaps->buttons.find);
	wxButton *cancel_button = new wxButton (this, wxID_CANCEL, _Z("Cancelar"));
	cancel_button->SetBitmap(*bitmaps->buttons.cancel);
	
	butSizer->Add(replace_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	butSizer->Add(replace_all_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	butSizer->Add(next_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	butSizer->Add(prev_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	butSizer->Add(cancel_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	butSizer->AddStretchSpacer();
	
	mySizer->Add(optSizer,wxSizerFlags().Proportion(1).Expand().Border(wxALL,5));
	mySizer->Add(butSizer,wxSizerFlags().Proportion(0).Expand().Border(wxALL,5));
	
	SetSizerAndFit(mySizer);

	combo_find->Append(wxString());
	combo_replace->Append(wxString());
	
	SetEscapeId(wxID_CANCEL);
	
}

mxFindDialog::~mxFindDialog() {
	
}

void mxFindDialog::ShowFind(mxSource *source) {
	if (source) {
		int i=source->GetSelectionStart();
		int f=source->GetSelectionEnd();
//		if (i==f) {
//			int s=source->WordStartPosition(i,true);
//			int e=source->WordEndPosition(i,true);
//			if (s!=e)
//				combo_find->SetValue(source->GetTextRange(s,e));
//		} else if (source->LineFromPosition(i)==source->LineFromPosition(f)) {
		if (i!=f && source->LineFromPosition(i)==source->LineFromPosition(f)) {
			combo_find->SetSelection(combo_find->GetCount()-1);
			if (i<f)
				combo_find->SetValue(source->GetTextRange(i,f));
			else
				combo_find->SetValue(source->GetTextRange(f,i));
		} else {
			if (combo_find->GetCount()>1)
				combo_find->SetSelection(combo_find->GetCount()-2);
		}
	}
	replace_static->Hide();
	combo_replace->Hide();
	replace_all_button->Hide();
	replace_button->Hide();
	next_button->SetDefault();
	SetTitle(_Z("Buscar"));
	GetSizer()->SetSizeHints(this);
	Fit();
	combo_find->SetFocus();
	Show();
	Raise();
}

void mxFindDialog::ShowReplace(mxSource *source) {
	if (combo_replace->GetCount()>1)
		combo_replace->SetSelection(combo_find->GetCount()-2);
	if (source) {
		int i=source->GetSelectionStart();
		int f=source->GetSelectionEnd();
//		if (i==f) {
//			int s=source->WordStartPosition(i,true);
//			int e=source->WordEndPosition(i,true);
//			if (s!=e)
//				combo_find->SetValue(source->GetTextRange(s,e));
//		} else if (source->LineFromPosition(i)==source->LineFromPosition(f)) {
		if (i!=f && source->LineFromPosition(i)==source->LineFromPosition(f)) {
			combo_find->SetSelection(combo_find->GetCount()-1);
			if (i<f)
				combo_find->SetValue(source->GetTextRange(i,f));
			else
				combo_find->SetValue(source->GetTextRange(f,i));
		} else {
			if (combo_find->GetCount()>1)
				combo_find->SetSelection(combo_find->GetCount()-2);
		}
	}
	replace_static->Show();
	combo_replace->Show();
	replace_all_button->Show();
	replace_button->Show();
	replace_button->SetDefault();
	SetTitle(_Z("Reemplazar"));
	GetSizer()->SetSizeHints(this);
	Fit();
	combo_find->SetFocus();
	Show();
	Raise();
}

bool mxFindDialog::FindPrev() {
	if (main_window->notebook->GetPageCount()!=0) {
		mxSource *source = (mxSource*)(main_window->notebook->GetPage(main_window->notebook->GetSelection()));
		int f,t,p;
		p=source->GetSelectionStart();
		t=0;
		f=source->GetLength();
		
		// buscar
		source->SetSearchFlags(last_flags);
		source->SetTargetStart(p);
		source->SetTargetEnd(t);
		int ret = source->SearchInTarget(last_search);
		if (ret==wxSTC_INVALID_POSITION && p!=f) {
			source->SetTargetStart(f);
			if (last_flags&wxSTC_FIND_REGEXP)
				source->SetTargetEnd(t);
			else
				source->SetTargetEnd(p-last_search.Len());
			ret=source->SearchInTarget(last_search);
		}
		if (ret>=0) {
			source->EnsureVisibleEnforcePolicy(source->LineFromPosition(ret));
			source->SetSelection(source->GetTargetStart(),source->GetTargetEnd());
			return true;
		} else { 
			wxMessageBox(wxString(_Z("La cadena \""))<<last_search<<_Z("\" no se encontró."), _Z("Buscar"));
			return false;
		}
	}
	return false;
}

bool mxFindDialog::FindNext() {
	if (main_window->notebook->GetPageCount()!=0) {
		mxSource *source = (mxSource*)(main_window->notebook->GetPage(main_window->notebook->GetSelection()));
		int f,t,p;
		p=source->GetSelectionEnd();
		f=0;
		t=source->GetLength();

		// buscar
		source->SetSearchFlags(last_flags);
		source->SetTargetStart(p);
		source->SetTargetEnd(t);
		int ret = source->SearchInTarget(last_search);
		if (ret==wxSTC_INVALID_POSITION && p!=f) {
			source->SetTargetStart(f);
			if (last_flags&wxSTC_FIND_REGEXP)
				source->SetTargetEnd(t);
			else
				source->SetTargetEnd(p+last_search.Len());
			ret = source->SearchInTarget(last_search);
		}
		if (ret>=0) {
			source->EnsureVisibleEnforcePolicy(source->LineFromPosition(ret));
			source->SetSelection(source->GetTargetStart(),source->GetTargetEnd());
			return true;
		} else { 
			return false;
		}
	}
	return false;
}

void mxFindDialog::OnFindNextButton(wxCommandEvent &event) {
	
	if (combo_find->GetValue().Len()==0) {
		combo_find->SetFocus();
		return;
	}

	if (!main_window->notebook->GetPageCount()) {
		return;
	}

	last_search = combo_find->GetValue();
	if (last_search!=combo_find->GetString(combo_find->GetCount()-1)) {
		combo_find->SetString(combo_find->GetCount()-1,last_search);
		combo_find->Append(wxString());
	}

	last_flags = 
		(check_case->GetValue()?wxSTC_FIND_MATCHCASE:0) |
		(check_word->GetValue()?wxSTC_FIND_WHOLEWORD:0) |
		(check_start->GetValue()?wxSTC_FIND_WORDSTART:0);

	if (FindNext()) {
		if (check_close->GetValue())
			Hide();
		else {
			Raise();
			combo_find->SetFocus();
		}
	} else {
		wxMessageBox(wxString(_Z("La cadena \""))<<last_search<<_Z("\" no se encontró."), _Z("Buscar"));
		Raise();
	}
	
}

void mxFindDialog::OnFindPrevButton(wxCommandEvent &event) {

	if (combo_find->GetValue().Len()==0) {
		combo_find->SetFocus();
		return;
	}
	
	if (!main_window->notebook->GetPageCount()) {
		wxMessageBox(_Z("No hay ningún archivo abierto actualmente."),_Z("Error"));
		return;
	}

	last_search = combo_find->GetValue();
	if (last_search!=combo_find->GetString(combo_find->GetCount()-1)) {
		combo_find->SetString(combo_find->GetCount()-1,last_search);
		combo_find->Append(wxString());
	}
	
	last_flags = 
		(check_case->GetValue()?wxSTC_FIND_MATCHCASE:0) |
		(check_word->GetValue()?wxSTC_FIND_WHOLEWORD:0) |
		(check_start->GetValue()?wxSTC_FIND_WORDSTART:0);

	if (FindPrev()) {
		if (check_close->GetValue())
			Hide();
		else {
			Raise();
			combo_find->SetFocus();
		}
	} else
		wxMessageBox(wxString(_Z("La cadena \""))<<last_search<<_Z("\" no se encontró."), _Z("Buscar"));
}

void mxFindDialog::OnReplaceButton(wxCommandEvent &event) {

	if (combo_find->GetValue().Len()==0) {
		combo_find->SetFocus();
		return;
	}

	if (!main_window->notebook->GetPageCount()) {
		wxMessageBox(_Z("No hay ningún archivo abierto actualmente."),_Z("Error"));
		return;
	}
	
	last_search = combo_find->GetValue();
	if (last_search!=combo_find->GetString(combo_find->GetCount()-1)) {
		combo_find->SetString(combo_find->GetCount()-1,last_search);
		combo_find->Append(wxString());
	}
	last_replace = combo_replace->GetValue();
	if (last_replace.Len() && last_replace!=combo_replace->GetString(combo_replace->GetCount()-1)) {
		combo_replace->SetString(combo_replace->GetCount()-1,last_replace);
		combo_replace->Append(wxString());
	}
	last_flags = 
		(check_case->GetValue()?wxSTC_FIND_MATCHCASE:0) |
		(check_word->GetValue()?wxSTC_FIND_WHOLEWORD:0) |
		(check_start->GetValue()?wxSTC_FIND_WORDSTART:0);
	
	mxSource *source = (mxSource*)(main_window->notebook->GetPage(main_window->notebook->GetSelection()));
	int f,t;
	source->SetTargetStart(f=source->GetSelectionStart());
	source->SetTargetEnd(t=source->GetSelectionEnd());
	if (source->SearchInTarget(last_search)!=wxSTC_INVALID_POSITION && ( (source->GetTargetStart()==f && source->GetTargetEnd()==t) || (source->GetTargetStart()==t && source->GetTargetEnd()==f) ) ) {
		if (last_flags&wxSTC_FIND_REGEXP)
			source->ReplaceTargetRE(last_replace);
		else
			source->ReplaceTarget(last_replace);
		source->SetSelection(source->GetTargetEnd(),source->GetTargetEnd());
	}
	FindNext();
	
}

void mxFindDialog::OnReplaceAllButton(wxCommandEvent &event) {

	if (combo_find->GetValue().Len()==0) {
		combo_find->SetFocus();
		return;
	}

	if (!main_window->notebook->GetPageCount()) {
		wxMessageBox(_Z("No hay ningún archivo abierto actualmente."),_Z("Error"));
		return;
	}
	
	last_search = combo_find->GetValue();
	if (last_search!=combo_find->GetString(combo_find->GetCount()-1)) {
		combo_find->SetString(combo_find->GetCount()-1,last_search);
		combo_find->Append(wxString());
	}
	last_replace = combo_replace->GetValue();
	if (last_replace.Len() && last_replace!=combo_replace->GetString(combo_replace->GetCount()-1)) {
		combo_replace->SetString(combo_replace->GetCount()-1,last_replace);
		combo_replace->Append(wxString());
	}

	last_flags = 
		(check_case->GetValue()?wxSTC_FIND_MATCHCASE:0) |
		(check_word->GetValue()?wxSTC_FIND_WHOLEWORD:0) |
		(check_start->GetValue()?wxSTC_FIND_WORDSTART:0);
	
	mxSource *source = (mxSource*)(main_window->notebook->GetPage(main_window->notebook->GetSelection()));
	
	int f,t;
	f=0;
	t=source->GetLength();
	
	int c=0; // contador de reemplazos
	
	// primera busqueda
	source->SetSearchFlags(last_flags);
	source->SetTargetStart(f);
	source->SetTargetEnd(t);
	int ret = source->SearchInTarget(last_search);
	while (ret!=wxSTC_INVALID_POSITION) {
		int l = source->GetTargetEnd()-source->GetTargetStart(); // para saber si cambio el largo de la seleccion despues de reemplazar
		if (c==0)
			source->BeginUndoAction();
		if (last_flags&wxSTC_FIND_REGEXP) // el remplazo propiamente dicho
			source->ReplaceTargetRE(last_replace);
		else
			source->ReplaceTarget(last_replace);
		t+=(source->GetTargetEnd()-source->GetTargetStart())-l; // actualizar el largo del bloque donde se busca
		c++; // contar
		// buscar otra vez
		source->SetTargetStart(source->GetTargetEnd());
		source->SetTargetEnd(t);
		ret = source->SearchInTarget(last_search);
	}
	if (c!=0)
		source->EndUndoAction();
	if (only_selection) {
		source->SetSelection(f,t);
	}

	if (c==0)
		wxMessageBox(_Z("No se realizó ningún reemplazo."), _Z("Reemplazar"));
	else if (c==1)
		wxMessageBox(_Z("Se realizó un remplazo."), _Z("Reemplazar"));
	else
		wxMessageBox(wxString(_Z("Se realizaron "))<<c<<_Z(" reemplazos."), _Z("Reemplazar"));

}

