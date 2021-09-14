#ifndef PARSER_UTILITY_H
#define PARSER_UTILITY_H
#include <list>
#include <string>
void splitStringIntoTokens(const std::string& str, std::list<std::string>& strlist, const std::string& delimitevimr);

void trimSpace(std::string& hierName);
#endif //PARSER_UTILITY_H
