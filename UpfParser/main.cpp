#include <iostream>
#include "TclCommand.h"
#include "UpfModel.h"
#include "NetworkAnalysis.h"

void my_poweranaly()
{

    NetworkAnalysis *netAnaly = NetworkAnalysis::getInstance();
    netAnaly->buildNetwork();

    netAnaly->outputNetwork();

    return;
}

//这一个函数和上面的函数一样，不同的是用的是cout输出形式，输出的只是topScope

int main(int argc, char *argv[])
{
    std::string upfTopFile;
    for (int i = 1; i != argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-f")
        {
            upfTopFile = argv[++i];
        }
    }
    if (upfTopFile.empty())
    {
        printf("file not exist");
        return -1;
    }
    UpfCommandMgr::getInstance()->readUpfFile(upfTopFile);
    // UpfCommandMgr::getInstance()->readUpfFile("test.upf");
    // do power analysis;
    my_poweranaly();
    // PwObjMgr::getInstance()->print();
    PwObjMgr::getInstance()->clear();
    return 0;
}
