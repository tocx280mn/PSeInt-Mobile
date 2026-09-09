#include "mobile_runtime.h"
#include "RunTime.hpp"
#include "global.h"
#include "intercambio.h"
#include "SynCheck.hpp"
#include "Ejecutar.hpp"
#include "zcurlib.h"
#include <sstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <ctime>

LangSettings lang(LS_DO_NOT_INIT);
static std::mutex engineMutex;
static MobileCallbacks *client = nullptr;
static bool debugging = false;
static int executionDepth = 0;

MobileDepthGuard::MobileDepthGuard() {
    mobileCheckpoint();
    if (executionDepth >= 128) {
        client->diagnostic(Inter.GetLocation().linea,1004,"Limite de 128 llamadas o bloques anidados excedido.",false);
        throw MobileRuntimeError();
    }
    ++executionDepth;
}
MobileDepthGuard::~MobileDepthGuard() { --executionDepth; }

void mobileCheckpoint() { if (client) client->checkpoint(); }
int mobileArrayBase() { return lang[LS_BASE_ZERO_ARRAYS] ? 0 : 1; }

void mobileStep() {
    mobileCheckpoint();
    if (!debugging || !Inter.IsRunning()) return;
    std::vector<std::pair<std::string,std::string>> values;
    if (memoria) {
        // The debugger must not initialize variables or change type inference.
        for (const auto &p : memoria->MobileVariableNames()) {
            const auto &name = p.first;
            if (name.empty() || !memoria->Existe(name)) continue;
            const int *dims = memoria->LeerDims(name);
            std::string value;
            if (dims) {
                value = "[Arreglo ";
                for (int i=1;i<=dims[0];++i) value += (i==1?"":" x ") + std::to_string(dims[i]);
                value += "]";
            } else if (memoria->EstaInicializada(name)) {
                value = memoria->Leer(name).GetForUser();
            } else value = "Sin inicializar";
            values.emplace_back(name, value);
        }
    }
    client->step(Inter.GetLocation().linea, values);
}

void mobileSleep(long long milliseconds) {
    const auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(milliseconds);
    while (std::chrono::steady_clock::now() < end) {
        mobileCheckpoint();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    mobileCheckpoint();
}

std::string mobileRead(const std::string &variable) {
    mobileCheckpoint();
    try { return client->input(variable); }
    catch (const MobileEndOfInput &) {
        client->diagnostic(Inter.GetLocation().linea,214,"Sin entradas disponibles.",false);
        throw MobileRuntimeError();
    }
}

class OutputBuffer : public std::streambuf {
    std::string pending;
    int overflow(int ch) override {
        if (ch != traits_type::eof()) { pending += char(ch); if (ch=='\n' || pending.size()>=4096) sync(); }
        return traits_type::not_eof(ch);
    }
    std::streamsize xsputn(const char *data, std::streamsize n) override {
        pending.append(data, static_cast<size_t>(n));
        if (pending.find('\n')!=std::string::npos || pending.size()>=4096) sync();
        return n;
    }
    int sync() override {
        if (!pending.empty()) { auto text=std::move(pending); pending.clear(); client->output(text); }
        return 0;
    }
};

int mobileRun(const std::string &source, const std::string &flags, bool execute,
              bool debug, MobileCallbacks &callbacks, bool testMode) {
    std::unique_lock<std::mutex> lock(engineMutex, std::defer_lock);
    try {
        while (!lock.try_lock()) { callbacks.checkpoint(); std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
    } catch (const MobileCancelled &) { return 3; }
    client=&callbacks; debugging=debug; executionDepth=0;
    OutputBuffer buffer;
    auto previousBuffer=std::cout.rdbuf(&buffer);
    auto previousExceptions=std::cout.exceptions();
    std::cout.exceptions(std::ios::badbit);
    int result=0;
    try {
        lang.Reset();
        if (!lang.SetFromSingleString(flags)) throw std::invalid_argument("Perfil nativo incompleto");
        lang.Fix();
        Inter.SetFinished();
        Inter.SetLocation({1,1});
        Inter.subtitles_on=false;
        for_test=testMode;
        srand(testMode ? 1 : static_cast<unsigned int>(time(nullptr)));
        raw_errors=colored_output=noinput=fix_win_charset=for_pseint_terminal=false;
        for_eval=ignore_logic_errors=with_io_references=preserve_comments=wait_key=false;
        while (!predef_input.empty()) predef_input.pop();
        RunTime rt;
        rt.funcs.LoadPredefs();
        std::istringstream lines(source);
        for (std::string line; std::getline(lines,line);) { mobileCheckpoint(); rt.prog.PushBack(line); }
        SynCheck(rt);
        if (!rt.err.IsOk()) result=1;
        else if (execute && rt.funcs.HaveMain()) {
            for (auto &function : rt.funcs.GetAllSubs()) function.second->memoria->FakeReset();
            Inter.SetStarted();
            auto main=rt.funcs.GetMainFunc();
            memoria=main->memoria.get();
            Inter.SetLocation(rt.prog[main->line_start].loc);
            Ejecutar(rt,main->line_start);
        }
        std::cout.flush();
    } catch (const MobileRuntimeError &) { result=2; }
    catch (const MobileCancelled &) { result=3; }
    catch (const std::exception &e) {
        result=2;
        try { callbacks.diagnostic(Inter.GetLocation().linea,0,e.what(),false); } catch (...) { result=3; }
    }
    // Also runs after cancellation inside a subprogram or while awaiting input.
    Inter.SetFinished(); memoria=nullptr;
    std::cout.exceptions(std::ios::goodbit);
    std::cout.rdbuf(previousBuffer); std::cout.clear(); std::cout.exceptions(previousExceptions);
    client=nullptr;
    return result;
}

static void diagnostic(int number,const std::string &message,bool warning,int line) {
    mobileCheckpoint();
    client->diagnostic(line,number,message,warning);
}
void ErrorHandler::SyntaxError(int n,const std::string &s) { SyntaxError(n,s,Inter.GetLocation()); }
void ErrorHandler::SyntaxError(int n,const std::string &s,CodeLocation loc) {
    ++m_errors_count; diagnostic(n,s,false,loc.linea);
}
void ErrorHandler::ExecutionError(int n,const std::string &s) {
    ++m_errors_count; diagnostic(n,s,false,Inter.GetLocation().linea); throw MobileRuntimeError();
}
void ErrorHandler::CompileTimeWarning(int n,const std::string &s) {
    if (!m_disable_compile_time_warnings && !Inter.IsRunning()) { ++m_warnings_count; diagnostic(n,s,true,Inter.GetLocation().linea); }
}
void ErrorHandler::RunTimeWarning(int n,const std::string &s) {
    if (Inter.IsRunning()) { ++m_warnings_count; diagnostic(n,s,true,Inter.GetLocation().linea); }
}
void ErrorHandler::AnytimeError(int n,const std::string &s) { if(Inter.IsRunning()) ExecutionError(n,s); else SyntaxError(n,s); }
void ErrorHandler::ErrorIfRunning(int n,const std::string &s) { if(Inter.IsRunning()) ExecutionError(n,s); }

void setBackColor(int) {} void setForeColor(int) {}
void clrscr() { client->output("\f"); }
void gotoXY(int,int) {} void hideCursor() {} void showCursor() {} void setTitle(const char*) {}
int getKey() { mobileRead("Presiona Enviar para continuar"); return 13; }
std::string getLine() { return mobileRead("Entrada"); }
