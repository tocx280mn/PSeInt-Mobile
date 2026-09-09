// Test-only CLI: normal execution delegates unchanged to desktop's original main.
#include "LangSettings.h"
#include <iostream>
#include <string>
int pseintDesktopMain(int argc,char **argv);
int main(int argc,char **argv) {
    if (argc==3 && std::string(argv[1])=="--profile-flags") {
        LangSettings profile(LS_INIT);
        if (!profile.Load(std::string(argv[2]))) return 1;
        std::cout << profile.GetAsSingleString() << '\n';
        return 0;
    }
    return pseintDesktopMain(argc,argv);
}
