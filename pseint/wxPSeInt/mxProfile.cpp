#include <fstream>
#include <wx/dc.h>
#include <wx/settings.h>
#include <wx/dcmemory.h>
#include <wx/sizer.h>
#include <wx/dir.h>
#include <wx/stattext.h>
#include <wx/textfile.h>
#include <wx/imaglist.h>
#include <wx/hyperlink.h>
#include "mxProfile.h"
#include "ConfigManager.h"
#include "mxUtils.h"
#include "mxConfig.h"
#include "mxArt.h"
#include "mxMainWindow.h"
#include "RTSyntaxManager.h"

#include "string_conversions.h"
#include "Logger.h"
#include "ids.h"
	
static int comp_nocase(const wxString& first, const wxString& second) {
	return first.CmpNoCase(second);
}

static wxString normalize(wxString s) {
	s.MakeLower();
	s.Replace(_Z("á"),_T("a"),true);
	s.Replace(_Z("é"),_T("e"),true);
	s.Replace(_Z("í"),_T("i"),true);
	s.Replace(_Z("ó"),_T("o"),true);
	s.Replace(_Z("ú"),_T("u"),true);
	s.Replace(_Z("ñ"),_T("n"),true);
	s.Replace(_Z("ü"),_T("u"),true);
	return s;
}

static std::string ReadDesc(const std::string &fname) {
	std::ifstream fin(fname);
	if (not fin.is_open()) return "";
	std::string desc;
	for(std::string line; std::getline(fin,line); ) {
		if (line.empty()) continue;
		if (line[0]=='#') continue;
		if (line.size()<5) break;
		if (not (line[0]=='d' and line[1]=='e' and line[2]=='s' and line[3]=='c' and line[4]=='=')) break;
		desc = desc + line.substr(5) + '\n';
	}
	return desc;
}

mxProfilesListCtrl::mxProfilesListCtrl(wxWindow *parent) 
	: wxVListBox(parent,mxID_PROF_LIST,wxDefaultPosition,wxSize(250,300),0),
	  m_font(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)),
	  m_color_sel( wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT) ),
	  m_color_back( wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX) ),
	  m_color_front( wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT) ),
	  m_brush_sel(m_color_sel,wxBRUSHSTYLE_SOLID), 
	  m_brush_norm(m_color_back,wxBRUSHSTYLE_SOLID)
{
	m_null_icon.LoadFile(DIR_PLUS_FILE(DIR_PLUS_FILE(config->profiles_dir,"icons"),"null.png"),wxBITMAP_TYPE_PNG);
	
	wxArrayString names;
	wxDir dir(config->profiles_dir);
	if ( dir.IsOpened() ) {
		wxString filename;
		wxString spec;
		bool cont = dir.GetFirst(&filename, spec , wxDIR_FILES);
		while ( cont ) {
			names.Add(filename);
			cont = dir.GetNext(&filename);
		}
	}
	names.Sort(comp_nocase);
	
	m_full_list.resize(names.GetCount()+1);
	for (unsigned int i=0; i<names.GetCount(); ++i) { 
		m_full_list[i].name = names[i];
		wxString file = DIR_PLUS_FILE(config->profiles_dir,names[i]);
		m_full_list[i].desc = _S2W( ReadDesc( _W2S(file) ) );
	}
	
	m_full_list.back().name = CUSTOM_PROFILE;
	m_full_list.back().desc = _Z("Puede utilizar el botón \"Personalizar\" para definir su propia configuración.");
	m_full_list.back().icon = new wxBitmap(DIR_PLUS_FILE(DIR_PLUS_FILE(config->profiles_dir,"icons"),"personalizado.png"),wxBITMAP_TYPE_PNG);
	
	SetItemCount(0);
}

wxString mxProfilesListCtrl::GetCurrentDescription() const {
	int i = GetSelection(); if (i==wxNOT_FOUND) return "";
	return m_full_list[m_filtered[i]].desc;
}

wxString mxProfilesListCtrl::GetCurrentName() const {
	int i = GetSelection(); if (i==wxNOT_FOUND) return "";
	return m_full_list[m_filtered[i]].name;
}

void mxProfilesListCtrl::Search(const wxString &text, const wxString &to_sel_name) {
	m_filtered.clear();
	
	if (text.IsEmpty()) {
		m_filtered.resize(m_full_list.size());
		for(size_t i=0;i<m_filtered.size();++i) {
			m_filtered[i] = i;
		}
		
	} else {
		m_filtered.clear();
		for (unsigned int i=0; i+1<m_full_list.size(); ++i) {
			if ( m_full_list[i].name.Lower().Contains(text) or
				 m_full_list[i].desc.Contains(text) )
					m_filtered.push_back(i);
		}
	}
	SetItemCount(m_filtered.size());
	
	SetSelection(-1);
	if (not m_filtered.empty()) {
		if (not to_sel_name.IsEmpty()) {
			for(size_t i=0;i<m_filtered.size();++i) { 
				if (m_full_list[m_filtered[i]].name == to_sel_name)
					{ SetSelection(i); break; }
			}
		}
	}
}

void mxProfilesListCtrl::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const {
	
	bool is_selected = IsSelected(n);
	dc.SetBrush( is_selected ? m_brush_sel : m_brush_norm ); 
	// si no crezco el rect en 1, al menos en linux veo un borde negro
	dc.DrawRectangle( rect.x-1, rect.y-1, rect.width+2, rect.height+2 );
	
	const ProfInfo &pi = m_full_list[m_filtered[n]];
	if (not pi.icon) {
		wxString ficon = DIR_PLUS_FILE(DIR_PLUS_FILE(config->profiles_dir,"icons"),pi.name+".png");
		pi.icon = wxFileName::FileExists(ficon) ? new wxBitmap(ficon,wxBITMAP_TYPE_PNG) : &m_null_icon;
	}
	dc.DrawBitmap(*pi.icon,rect.GetLeft()+m_item_heigth/2-12,rect.GetTop()+m_item_heigth/2-12);
	
	dc.SetTextForeground(m_color_front); dc.SetFont(m_font);
	dc.DrawText(pi.name,rect.GetLeft()+m_item_heigth,rect.GetTop()+(m_item_heigth-m_text_height)/2);
}

wxCoord mxProfilesListCtrl::OnMeasureItem(size_t n) const {
	if (m_item_heigth==0) {
		wxMemoryDC dc;
		dc.SetFont(m_font);
		wxSize sz = dc.GetTextExtent("QWERTYUIOPASDFGHJKLZXCVBNMwertyuiopasdfghjklzxcvbnm0987654321");
		m_text_height = sz.GetHeight();
		m_item_heigth = std::max(30,(110*m_text_height)/100);
	}
	return m_item_heigth;
}

mxProfilesListCtrl::~mxProfilesListCtrl() {
	for(ProfInfo &p : m_full_list) {
		if (p.icon and p.icon!=&m_null_icon)
			delete p.icon;
	}
}


BEGIN_EVENT_TABLE(mxProfile,wxDialog)
	EVT_TEXT(wxID_FIND,mxProfile::OnSearchText)
	EVT_LISTBOX(mxID_PROF_LIST,mxProfile::OnListSelect)
	EVT_LISTBOX_DCLICK(mxID_PROF_LIST,mxProfile::OnListActivate)
	EVT_BUTTON(wxID_ABOUT,mxProfile::OnOptionsButton)
	EVT_BUTTON(wxID_OPEN,mxProfile::OnLoadButton)
	EVT_BUTTON(wxID_OK,mxProfile::OnOkButton)
	EVT_BUTTON(wxID_CANCEL,mxProfile::OnCancelButton)
	EVT_CLOSE(mxProfile::OnClose)
END_EVENT_TABLE()

mxProfile::mxProfile(wxWindow *parent) : 
	wxDialog(parent,wxID_ANY,_Z("Opciones del Lenguaje"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
	text(NULL), lang(cfg_lang)
{
	_LOG("mxProfile::mxProfile Start");
	
	list = new mxProfilesListCtrl(this);
	text = new wxTextCtrl(this,wxID_ANY,_T(""),wxDefaultPosition,wxDefaultSize,wxTE_MULTILINE|wxTE_READONLY);
	
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(new wxStaticText(this,wxID_ANY,_Z(" Puede buscar por nombre de la institución, materia, docente, siglas, etc.")),wxSizerFlags().Expand().Proportion(0).Border(wxTOP,5));
	
	wxBoxSizer *search_sizer = new wxBoxSizer(wxHORIZONTAL);
	search=new wxTextCtrl(this,wxID_FIND,"");
	search_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Buscar: ")),wxSizerFlags().Center());
	search_sizer->Add(search,wxSizerFlags().Proportion(1).Expand());
	sizer->Add(search_sizer,wxSizerFlags().Proportion(0).Expand().Border(wxALL,5));
	
	wxBoxSizer *sizer_prof = new wxBoxSizer(wxHORIZONTAL);
	sizer_prof->Add(list,wxSizerFlags().Expand().Proportion(2).FixedMinSize());
	sizer_prof->Add(text,wxSizerFlags().Expand().Proportion(3).FixedMinSize());
	sizer->Add(sizer_prof,wxSizerFlags().Expand().Proportion(1).FixedMinSize());
	
	sizer->AddSpacer(10);
	wxHyperlinkCtrl *label_help = new wxHyperlinkCtrl(this,wxID_ANY,_Z("Soy docente, ¿cómo registro mi institución?"),"http://pseint.sourceforge.net/index.php?page=perfiles.php");
	sizer->Add(label_help,wxSizerFlags().Center().Proportion(0));
	
	sizer->AddSpacer(10);
	wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *options_button = new wxButton (this, wxID_ABOUT, _Z("Personalizar..."));
	options_button->SetBitmap(*bitmaps->buttons.options);
	wxButton *load_button = new wxButton (this, wxID_OPEN, _Z("Cargar..."));
	load_button->SetBitmap(*bitmaps->buttons.load);
	wxButton *ok_button = new wxButton (this, wxID_OK, _Z("Aceptar"));
	ok_button->SetBitmap(*bitmaps->buttons.ok);
	wxButton *cancel_button = new wxButton (this, wxID_CANCEL, _Z("Cancelar"));
	cancel_button->SetBitmap(*bitmaps->buttons.cancel);
	button_sizer->Add(options_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	button_sizer->Add(load_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	button_sizer->AddStretchSpacer(1);
	button_sizer->Add(cancel_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	button_sizer->Add(ok_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	sizer->Add(button_sizer,wxSizerFlags().Expand().Proportion(0));
	
	
	ok_button->SetDefault();
	SetEscapeId(wxID_CANCEL);
	
	SetSizerAndFit(sizer);
	
	this->Layout(); // para ajustar el tamaño de la columna de la lista
	
	search->SetFocus();
	list->Search("",lang.name);
	UpdateDetails();
	
	_LOG("mxProfile::mxProfile End");
}

void mxProfile::OnClose(wxCloseEvent &evt) {
	wxCommandEvent e;
	OnCancelButton(e);
}

void mxProfile::OnOkButton(wxCommandEvent &evt) {
	config->SetProfile(lang);
	EndModal(1); 
}

void mxProfile::OnCancelButton(wxCommandEvent &evt) {
	EndModal(0);
}

void mxProfile::OnListSelect(wxCommandEvent &evt) {
	evt.Skip(); wxString sel_name = list->GetCurrentName();
	if (sel_name==CUSTOM_PROFILE && lang.source==LS_LIST) lang.source = LS_CUSTOM;
	else lang.Load(DIR_PLUS_FILE(config->profiles_dir,sel_name),true);
	UpdateDetails();
}

void mxProfile::OnListActivate(wxCommandEvent &evt) {
	evt.Skip();
	wxCommandEvent ce;
	OnOkButton(ce);
}

void mxProfile::OnOptionsButton(wxCommandEvent &evt) {
	LangSettings custom_lang = lang;
	if (mxConfig(this,custom_lang).ShowModal() && custom_lang!=lang) {
		lang = custom_lang;
		search->Clear(); list->Search("",CUSTOM_PROFILE);
	}
	list->SetFocus();
}

void mxProfile::UpdateDetails() {
	if (!text) return;
	if (lang.source==LS_FILE) {
		text->SetValue(wxString()
					   <<_Z("Su personalización actual ha sido cargada desde el archivo: ") << _S2W(lang.name)); 
	} else if (lang.source==LS_LIST) {
		if (list->GetSelection()==-1)
			text->SetValue(_Z("Seleccione un perfil de la lista para ver su descripción."));
		else
			text->SetValue(list->GetCurrentDescription());
	} else {
		text->SetValue(_Z("Puede utilizar el botón \"Personalizar\" para definir su propia configuración.")); 
	}
}

void mxProfile::OnSearchText (wxCommandEvent & evt) {
	wxString name = lang.source==LS_LIST ? wxString(_S2W(lang.name)) : "";
	list->Search( normalize(search->GetValue()), name );
	UpdateDetails();
}

void mxProfile::OnLoadButton (wxCommandEvent & evt) {
	wxString file = mxConfig::LoadFromFile(this);
	if (!file.IsEmpty()) {
		lang.Load(file,false);
		search->Clear(); list->Search("",CUSTOM_PROFILE);
		UpdateDetails();
	}
}

