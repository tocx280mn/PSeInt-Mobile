#include "global.h"

//LangSettings lang(LS_DO_NOT_INIT); // no definir aqui, para forzar que cada proyecto lo defina y no se olvide de inicializarlo

std::queue<std::string> predef_input; // entrada predefinida para los tests
std::ofstream ExeInfo; // Archivo de Inforamacion de Ejecucion para el entorno
bool ExeInfoOn=false;
bool wait_key;
bool for_test=false;
bool raw_errors=false;
bool colored_output=false;
bool noinput=false;
bool fix_win_charset=false;
bool remembar_case=false;
bool for_pseint_terminal=false;
bool with_io_references=false;
bool for_eval=false;
bool ignore_logic_errors=false;
bool preserve_comments=false;
