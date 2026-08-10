#include "ErrorHandler.hpp"
#include "utils.h"
#include "intercambio.h"

void ErrorHandler::SyntaxError (int num, const std::string & desc) {
	++m_errors_count;
	SynError_impl(num,desc);
}

void ErrorHandler::SyntaxError (int num, const std::string & desc, CodeLocation loc) {
	++m_errors_count;
	SynError_impl(num,desc,loc);
}

void ErrorHandler::ExecutionError (int num, const std::string & desc) {
	++m_errors_count;
	ExeError_impl(num,desc);
}

void ErrorHandler::RunTimeWarning(int num, const std::string & desc) {
	++m_warnings_count;
	WarnError_impl(num,desc,true);
}

void ErrorHandler::CompileTimeWarning(int num, const std::string & desc) {
	if (m_disable_compile_time_warnings) return;
	++m_warnings_count;
	WarnError_impl(num,desc,false);
}

void ErrorHandler::AnytimeError (int num, const std::string & desc) {
	if (Inter.IsRunning()) ExecutionError(num,desc);
	else                   SyntaxError(num,desc);
}

void ErrorHandler::ErrorIfRunning(int num, const std::string & desc) {
	if (Inter.IsRunning()) ExecutionError(num,desc);
}

