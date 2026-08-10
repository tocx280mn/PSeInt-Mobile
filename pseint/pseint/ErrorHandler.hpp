#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H
#include <string>
#include "Code.h"

/// MAX ERROR CODED USED: 333

class ErrorHandler {
public:
	void SyntaxError(int num, const std::string &desc);
	void SyntaxError(int num, const std::string &desc, CodeLocation loc);
	void ExecutionError(int num, const std::string &desc);
	void CompileTimeWarning(int num, const std::string &desc);
	void RunTimeWarning(int num, const std::string &desc);
	void ErrorIfRunning(int num, const std::string &desc);
	void AnytimeError(int num, const std::string &desc);
	bool IsOk() const { return m_errors_count==0; }
	int GetErrorsCount() const { return m_errors_count; }
private:
	int m_errors_count = 0, m_warnings_count = 0;
	
	// hasta que haga las herencias e integre el interprete como lib a los otros modulos
	// necesito esto para psterm
	bool m_disable_compile_time_warnings = false;
public:
	void DisableCompileTimeWarnings() { m_disable_compile_time_warnings = true; }
};

#endif

