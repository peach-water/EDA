#ifndef PARSER_NETWORK_ANALYSIS_H
#define PARSER_NETWORK_ANALYSIS_H
#include <iostream>
#include "UpfModel.h"
#include <fstream>
#include <unordered_set>
#include <unistd.h>

class unionPst
{
private:
    static unionPst *m_inst;
    std::vector<std::string> *m_header;  // 存放表头标题
    std::vector<PwPstState *> *m_states; // 存放数据 TODO更换为map方便根据状态查找
    std::fstream m_file;                 // 将Pst重复的内容输出到文件

    // 接下来是表格合并会使用到的数据
    std::unordered_map<std::string, std::pair<int, int>> m_samePortIndex; // 存放两张表格中表头相同的下标
    std::unordered_map<std::string, std::pair<int, int>> m_newPortIndex;  // 存放新增加的表项目
    std::unordered_set<int> m_unUsedPortIndex;                            // 记录下旧表中没有出现过的表头的下标
    int l_counter = 0;

public:
    unionPst();
    ~unionPst();

    static unionPst *getInstance();

    void mergePst(PwScope *scope);                                                                                     // 合并表格
    std::vector<PwSupplyState *> mergeRow(std::vector<PwSupplyState *> *oldRow, std::vector<PwSupplyState *> *newRow); // 合并一行的内容
    void outputPst();                                                                                                  // 输出表格
    std::vector<PwPstState *> *deDublicatePst(std::vector<PwPstState *> *newPst);                                      // 给新表去重，同时输出重复状态信息
    
};

class NetworkAnalysis
{
private:
    std::unordered_map<std::string, std::string> m_sourceMap;                      // 查询，会返回最上面一层的节点，也就是当前位置源自哪里
    std::unordered_map<std::string, std::unordered_set<std::string>> m_endNetMap;  // 查询，返回Port相关的Net
    std::unordered_map<std::string, std::unordered_set<std::string>> m_endPortMap; // 查询，返回Net相关的Net
    std::unordered_set<std::string> m_supplyList;                                  // 记录下哪一些是source Supply
    static NetworkAnalysis *m_inst;
    std::fstream m_outputfile; // 文件指针

public:
    NetworkAnalysis();
    ~NetworkAnalysis();

    static NetworkAnalysis *getInstance(); // 获得分析器
    void networkAnaly();                   // 分析当前Scope下的源节点
    void DFSScopeAnaly();                  // 循环调用上面函数，直到所有节点军建立起
    void buildNetwork();                   // 建立起连接关系
    void outputNetwork();                  // 输出链接关系。这个目前就是source.csv输出内容  TODO：做一个写文件函数
    void gnd_and_unused_check();           // 检查gnd合格与未用到状态

    std::string GetSourcePortName(const std::string in_port_name) // 对外的接口，用于查找当前接口源接口
    {
        return this->m_sourceMap[in_port_name];
    }
};

void printMap(const std::unordered_map<std::string, std::string> &Map); // 检查错误使用，不要理会

#endif