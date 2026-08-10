#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include "mxOpersWindow.h"
#include "ids.h"
#include "mxMainWindow.h"
#include "mxUtils.h"
#include <wx/tooltip.h>
#include "ConfigManager.h"
#include <wx/utils.h>
#include <wx/statline.h>
#include <wx/settings.h>

mxOpersWindow *opers_window = NULL;

BEGIN_EVENT_TABLE(mxOpersWindow,wxScrolledWindow)
	EVT_COMMAND_RANGE(mxID_LAST,mxID_LAST+200,wxEVT_COMMAND_BUTTON_CLICKED,mxOpersWindow::OnItem)
	EVT_COMMAND_RANGE(mxID_LAST+250,mxID_LAST+500,wxEVT_COMMAND_BUTTON_CLICKED,mxOpersWindow::OnLabel)
END_EVENT_TABLE()
	
mxOpersWindow::mxOpersWindow(wxWindow *parent):wxScrolledWindow(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize) {
	sizer=new wxBoxSizer(wxVERTICAL);
	
	AddCaterory(_Z("Op. Algebraicos"));
	Add("+","+",_Z("suma"),_Z("Ej: A+B"));
	Add("-","-",_Z("resta"),_Z("Ej: A-B"));
	Add("*","*",_Z("multiplicación"),_Z("Ej: A*B"));
	Add("/","/",_Z("división"),_Z("Ej: A/B"));
	Add("^","^",_Z("potencia"),_Z("Ej: A^N (A elevado a la N-esima potencia)"));
	Add("%","%",_Z("resto/modulo"),_Z("A%B (resto de dividir A por B)"));
	
	AddCaterory(_Z("Op. Lógicos"));
	Add("&","&&",_Z("conjunción"),_Z("Ej: X>5 Y X<20 (Verdadero solo si ambas son Verdadero)"));
	Add("|","|",_Z("disyunción"),_Z("Ej: X=0 O Z=5 (Verdadero si al menos una de las dos es Verdadero)"));
	Add("~","~",_Z("negación"),_Z("Ej: NO X=5 (Invierte el resultado de la expresión lógica)"));
	
	AddCaterory(_Z("Op. Relacionales"));
	Add("=","=",_Z("igual"),_Z("Ej: A=B"));
//	Add("<>","<>",_Z("distinto"),_Z("Ej: A<>B"));
	Add("!=","!=",_Z("distinto"),_Z("Ej: A!=B (equivalente a A<>B)"));
	Add("<","<",_Z("menor"),_Z("Ej: A<B"));
	Add("<=","<=",_Z("menor o igual"),_Z("Ej: A<=B"));
	Add(">",">",_Z("mayor"),_Z("Ej: A>B"));
	Add(">=",">=",_Z("mayor o igual"),_Z("Ej: A>=B"));
	
	AddCaterory(_Z("Func. Matemáticas"));
	Add(_Z("abs({expresión_numérica})"),"abs",_Z("valor absoluto"),_Z("abs(X) calcula el valor absoluto de x"));
	Add(_Z("trunc({expresión_numérica})"),"trunc",_Z("valor truncado"),_Z("trunc(X) devuelve la parte entera de X"));
	Add(_Z("redon({expresión_numérica})"),"redon",_Z("valor redondeado"),_Z("redon(X) redondea X al entero más cercano"));
	Add(_Z("raiz({expr_num_no_negativa})"),"raiz",_Z("raíz cuadrada"),_Z("raiz(X) calcula la raiz cuadrada de X"));
	Add(_Z("sen({ángulo_en_radianes})"),"sen",_Z("seno"),_Z("sen(X) calcula el seno del angulo X (en radianes)"));
	Add(_Z("cos({ángulo_en_radianes})"),"cos",_Z("coseno"),_Z("cos(X) calcula el coseno del angulo X (en radianes)"));
	Add(_Z("tan({ángulo_en_radianes})"),"tan",_Z("tangente"),_Z("tan(X) calcula la tangente del angulo X (en radianes)"));
	Add(_Z("asen({expresión_numérica})"),"asen",_Z("arcoseno"),_Z("asen(X) calcula el arcoseno (en radianes) de X"));
	Add(_Z("acos({expresión_numérica})"),"acos",_Z("arcocoseno"),_Z("acos(X) calcula el arcocoseno (en radianes) de X"));
	Add(_Z("atan({expresión_numérica})"),"atan",_Z("arcotangente"),_Z("atan(X) calcula el arcotangenta (en radianes) de X"));
	Add(_Z("ln({expr_numérica_positiva})"),"ln",_Z("logaritmo natural"),_Z("ln(X) calcula el logaritmo natural (base e) de X"));
	Add(_Z("exp({expresión_numérica})"),"exp",_Z("func. exponencial"),_Z("exp(X) calcula la función exponencial e^X"));
	
	if (cfg_lang[LS_ENABLE_STRING_FUNCTIONS]) {
		AddCaterory(_Z("Func. p/Cadenas"));
		Add(_Z("Longitud({cadena})"),"Longitud","",_Z("Longutid(S) retorna la cantidad de caracteres de la cadena S"));
		Add(_Z("SubCadena({cadena},{desde},{hasta})"),"SubCadena","",_Z("R<-Subcadena(S,I,J) retorna en R la cadena formada por los caracteres de la cadena S desde la posición I a la J inclusive"));
		Add(_Z("Concatenar({cadena},{cadena})"),"Concatenar","",_Z("R<-Concatenar(S1,S2) retorna en R una sola cadena con los contenidos unidos de S1 y S2"));
		Add(_Z("ConvertirANumero({cadena})"),"ConvertirANumero","",_Z("S<-ConvertirANumero(N) recibe un valor/expresión numérico N, y retorna su equivalente de tipo cadena de caracteres en S"));
		Add(_Z("ConvertirATexto({número})"),"ConvertirATexto","",_Z("N<-ConvertirATexto(S) recibe una variable o expresión S de tipo cadena de caracteres que contiene un valor numérico, y obtiene ne N su equivalente de tipo numérico"));
		Add(_Z("Mayusculas({cadena})"),"Mayusculas","",_Z("MAY<-Mayusculas(S) retorna en MAY el contenido de la cadena S con sus letras minusculas reemplazadas por mayusculas"));
		Add(_Z("Minusculas({cadena})"),"Minusculas","",_Z("MIN<-Minusculas(S) retorna en MIN el contenido de la cadena S con sus letras mayusculas reemplazadas por minusculas"));
	}
	
	AddCaterory(_Z("Otras Funciones"));
	Add(_Z("Azar({expr_num_entera_positiva})"),"Azar",_Z("numero aleatorio"),_Z("Ej: Azar(N) retorna un numero aleatorio entre 0 y N-1"));
	Add(_Z("Aleatorio({valor_minimo},{valor_maximo})"),"Aleatorio",_Z("numero aleatorio"),_Z("Ej: Aleatorio(A,B) retorna un numero aleatorio entre A y B"));
	Add(_Z("HoraActual()"),"HoraActual",_Z("hora actual"),_Z("Retorna un único numero entero con la hora del sistema en formato HHMMSS"));
	Add(_Z("FechaActual()"),"FechaActual",_Z("fecha actual"),_Z("Retorna un único numero entero con la fecha del sistema en formato AAAAMMDD"));
	
	AddCaterory(_Z("Constantes"));
	Add("PI","PI","",_Z("PI o constante de Arquímides (equivale a 3.1415926...)"));
	Add("Euler","Euler","",_Z("E o constante de Euler (equivale a 2.7182818...)"));
	Finish();
	
	AdjustToProfile();
	
	SetScrollRate(0,10);
	SetSizerAndFit(sizer);
}

void mxOpersWindow::Add (wxString code, wxString label, wxString name, wxString desc) {
	if (name.Len()) label<<" ("<<name<<")";
	wxButton *but=new wxButton(this,mxID_LAST+lista.size(),label,wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	sizer->Add(but,wxSizerFlags().Border(wxLEFT,20).Expand());
	but->SetToolTip(utils->FixTooltip(desc));
	lista.push_back(oper_item(code,but));
}

void mxOpersWindow::Replace(oper_item &o, wxString f1, wxString t1, wxString f2, wxString t2, bool sent) {
	if (o.code==(sent?f1:t1)) o.code=(sent?t1:f1);
	if (o.but->GetToolTip() && o.but->GetToolTip()->GetTip().Contains(sent?f2:t2)) {
		wxString s=o.but->GetToolTip()->GetTip(); 
		s.Replace(sent?f2:t2,sent?t2:f2);
		o.but->SetToolTip(s);
	}
	if (f1=="&") f1="&&";
	if (o.but->GetLabel().StartsWith(sent?f1:t1)) {
		wxString s=o.but->GetLabel();
		s.Replace(sent?f1:t1,sent?t1:f1,false);
		o.but->SetLabel(s);
	}
}

void mxOpersWindow::AdjustToProfile ( ) {
	if (cfg_lang[LS_WORD_OPERATORS]) {
		for(unsigned int i=0;i<lista.size();i++) {  
			Replace(lista[i],"&","Y"," & "," Y ",cfg_lang[LS_WORD_OPERATORS]);
			Replace(lista[i],"|","O"," | "," O ",cfg_lang[LS_WORD_OPERATORS]);
			Replace(lista[i],"~","NO","~ ","NO ",cfg_lang[LS_WORD_OPERATORS]);
			Replace(lista[i],"%","MOD","%"," MOD ",cfg_lang[LS_WORD_OPERATORS]);
		}
	}
	for(unsigned int i=0;i<lista.size();i++) {  
		Replace(lista[i],"<=",_T("\u2264"),"<=",_T("\u2264"),config->unicode_opers);
		Replace(lista[i],">=",_T("\u2265"),">=",_T("\u2265"),config->unicode_opers);
		Replace(lista[i],"!=",_T("\u2260"),"!=",_T("\u2260"),config->unicode_opers);
//		Replace(lista[i],"<>",_T("\u2260"),">=",_T("\u2260"),config->unicode_opers);
		Replace(lista[i],"^",_T("\u2191"),"^",_T("\u2191"),config->unicode_opers);
	}
}

void mxOpersWindow::AddCaterory (wxString label) {
	if (!lista2.empty()) lista2.back().to=lista.size();
	label_item i(label,new wxButton(this,mxID_LAST+250+lista2.size(),wxString("[-]  ")+label,wxDefaultPosition,wxDefaultSize,wxNO_BORDER|wxBU_EXACTFIT
#ifndef __WIN32__
		|wxBU_LEFT
#endif
		));
	i.from=lista.size();
#ifdef __WIN32__
	// por alguna razon y contrariamente a lo que dice la documentacion en windows wx 2.8 no toma el wxNO_BORDER ni wxBORDER_NONE
	wxBoxSizer *sizer_aux=new wxBoxSizer(wxHORIZONTAL);
	sizer_aux->Add(i.but,wxSizerFlags().Center());	
	sizer_aux->Add(new wxStaticLine(this),wxSizerFlags().Proportion(1).Center());	
	sizer->Add(sizer_aux,wxSizerFlags().Proportion(0).Expand().Border(wxTOP,5));	
#else
	sizer->Add(i.but,wxSizerFlags().Proportion(0).Expand().Border(wxTOP,5));	
#endif
	lista2.push_back(i);
}

void mxOpersWindow::Finish ( ) {
	if (!lista2.empty()) lista2.back().to=lista.size();
	sizer->Add(new wxStaticText(this,wxID_ANY,""));
	sizer->Add(new wxStaticText(this,wxID_ANY,"Haga click en un operador\no en una función para\ninsertarlo/a en expresiones\ndel pseudocódigo."));
}


void mxOpersWindow::OnItem (wxCommandEvent & evt) {
	int i=evt.GetId()-mxID_LAST;
	main_window->InsertCode(lista[i].code);
}


void mxOpersWindow::OnLabel (wxCommandEvent & evt) {
	int i=evt.GetId()-mxID_LAST-250;
	for(int j=lista2[i].from;j<lista2[i].to;j++) {
		if (lista2[i].showing) lista[j].but->Hide();
		else lista[j].but->Show();
		if (!config->animate_gui) continue;
		sizer->Layout();
		FitInside();
		wxYield();
		wxMilliSleep(15);
	}
	lista2[i].showing=!lista2[i].showing;
	if (lista2[i].showing) lista2[i].but->SetLabel(wxString("[-]  ")+lista2[i].label);
	else lista2[i].but->SetLabel(wxString("[+] ")+lista2[i].label+" (...)");
	sizer->Layout();
	FitInside();
}
