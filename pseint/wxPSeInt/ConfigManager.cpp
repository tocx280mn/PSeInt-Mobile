#include <wx/filename.h>
#include <wx/settings.h>
#include <wx/textfile.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/fontenum.h>
#include "ConfigManager.h"
#include "mxUtils.h"
#include "version.h"
#include "Logger.h"
#include "string_conversions.h"
#include "error_recovery.h"
#include "osdep.h"

ConfigManager *config = NULL;

ConfigManager::ConfigManager(wxString apath) : lang(LS_INIT) {
	
	pseint_dir = apath;
	version=0; 
	
#if defined(_WIN32) || defined(__WIN32__)
	home_dir = DIR_PLUS_FILE(wxFileName::GetHomeDir(),"pseint");
#else
	home_dir = DIR_PLUS_FILE(wxFileName::GetHomeDir(),".pseint");
#endif
	if (!wxFileName::DirExists(home_dir))
		wxFileName::Mkdir(home_dir);
	if (wxFileName::FileExists("config.here"))
		filename="config.here";
	else
		filename = DIR_PLUS_FILE(home_dir,"config");
	LoadDefaults();
	Read();
	
#ifdef __APPLE__
	if (version<20210407)
#else
	bool inconsolata_ok = wxFont::AddPrivateFont("Inconsolata-Regular.ttf");
	if (version<20200424 && inconsolata_ok)
#endif
	{
		term_font_name = wx_font_name = "Inconsolata";
		unicode_opers = true;
	}
	
#ifdef __APPLE__
	// por alguna extraña razon no logro hacer que la terminal tome la fuente Inconsolata
	// (llamar a wxDC::SetFont(f) modifica a f y la convierte en Helvetica ¿?), asi que
	// uso una monospaced del sistema para salir del paso hasta que lo solucione
	if (version<20230426 and term_font_name=="Inconsolata")
		term_font_name="Menlo";
#endif
		
	lang.Log();
	
	er_init(temp_dir);
}

void ConfigManager::LoadDefaults() {
//	profile=DEFAULT_PROFILE;
	animate_gui=true;
	reorganize_for_debug=true;
	use_colors=true;
	show_debug_panel = false;
	maximized = false;
	psdraw_nocrop = false;
	shape_colors = true;
	unicode_opers = true;
	show_vars = false;
	show_opers = false;
	show_commands = true;
	autocomp = true;
	highlight_blocks = true;
	autoclose = true;
	calltip_helps = true;
	auto_quickhelp = true;
	size_x = size_y = 0;
	images_path = "imgs";
	tabw = 4;
	stepstep_tspeed = 50;
	debug_port  =55374;
	comm_port=55375;
	big_icons = OSDep::GetDPI()>=120;
	wxColour ref_color = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	use_dark_theme = (ref_color.Red()/255.f+ref_color.Green()/255.f+ref_color.Blue()/255.f)/3.f < 0.5f;
	use_dark_psdraw = use_dark_theme;
	use_dark_psterm = use_dark_theme;
	check_for_updates = true;
	last_update_check = 0;
	fixed_port = false;
	rt_syntax = true;
	rt_annotate= true;
	smart_indent = true;
	last_dir=wxFileName::GetHomeDir();
	
#if defined(_WIN32) || defined(__WIN32__)
	std::string bin_pre="", bin_post=".exe";
#elif defined(__APPLE__)
	std::string bin_pre="./", bin_post="";
#else
	std::string bin_pre="./bin/", bin_post="";
#endif
	pseint_command   = bin_pre+"pseint"+bin_post;
	psterm_command   = bin_pre+"psterm"+bin_post;
	psdrawe_command  = bin_pre+"psdrawE"+bin_post;
	psdraw3_command  = bin_pre+"psdraw3"+bin_post;
	psexport_command = bin_pre+"psexport"+bin_post;
	pseval_command   = bin_pre+"pseval"+bin_post;
	updatem_command  = bin_pre+"updatem"+bin_post;

	wx_font_size = big_icons?12:10;
	wx_font_name = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT).GetFaceName();
	print_font_size = wx_font_size-2;
	term_font_size = big_icons?14:11;
	term_font_name = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT).GetFaceName();
#ifdef __APPLE__
	// la fuente por defecto en mac es fea y muy chica...
	wx_font_size = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT).GetPointSize();
	term_font_size = wx_font_size+1;
	if (wxFontEnumerator::GetFacenames(wxFONTENCODING_SYSTEM).Index("Monaco")!=wxNOT_FOUND)
		term_font_name = wx_font_name  = "Monaco";
#endif
	
	help_dir = "help";
	proxy = "";
	profiles_dir = "perfiles";
	examples_dir = "ejemplos";
	temp_dir = home_dir;
}

void ConfigManager::Save() {
	wxTextFile fil(filename);
	if (fil.Exists())
		fil.Open();
	else
		fil.Create();
	fil.Clear();
	
	fil.AddLine(wxString("# generado por PSeInt ")<<VERSION<<"-" ARCHITECTURE ARCH_EXTRA);
	fil.AddLine(wxString("version=")<<VERSION);
	fil.AddLine(wxString("images_path=")<<images_path);
	if (temp_dir!=home_dir) fil.AddLine(wxString("temp_dir=")<<temp_dir); // evitar rutas absolutas al home, parece que puede cambiar si se renombra el usuario o se migran configuraciones de uno a otro
	fil.AddLine(wxString("last_dir=")<<last_dir);
	fil.AddLine(wxString("help_dir=")<<help_dir);
	fil.AddLine(wxString("proxy=")<<proxy);
	fil.AddLine(wxString("profiles_dir=")<<profiles_dir);
	fil.AddLine(wxString("examples_dir=")<<examples_dir);
	fil.AddLine(wxString("rt_syntax=")<<(rt_syntax?1:0));
	fil.AddLine(wxString("rt_annotate=")<<(rt_annotate?1:0));
	fil.AddLine(wxString("smart_indent=")<<(smart_indent?1:0));
	fil.AddLine(wxString("psdraw_nocrop=")<<(psdraw_nocrop?1:0));
	fil.AddLine(wxString("shape_colors=")<<(shape_colors?1:0));
	fil.AddLine(wxString("unicode_opers=")<<(unicode_opers?1:0));
	fil.AddLine(wxString("show_vars=")<<(show_vars?1:0));
	fil.AddLine(wxString("show_opers=")<<(show_opers?1:0));
	fil.AddLine(wxString("show_commands=")<<(show_commands?1:0));
	fil.AddLine(wxString("show_debug_panel=")<<(show_debug_panel?1:0));
	fil.AddLine(wxString("calltip_helps=")<<(calltip_helps?1:0));
	fil.AddLine(wxString("autocomp=")<<(autocomp?1:0));
	fil.AddLine(wxString("highlight_blocks=")<<(highlight_blocks?1:0));
	fil.AddLine(wxString("autoclose=")<<(autoclose?1:0));
	fil.AddLine(wxString("auto_quickhelp=")<<(auto_quickhelp?1:0));
	fil.AddLine(wxString("use_colors=")<<(use_colors?1:0));
	fil.AddLine(wxString("animate_gui=")<<(animate_gui?1:0));
	fil.AddLine(wxString("reorganize_for_debug=")<<(reorganize_for_debug?1:0));
	fil.AddLine(wxString("maximized=")<<(maximized?1:0));
	fil.AddLine(wxString("wx_font_size=")<<wx_font_size);
	fil.AddLine(wxString("print_font_size=")<<print_font_size);
	fil.AddLine(wxString("term_font_size=")<<term_font_size);
	fil.AddLine(wxString("wx_font_name=")<<wx_font_name);
	fil.AddLine(wxString("term_font_name=")<<term_font_name);
	fil.AddLine(wxString("tabw=")<<tabw);
	fil.AddLine(wxString("stepstep_tspeed=")<<stepstep_tspeed);
	fil.AddLine(wxString("size_x=")<<size_x);
	fil.AddLine(wxString("size_y=")<<size_y);
	fil.AddLine(wxString("pos_x=")<<pos_x);
	fil.AddLine(wxString("pos_y=")<<pos_y);
	if (fixed_port) {
		fil.AddLine(wxString("debug_port=")<<debug_port);	
		fil.AddLine(wxString("comm_port=")<<comm_port);	
	}
	fil.AddLine(wxString("use_dark_theme=")<<(use_dark_theme?1:0));	
	fil.AddLine(wxString("use_dark_psdraw=")<<(use_dark_psdraw?1:0));	
	fil.AddLine(wxString("use_dark_psterm=")<<(use_dark_psterm?1:0));	
	fil.AddLine(wxString("big_icons=")<<big_icons);
	fil.AddLine(wxString("check_for_updates=")<<(check_for_updates?1:0));	
	fil.AddLine(wxString("last_update_check=")<<last_update_check);	
	fil.AddLine(wxString("fixed_port=")<<(fixed_port?1:0));	
	for (unsigned int i=0;i<last_files.GetCount();i++)
		fil.AddLine(wxString("history=")<<last_files[i]);
	fil.AddLine("");
	fil.AddLine(wxString("profile=")<<_S2W(lang.name));
	if (lang.source==LS_FILE)      fil.AddLine(wxString("profile:source=file"));
	else if (lang.source==LS_LIST) fil.AddLine(wxString("profile:source=list"));
	else                           fil.AddLine(wxString("profile:source=custom"));
	for(int i=0;i<LS_COUNT;i++) fil.AddLine(wxString("profile:")+lang.GetConfigLine(i).c_str());
	fil.AddLine("");
	
	fil.Write();
	fil.Close();
	
}

void ConfigManager::Read() {
	wxTextFile fil(filename);
	if (!fil.Exists()) return;
	fil.Open();
	wxString key, value;
	long l;
	for ( wxString str = fil.GetFirstLine(); !fil.Eof(); str = fil.GetNextLine() ) {
		if (str[0]=='#') {
			continue;
		} else {
			key=str.BeforeFirst('=');
			value=str.AfterFirst('=');
			if (key=="version") { value.ToLong(&l); version=l; lang.Reset(version); }
			else if (key=="wx_font_size") { value.ToLong(&l); wx_font_size=l; }
			else if (key=="print_font_size") { value.ToLong(&l); print_font_size=l; }
			else if (key=="term_font_size") { value.ToLong(&l); term_font_size=l; }
			else if (key=="wx_font_name") { wx_font_name=value; }
			else if (key=="term_font_name") { term_font_name=value; }
			else if (key=="tabWidth") { value.ToLong(&l); tabw=l; }
			else if (key=="size_x") { value.ToLong(&l); size_x=l; }
			else if (key=="size_y") { value.ToLong(&l); size_y=l; }
			else if (key=="pos_x") { value.ToLong(&l); pos_x=l; }
			else if (key=="pos_y") { value.ToLong(&l); pos_y=l; }
			else if (key=="maximized") { maximized = utils->IsTrue(value); }
			else if (key=="debug_port") { value.ToLong(&l); debug_port=l; }
			else if (key=="comm_port") { value.ToLong(&l); comm_port=l; }
			else if (key=="big_icons") big_icons=utils->IsTrue(value);
			else if (key=="use_dark_theme") use_dark_theme=utils->IsTrue(value);
			else if (key=="use_dark_psdraw") use_dark_psdraw=utils->IsTrue(value);
			else if (key=="use_dark_psterm") use_dark_psterm=utils->IsTrue(value);
			else if (key=="check_for_updates") check_for_updates=utils->IsTrue(value);
			else if (key=="last_update_check") value.ToLong(&last_update_check);
			else if (key=="fixed_port") fixed_port=utils->IsTrue(value);
			else if (key=="stepstep_tspeed") { value.ToLong(&l); stepstep_tspeed=l; }
			else if (key=="rt_syntax") rt_syntax=utils->IsTrue(value);
			else if (key=="rt_annotate") rt_annotate=utils->IsTrue(value);
			else if (key=="smart_indent") smart_indent=utils->IsTrue(value);
			else if (key=="show_commands") show_commands=utils->IsTrue(value);
			else if (key=="show_vars") show_vars=utils->IsTrue(value);
			else if (key=="show_opers") show_opers=utils->IsTrue(value);
			else if (key=="show_debug_panel") show_debug_panel=utils->IsTrue(value);
			else if (key=="auto_quickhelp") auto_quickhelp=utils->IsTrue(value);
			else if (key=="calltip_helps") calltip_helps=utils->IsTrue(value);
			else if (key=="autocomp") autocomp=utils->IsTrue(value);
			else if (key=="highlight_blocks") highlight_blocks=utils->IsTrue(value);
			else if (key=="autoclose") autoclose=utils->IsTrue(value);
			else if (key=="psdraw_nocrop") psdraw_nocrop=utils->IsTrue(value);
			else if (key=="shape_colors") shape_colors=utils->IsTrue(value);
			else if (key=="unicode_opers") unicode_opers=utils->IsTrue(value);
			else if (key=="use_colors") use_colors=utils->IsTrue(value);
			else if (key=="animate_gui") animate_gui=utils->IsTrue(value);
			else if (key=="reorganize_for_debug") reorganize_for_debug=utils->IsTrue(value);
			else if (key=="images_path") images_path=value;
			else if (key=="help_dir") help_dir=value;
			else if (key=="proxy") proxy=value;
			else if (key=="profiles_dir") profiles_dir=value;
			else if (key=="profile") lang.name=value;
			else if (key=="examples_dir") examples_dir=value;
			else if (key=="last_dir") last_dir=value;
			else if (key=="temp_dir") temp_dir=value;
			else if (key=="history") last_files.Add(value);
			
			// new method
			else if (key.StartsWith("profile:")) lang.ProcessConfigLine(_W2S(key.AfterFirst(':')),_W2S(value));
			// old method
			else if (version<20180126) lang.ProcessConfigLine(_W2S(key),_W2S(value));
		}
	}
	fil.Close();
	if (version<20180126) {
		lang.source = lang.name=="<personalizado>" ? LS_CUSTOM : LS_LIST;
	}
	if (lang.source==LS_LIST) {
		// si era de la lista, luego de una actualización el perfil
		// puede haber cambiado... o la interpretación del mismo
		if (!LoadListedProfile(_S2W(lang.name))) lang.Fix();
	}
	if (version!=0 && version<20160321) temp_dir = home_dir;
	if (version<20150627) shape_colors = true;
	// asegurarse de que tamaños y posiciones de la ventana estén en el rango de 
	// la pantalla actual (por si se guardaron cuando había un segundo monitor que
	// ya no está)
	if (pos_x<0) pos_x = 0; if (pos_y<0) pos_y = 0;
	int screen_w = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	int screen_h = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	if (screen_h>0 && screen_w>0) {
		if (size_x>screen_w) size_x = screen_w;
		if (size_y>screen_h) size_y = screen_h;
		if (pos_x+size_x>screen_w) pos_x = screen_w-size_x;
		if (pos_y+size_y>screen_h) pos_y = screen_h-size_y;
	}
	if (!wxFileName::DirExists(temp_dir)) wxFileName::Mkdir(temp_dir);
}

ConfigManager::~ConfigManager() {
	config = NULL;
}

bool ConfigManager::LoadProfileFromFile(wxString path) {
	if (!lang.Load(path,false)) return false;
	return true;
}

bool ConfigManager::LoadListedProfile(wxString name) {
	if (!lang.Load(DIR_PLUS_FILE(profiles_dir,name),true)) return false;
	return true;
}

bool ConfigManager::SetProfile(LangSettings custom_lang) {
	lang = custom_lang;
	return true;
}

int ConfigManager::GetCommPort () {
	if (fixed_port) return comm_port; else return comm_port+rand()%150+150;
}

int ConfigManager::GetDebugPort ( ) {
	if (fixed_port) return debug_port; else return debug_port+rand()%150;
}

wxString ConfigManager::GetTermCommand ( ) {
	return psterm_command + (config->use_dark_psterm?" --darktheme":"")
		+ " \"--font="<<config->term_font_name<<":"<<config->term_font_size<<"\"";
}

void ConfigManager::Log ( ) const {
	Logger::DumpVersions();
	_LOG("ConfigManager");
	_LOG("   cwd="<<wxGetCwd());
	_LOG("   filename="<<filename);
	_LOG("   profile="<<_S2W(lang.name));
	_LOG("   pseint_dir="<<pseint_dir);
	_LOG("   home_dir="<<home_dir);
	_LOG("   psdraw3_command="<<psdraw3_command);
	_LOG("   psdrawe_command="<<psdrawe_command);
	_LOG("   pseint_command="<<pseint_command);
	_LOG("   pseval_command="<<pseval_command);
	_LOG("   psexport_command="<<psexport_command);
	_LOG("   psterm_command="<<psterm_command);
	_LOG("   temp_dir="<<temp_dir);
}

