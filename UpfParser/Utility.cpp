#include "Utility.h"

void splitStringIntoTokens(const std::string& str, std::list<std::string>& strlist, const std::string& delimiter) {
    size_t pos1 = 0;
    size_t pos2 = str.find_first_of(delimiter);
    if (pos2 == std::string::npos) {
        strlist.push_back(str);
        return;
    }
    std::string tmpStr = str.substr(pos1, pos2 - pos1 );
    strlist.push_back(tmpStr);
    while (pos2 != std::string::npos) {
        pos1 = pos2 + 1;
        pos2 = str.find_first_of(delimiter, pos1);
        tmpStr = (pos2 == std::string::npos)? str.substr(pos1) : str.substr(pos1, pos2 - pos1 );
        strlist.push_back(tmpStr);
    }
}

void trimSpace(std::string& hierName) {
    size_t pos1 = hierName.find_first_not_of(" ");
    size_t pos2 = hierName.find_last_not_of(" ");
    if (pos2 > pos1) {
        hierName = hierName.substr(pos1);
        if (hierName[hierName.length() - 1] == ' ') {
            hierName.erase(pos2 + 1);
        }
    }
}