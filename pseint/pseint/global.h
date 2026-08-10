#ifndef GLOBAL_H
#define GLOBAL_H
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include "LangSettings.h"

// *********************** Variables Globales ****************************
extern std::queue<std::string> predef_input;
extern std::ofstream ExeInfo; // Archivo de Informacion de Ejecucion para el entorno
extern bool ExeInfoOn;
extern bool wait_key;
extern LangSettings lang;
extern bool colored_output;
extern bool for_test; // no hacer nada con las instrucciones EsperarTecla y LimpiarPantalla para no arruinar la automatización de los tests (tambien usado en psexport para no poner comentarios ni lineas en blanco)
extern bool raw_errors; // salida para test automatico, sin mensajes explicativos ni adornos
extern bool noinput; // no pedir entradas al usuario, tomar de los argumentos
extern bool fix_win_charset; // indica si hay que cambiar los caracteres de acentos y ñs de las cadenas para que se vean bien en la consola de windows
extern bool for_pseint_terminal; // habilita algunas secuencias de escape adicionales para usar en la terminal propia
extern bool with_io_references; // habilita algunas secuencias de escape adicionales para que la terminal propia sepa qué instrucción en el código generó cada entrada/salida
extern bool preserve_comments; // guardar los comentarios en el archivo de salida (cuando es para psexport o psdraw)
extern bool for_eval; // formateado para psEval
extern bool ignore_logic_errors; // para que al generar el diagrama de flujo no marque ciertos errores que no afectan al diagrama (como los de tipos en expresiones)

#define COLOR_INFO COLOR_LBLUE
#define COLOR_ERROR COLOR_LRED
#define COLOR_WARNING COLOR_INFO
#define COLOR_INPUT COLOR_YELLOW
#define COLOR_OUTPUT COLOR_LGREEN

#endif
