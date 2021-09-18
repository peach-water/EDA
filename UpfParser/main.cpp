
#include "TclCommand.h"
#include "UpfModel.h"

int main(int argc, char *argv[])
{
    std::string upfTopFile;
    for (int i = 1; i != argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-f") {
            upfTopFile = argv[++i];
        }
    }
    // if (upfTopFile.empty()) {
    //     return -1;
    // }
    UpfCommandMgr::getInstance()->readUpfFile("example.upf");
    //do power analysis;
    PwObjMgr::getInstance()->print();
    PwObjMgr::getInstance()->clear();
    return 0;
}
