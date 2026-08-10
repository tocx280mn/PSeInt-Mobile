#ifndef PACKAGE_H
#define PACKAGE_H
#include <set>
#include <wx/string.h>
#include <wx/arrstr.h>
#include <map>

#define PACKAGE_VERSION_NO_PROFILE 1 // version del formato de paquete, incrementar cada vez que cambie
#define PACKAGE_VERSION 2 // version del formato de paquete, incrementar cada vez que cambie

/// struct para guardar datos de un caso de prueba
struct TestCase { 
	wxString input, output, solution;
};

/// carga y gestiona todos los datos de un ejercicio (todo lo que contiene el archivo de ejercicio)
class Package {
private:
	bool m_is_ok; ///< si se cargo correctamente
	std::map<wxString,TestCase> m_tests; ///< mapea los nombres de los tests a sus entradas/salidas (de los pares de archivos input??.txt/output??.txt, donde ?? es el nombre)
	std::map<wxString,wxString> m_config; ///< palabras claves de config.txt, indican opciones de configuracion de como probar y mostrar los resultados
	wxString m_base_psc; ///< codigo inicial a mostrar en el editor
	wxString m_help_text; ///< texto de ayuda para el problema, opcional
	wxString m_profile; ///< perfil obligatorio para el ejercicio (o en blanco si no requiere ninguno en particular)
	void ProcessFile(wxString name, wxString &content);
	wxArrayString m_images;
public:
	Package();
	void Reset();
	bool Load(const wxString &fname, const wxString &passkey); ///< carga todos los datos del ejercicio a partir del archivo que recibe como argumento
	bool Load(const wxString &dir); ///< carga desde un directorio, para el generador de packages
	bool Save(const wxString &fname, const wxString &passkey, bool old_cypher); ///< genera el zip
	TestCase &GetTest(const wxString &name); ///< obtiene los datos de un caso a partir de su nombre (se asume que el nombre existe)
	int GetNames(wxArrayString &names); ///< colocar en el arreglo los nombres de todos los casos de prueba, y retorna la cantidad
	bool IsInConfig(const wxString &key); 
	wxString &GetConfigStr(const wxString &key); 
	bool GetConfigBool(const wxString &key); 
	long GetConfigInt(const wxString &key); 
	void SetConfigStr(const wxString &key, const wxString &value); 
	void SetConfigBool(const wxString &key, bool value); 
	void SetConfigInt(const wxString &key, long value); 
	const wxString &GetBaseSrc() const { return m_base_psc; }
	const wxString &GetHelp() const { return m_help_text; }
	bool IsOk() { return m_is_ok; }
	bool SaveConfig (const wxString &fname);
	void UnloadImages();
	wxString GetFullConfig();
};

#endif

