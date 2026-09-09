#pragma once
#include <string>
#include <vector>
#include <utility>
#include <functional>

// Desktop 8-bit text. Only console/debugger transport is replaced on mobile.
struct MobileCallbacks {
    std::function<void(const std::string&)> output;
    std::function<std::string(const std::string&)> input;
    std::function<void(int, int, const std::string&, bool)> diagnostic;
    std::function<void(int, const std::vector<std::pair<std::string,std::string>>&)> step;
    std::function<void()> checkpoint;
};
struct MobileCancelled {};
struct MobileRuntimeError {};
struct MobileEndOfInput {};
struct MobileDepthGuard {
    MobileDepthGuard();
    ~MobileDepthGuard();
};
int mobileRun(const std::string &source, const std::string &flags, bool execute,
              bool debug, MobileCallbacks &callbacks, bool testMode = false);
void mobileCheckpoint();
void mobileStep();
int mobileArrayBase();
void mobileSleep(long long milliseconds);
std::string mobileRead(const std::string &variable);
