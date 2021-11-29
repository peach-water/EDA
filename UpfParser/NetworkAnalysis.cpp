#include <iomanip>
#include <cmath>
#include "NetworkAnalysis.h"

void printMap(const std::unordered_map<std::string, std::string> &Map)
{
    auto temp = Map.begin();
    for (; temp != Map.end(); temp++)
    {
        printf("%s -> %s \n", temp->first.c_str(), temp->second.c_str());
    }
}

unionPst::unionPst()
{
    this->m_header = new std::vector<std::string>;
    this->m_states = new std::vector<PwPstState *>;
    this->m_file.open("duplicatePst.out", std::ios::out); // 打开文件
    while (!m_file.is_open())
    {
        printf("无法打开文件\n");
        sleep(1);
        this->m_file.open("duplicatePst.out", std::ios::out);
    }
}
unionPst::~unionPst()
{
    this->m_states->clear();
    delete this->m_states;
    this->m_header->clear();
    delete this->m_header;
    delete this->m_inst;
    this->m_inst = nullptr;
    this->m_file.close();
}

unionPst *unionPst::getInstance()
{
    if (m_inst == nullptr)
    {
        m_inst = new unionPst;
    }
    return m_inst;
}

void unionPst::mergePst(PwScope *scope)
{
    NetworkAnalysis *analy = NetworkAnalysis::getInstance();
    for (auto i = scope->beginPsts(); i != scope->endPsts(); i++)
    {
        std::vector<PwPstState *> *l_newPst = deDublicatePst((*i)->getPstStates()); // 接收到的去重表格

        if (this->m_header->size() == 0 && this->m_states->size() == 0) // 如果当前还没有表格，直接将当前收到的Pst加入
        {

            for (auto header = (*i)->beginHeaders(); header != (*i)->endHeaders(); header++)
            {
                this->m_header->push_back(analy->GetSourcePortName((*header)->getHierName()));
            }
            for (auto pststate = l_newPst->begin(); pststate != l_newPst->end(); pststate++)
            {
                this->m_states->push_back(*pststate);
            }
        }
        else // 否则开始合并表格
        {
            int x = 0, y = 0; // 这是记录下标的

            for (size_t j = 0; j < this->m_header->size(); j++)
            {
                this->m_unUsedPortIndex.insert((int)j);
                (*m_header)[j] = analy->GetSourcePortName((*m_header)[j]);
            }
            std::vector<std::string> l_header;
            for (auto new_header = (*i)->beginHeaders(); new_header != (*i)->endHeaders(); new_header++)
            {
                const std::string new_header_name = (*new_header)->getHierName();
                l_header.push_back(analy->GetSourcePortName(new_header_name));
            }
            // 查找两张表都有的表头
            for (auto now_header = this->m_header->begin(); now_header != this->m_header->end(); now_header++)
            {
                const std::string now_header_name = analy->GetSourcePortName((*now_header));
                // for (auto new_header = (*i)->beginHeaders(); new_header != (*i)->endHeaders(); new_header++)
                for (size_t new_header = 0; new_header != l_header.size(); new_header++)
                {                  
                    const std::string new_header_name = l_header[new_header];

                    if (now_header_name == new_header_name)
                    {
                        std::pair<int, int> temp(x, y); // 先保存当前的index，再保存新表格的index
                        this->m_samePortIndex[now_header_name] = temp;
                        this->m_unUsedPortIndex.erase(this->m_unUsedPortIndex.find(x)); // 从旧表中从未使用过的下标中删除
                    }
                    y++;
                }
                x++;
                y = 0;
            }
            x = 0;
            // 计算新添加了表头
            for (size_t new_header = 0; new_header != l_header.size(); new_header++)
            {
                const std::string new_header_name = l_header[new_header];
                // 表示需要添加新内容
                if (this->m_samePortIndex.find(new_header_name) == this->m_samePortIndex.end())
                {
                    y = this->m_header->size();
                    std::pair<int, int> temp(y, x); // 先写旧表下标，再写新表下标
                    this->m_header->push_back(new_header_name);
                    this->m_newPortIndex[new_header_name] = temp;
                }
                x++;
            }
            l_header.clear();

            std::vector<PwPstState *> *new_m_states = new std::vector<PwPstState *>; // 创建一张全新的表格
            int merge_size = this->m_samePortIndex.size(), now_size = 0;             // 分别用来记录两张表一共有多少表头重合，现在多少重合
            for (auto now_state = this->m_states->begin(); now_state != this->m_states->end(); now_state++)
            {
                for (auto new_state = l_newPst->begin(); new_state != l_newPst->end(); new_state++)
                {
                    std::vector<PwSupplyState *> *now_row = (*now_state)->getStates();
                    std::vector<PwSupplyState *> *new_row = (*new_state)->getStates();
                    for (auto pairs = this->m_samePortIndex.begin(); pairs != this->m_samePortIndex.end(); pairs++)
                    {
                        // 检查是否满足行合并的条件
                        if ((*now_row)[pairs->second.first]->getValue() == (*new_row)[pairs->second.second]->getValue())
                        {
                            now_size++;
                        }
                        else
                        {
                            break; // 已经对应不上，不需要检查下一个
                        }
                    }
                    // 满足条件开始合并
                    if (now_size == merge_size)
                    {
                        std::vector<PwSupplyState *> row = mergeRow(now_row, new_row);
                        PwPstState *tmp = new PwPstState(*i, nullptr, "S", row);
                        new_m_states->push_back(tmp);
                    }
                    now_size = 0; // 归零准备下一次
                }
            }
            // 写成class内变量，这样可以在类内其他函数调用
            this->m_samePortIndex.clear();
            this->m_unUsedPortIndex.clear();
            this->m_newPortIndex.clear();
            std::vector<PwPstState *> *tmp = this->m_states;
            this->m_states = new_m_states;

            // 释放之前的空间
            if (l_counter > 0)
                for (auto j = tmp->begin(); j != tmp->end(); j++)
                {
                    delete *j;
                }
            delete tmp;
            l_counter = 1;
        }
        delete l_newPst;
    }
}

std::vector<PwSupplyState *> unionPst::mergeRow(std::vector<PwSupplyState *> *oldRow, std::vector<PwSupplyState *> *newRow)
{
    std::vector<PwSupplyState *> l_result;
    l_result.resize(this->m_header->size());
    // 先合并新的列
    for (auto i = this->m_newPortIndex.begin(); i != this->m_newPortIndex.end(); i++)
    {
        l_result[i->second.first] = (*newRow)[i->second.second];
    }
    // 合并同时出现的列
    for (auto i = this->m_samePortIndex.begin(); i != this->m_samePortIndex.end(); i++)
    {
        l_result[i->second.first] = (*oldRow)[i->second.first];
    }
    // 合并新表中没有出现的列
    for (auto i = this->m_unUsedPortIndex.begin(); i != this->m_unUsedPortIndex.end(); i++)
    {
        l_result[*i] = (*oldRow)[*i];
    }

    return l_result;
}

std::vector<PwPstState *> *unionPst::deDublicatePst(std::vector<PwPstState *> *newPst)
{
    std::string l_temp;                                                  // 字符串临时变量
    std::vector<PwPstState *> *l_result = new std::vector<PwPstState *>; // 返回结果
    std::unordered_map<std::string, PwPstState *> l_deDublicate;         // 去重表

    for (auto i = newPst->begin(); i != newPst->end(); i++)
    {
        l_temp.clear();
        for (auto j = (*i)->beginStates(); j != (*i)->endStates(); j++)
        {
            l_temp += (*j)->getName();
        }
        if (l_deDublicate.find(l_temp) != l_deDublicate.end())
        {
            this->m_file << l_deDublicate[l_temp]->getHierName() << "    " << (*i)->getHierName() << std::endl;
        }
        else
        {
            l_deDublicate[l_temp] = (*i);
            l_result->push_back(*i);
        }
    }
    l_deDublicate.clear();
    l_temp.clear();
    return l_result;
}

void unionPst::outputPst()
{

    std::fstream file;
    file.open("pst.csv", std::ios::out);
    if (!file.is_open())
    {
        printf("pst.csv file open fail\n");
        return;
    }

    for (auto i = this->m_header->begin(); i != this->m_header->end(); i++)
    {
        file << *i << " ";
    }
    file << std::endl;
    for (auto i = this->m_states->begin(); i != this->m_states->end(); i++)
    {
        for (auto j = (*i)->beginStates(); j != (*i)->endStates(); j++)
        {
            // printf("%f ",(*j)->getValue());
            double t = (*j)->getValue();
            if (t == 0)
            {
                file << "off ";
            }
            else
            {
                if (std::abs(std::round(t) - t) < 1e-6)
                {
                    file << std::fixed << std::setprecision(1) << t << ' ';
                    file.unsetf(std::ios_base::fixed);
                    file << std::setprecision(6);
                }
                else
                {
                    file << t << ' ';
                }
            }
        }
        file << std::endl;
    }
    file << std::endl;
    file.close();
}

NetworkAnalysis::NetworkAnalysis(/* args */)
{
}

NetworkAnalysis::~NetworkAnalysis()
{
    for (auto i = this->m_endNetMap.begin(); i != m_endNetMap.end(); i++)
    {
        (*i).second.clear();
    }
    this->m_endNetMap.clear();
    for (auto i = this->m_endPortMap.begin(); i != m_endPortMap.end(); i++)
    {
        (*i).second.clear();
    }
    this->m_endPortMap.clear();
    this->m_sourceMap.clear();
    this->m_supplyList.clear();
    this->m_outputfile.close();
}

NetworkAnalysis *NetworkAnalysis::getInstance()
{
    if (m_inst == nullptr)
    {
        m_inst = new NetworkAnalysis;
    }
    return m_inst;
}

void NetworkAnalysis::networkAnaly()
{
    PwObjMgr *manager = PwObjMgr::getInstance();
    PwScope *current = manager->getCurrentScope();

    std::unordered_map<std::string, int> sourcePortMap;                  // TODO:可以考虑换成set
    std::vector<std::pair<PwSupplyNet *, PwSupplyPort *>> l_unused_Conn; // 记录下还没有使用过的链接
    std::unordered_map<void *, std::string> l_point_to_name;             // 记录下Port或者Net指针调用getHierName的结果

    for (auto i = current->beginSupplyPorts(); i != current->endSupplyPorts(); ++i)
    {
        const std::string l_supplyPort_Name = (*i)->getHierName();
        sourcePortMap[l_supplyPort_Name] = 1;
        l_point_to_name[(*i)] = l_supplyPort_Name;
    }

    for (auto i = current->beginSupplyConn(); i != current->endSupplyConn(); ++i)
    {
        std::string l_supplyPort_Name = "";
        std::string l_supplyNet_Name = "";
        if (l_point_to_name.find((*i)->getSupplyPort()) == l_point_to_name.end())
        {
            l_supplyPort_Name = (*i)->getSupplyPort()->getHierName();
            l_point_to_name[(void *)(*i)->getSupplyPort()] = l_supplyPort_Name;
        }
        else
        {
            l_supplyPort_Name = l_point_to_name[(*i)->getSupplyPort()];
        }
        if (l_point_to_name.find((*i)->getSupplyNet()) == l_point_to_name.end())
        {
            l_supplyNet_Name = (*i)->getSupplyNet()->getHierName();
            l_point_to_name[(void *)(*i)->getSupplyNet()] = l_supplyNet_Name;
        }
        else
        {
            l_supplyNet_Name = l_point_to_name[(*i)->getSupplyNet()];
        }

        // Port在外面的定义，判断条件是out端口同时相连的net没有source端口
        if (sourcePortMap.find((l_supplyPort_Name)) != sourcePortMap.end() || ((*i)->getSupplyPort()->isOutput() && this->m_sourceMap.find(l_supplyNet_Name) == this->m_sourceMap.end()))
        {
            // 这里是初始化一个新的端口
            if (this->m_sourceMap.find(l_supplyPort_Name) == this->m_sourceMap.end())
            {
                this->m_sourceMap[l_supplyPort_Name] = l_supplyPort_Name;
                this->m_endNetMap[l_supplyPort_Name].clear();
                this->m_endPortMap[l_supplyPort_Name].clear();
                this->m_endPortMap[l_supplyPort_Name].insert(l_supplyPort_Name);
                this->m_supplyList.insert(l_supplyPort_Name);
            }
            this->m_sourceMap[l_supplyNet_Name] = this->m_sourceMap[l_supplyPort_Name];
            this->m_endNetMap[this->m_sourceMap[l_supplyPort_Name]].insert(l_supplyNet_Name);
        }
        else // source是Net处理
        {
            std::pair<PwSupplyNet *, PwSupplyPort *> l_temp; // 临时变量
            l_temp.first = (*i)->getSupplyNet();
            l_temp.second = (*i)->getSupplyPort();
            l_unused_Conn.push_back(l_temp);
        }
    }

    // 开始处理Net链接的东西，防止conn最开始的一项没有连接Port出现BUG
    for (auto i = l_unused_Conn.begin(); i != l_unused_Conn.end(); i++)
    {
        const std::string l_supplyNet_Name = l_point_to_name[i->first];
        const std::string l_supplyPort_Name = l_point_to_name[i->second];
        if (this->m_sourceMap.find(l_supplyNet_Name) == this->m_sourceMap.end())
        {
            this->m_sourceMap[l_supplyNet_Name] = l_supplyNet_Name;
            this->m_endNetMap[l_supplyNet_Name].clear();
            this->m_endPortMap[l_supplyNet_Name].clear();
            this->m_endNetMap[l_supplyNet_Name].insert(l_supplyNet_Name);
            this->m_supplyList.insert(l_supplyNet_Name);
        }
        // 如果这是一个out的port，一定是更加里面的文件。因此直接更新
        if (i->second->isOutput())
        {

            this->m_sourceMap[l_supplyPort_Name] = l_supplyNet_Name;
            // 首先，在最终结果中加入这一个端口
            this->m_supplyList.insert(l_supplyPort_Name);
            auto X = this->m_endNetMap[this->m_sourceMap[l_supplyPort_Name]];
            // 然后，endNet中逐个插入链接的Net数量。并修改sourceMap的指向
            for (auto j = X.begin(); j != X.end(); j++)
            {
                this->m_sourceMap[*j] = l_supplyPort_Name;
                this->m_endNetMap[l_supplyPort_Name].insert(*j);
            }
            X = this->m_endPortMap[this->m_sourceMap[l_supplyPort_Name]];
            // endPort中逐个插入连接的port。并修改sourcemap的指向
            for (auto j = X.begin(); j != X.end(); j++)
            {
                this->m_sourceMap[*j] = l_supplyPort_Name;
                this->m_endPortMap[l_supplyPort_Name].insert(*j);
            }
            // 清理原来source supply端口的endNet、endPort和List
            this->m_endNetMap[this->m_sourceMap[l_supplyPort_Name]].clear();
            this->m_endPortMap[this->m_sourceMap[l_supplyPort_Name]].clear();
            this->m_supplyList.erase(this->m_supplyList.find(this->m_sourceMap[l_supplyPort_Name]));
            // 最后让新端口在sourceMap中指向自己
            this->m_sourceMap[l_supplyPort_Name] = l_supplyPort_Name;
        }
        this->m_sourceMap[l_supplyPort_Name] = this->m_sourceMap[l_supplyNet_Name];
        this->m_endPortMap[this->m_sourceMap[l_supplyNet_Name]].insert(l_supplyPort_Name);
    }

    for (auto i = sourcePortMap.begin(); i != sourcePortMap.end(); i++)
    {
        const std::string l_supplyPort_Name = i->first;
        auto T = this->m_sourceMap.find(l_supplyPort_Name);
        if (T != this->m_sourceMap.end())
            continue;
        this->m_sourceMap[l_supplyPort_Name] = l_supplyPort_Name;
        this->m_endNetMap[l_supplyPort_Name].clear();
        this->m_endPortMap[l_supplyPort_Name].insert(l_supplyPort_Name);
        this->m_supplyList.insert(l_supplyPort_Name);
    }

    l_point_to_name.clear();
    sourcePortMap.clear();
    l_unused_Conn.clear();
}

void NetworkAnalysis::DFSScopeAnaly()
{
    PwObjMgr *manager = PwObjMgr::getInstance();
    unionPst *unionP = unionPst::getInstance();

    this->networkAnaly();
    this->gnd_and_unused_check();
    unionP->mergePst(manager->getCurrentScope());
    // 访问所有子Scope
    for (auto i = manager->getCurrentScope()->beginChildScopes(); i != manager->getCurrentScope()->endChildScopes(); ++i)
    {
        PwScope *current = manager->getCurrentScope();
        manager->setCurrentScope(*i);
        this->DFSScopeAnaly();
        manager->setCurrentScope(current);
    }
}

void NetworkAnalysis::buildNetwork()
{
    PwObjMgr *manager = PwObjMgr::getInstance();
    PwScope *top = manager->getTopScope();
    this->m_outputfile.open("terminal.out", std::ios::out);
    manager->setCurrentScope(top);
    this->DFSScopeAnaly();
    unionPst *unionP = unionPst::getInstance();
    unionP->outputPst();
}

void NetworkAnalysis::outputNetwork()
{
    PwObjMgr *manager = PwObjMgr::getInstance();
    PwScope *top = manager->getTopScope();

    std::ofstream outfile;
    outfile.open("source.csv", std::ios::out);

    for (auto i = this->m_supplyList.begin(); i != this->m_supplyList.end(); i++)
    {
        const std::string l_supplyPort_Name = *i;

        outfile << "Supply_Source " << l_supplyPort_Name << std::endl;

        outfile << "Supply_Net ";
        std::unordered_set<std::string> *temp = &m_endNetMap[l_supplyPort_Name];
        for (auto j = temp->begin(); j != temp->end(); j++)
        {
            outfile << *j << " ";
        }
        outfile << std::endl;
        outfile << "Supply_Port ";
        temp = &m_endPortMap[l_supplyPort_Name];
        for (auto j = temp->begin(); j != temp->end(); j++)
        {
            outfile << *j << " ";
        }
        outfile << std::endl;
    }
    outfile.close();
}

void NetworkAnalysis::gnd_and_unused_check()
{
    PwObjMgr *manager = PwObjMgr::getInstance();
    PwScope *current = manager->getCurrentScope();

    for (auto pd = current->beginPowerDomains(); pd != current->endPowerDomains(); pd++)
    {
        bool haveGnd = 1;
        PwSupplyNet *gndNet = (*pd)->getGroundNet(); //获取地线
        if (gndNet == nullptr)
            haveGnd = 0;

        PwSupplyPort *gndPort;
        std::string gndPortName;
        if (haveGnd)
        {
            gndPort = gndNet->getDriver()->getSupplyPort(); //获取地线的供电端口
            gndPortName = gndPort->getName();
        }

        //创建定义的供电端口port_state_map
        std::unordered_map<std::string, std::unordered_set<std::string>> port_state_map;
        std::unordered_set<std::string> NonZeroStates;
        std::string portName;
        for (auto pt = current->beginSupplyPorts(); pt != current->endSupplyPorts(); pt++)
        {
            portName = (*pt)->getName();
            for (auto st = (*pt)->beginStates(); st != (*pt)->endStates(); st++)
            {
                port_state_map[portName].insert((*st)->getName());
            }
        }

        for (auto pst = current->beginPsts(); pst != current->endPsts(); pst++)
        {
            std::unordered_map<std::string, std::unordered_set<std::string>> pst_state_map;
            std::unordered_set<std::string> headerSet;

            for (auto hd = (*pst)->beginHeaders(); hd != (*pst)->endHeaders(); hd++)
            {
                headerSet.insert((*hd)->getName());
            }

            //创建pst_state_map,其中key全部都是port名而没有net名
            int xx = 0, yy = 0;
            std::string st2_getName;
            std::unordered_map<int, std::string> st2_SupplyHierName;
            for (auto stp = (*pst)->beginPstStates(); stp != (*pst)->endPstStates(); stp++) // stp是pst的每一个state
            {
                yy = 0;
                for (auto st2 = (*stp)->beginStates(); st2 != (*stp)->endStates(); st2++)
                {
                    st2_getName = (*st2)->getName();
                    if (xx == 0)
                    {
                        st2_SupplyHierName[yy] = (*st2)->getOwnerSupply()->getName();
                    }
                    if (haveGnd && st2_SupplyHierName[yy] == gndPortName)
                    {
                        if ((*st2)->getValue() != 0)
                            NonZeroStates.insert(st2_getName);
                    }
                    pst_state_map[st2_SupplyHierName[yy]].insert(st2_getName); // pst_state_map的Index只有port名没有net名
                    yy++;
                }
                xx++;
                // printf("\n");
            }
            st2_SupplyHierName.clear();

            //输出未用到状态
            std::string hdout;
            bool pstHavePort = 0;
            for (auto kp = port_state_map.begin(); kp != port_state_map.end(); kp++)
            {
                std::string k_p = (*kp).first;
                if (headerSet.find(k_p) != headerSet.end()) // header是ports
                {
                    hdout = current->findSupplyPort((k_p))->getHierName();
                    pstHavePort = 1;
                }
                else // header是net
                {
                    for (auto hd = headerSet.begin(); hd != headerSet.end(); hd++)
                    {
                        if (current->findSupplyNet((*hd))->getDriver()->getSupplyPort()->getName() == (k_p))
                        {
                            hdout = current->findSupplyNet((*hd))->getHierName();
                            pstHavePort = 1;
                            break;
                        }
                    }
                }
                //如果pst里有之前定义的port
                if (pstHavePort)
                {
                    for (auto stpt = port_state_map[k_p].begin(); stpt != port_state_map[k_p].end(); stpt++)
                    {
                        if (pst_state_map[k_p].find(*stpt) == pst_state_map[k_p].end())
                        {
                            this->m_outputfile << "State \"" << (*stpt) << "\" defined for \""
                                               << hdout << "\" is unused in PST!" << std::endl;
                        }
                    }
                }
                pstHavePort = false;
            }

            //输出gnd非零状态
            if (!NonZeroStates.empty())
            {
                this->m_outputfile << "Non-Zero State ";
                for (auto nzs = NonZeroStates.begin(); nzs != NonZeroStates.end(); nzs++)
                {
                    this->m_outputfile << "\"" << (*nzs) << "\" ";
                }
                this->m_outputfile << "applied on ground net "
                                   << "\"" << gndNet->getHierName() << "\"." << std::endl;
            }
            headerSet.clear();
            pst_state_map.clear();
        }
        port_state_map.clear();
        NonZeroStates.clear();
    }
}

NetworkAnalysis *NetworkAnalysis::m_inst = nullptr;
unionPst *unionPst::m_inst = nullptr;
