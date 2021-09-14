
#include "UpfModel.h"
#include <iostream>

PwPowerDomain::PwPowerDomain(PwScope *scope, UpfCommand *cmd, const std::string  &name)
: PwObj(name, scope, cmd),m_ground(nullptr), m_power(nullptr)
{
}

std::string PwPowerDomain::getHierName() const {
    return (m_scope->isTopScope() && m_scope->getName() == "/")? getName() : m_scope->getHierName() + "/" + getName();
}


PwSupplyConn::PwSupplyConn(PwScope *scope, UpfCommand *cmd, PwSupplyNet *sn, PwSupplyPort *sp)
: PwObj("", scope, cmd), m_supplyNet(sn), m_supplyPort(sp)
{
}

PwSupplyNetwork::PwSupplyNetwork(PwScope *scope, UpfCommand *cmd, const std::string &name)
: PwObj(name, scope, cmd)
{
}

std::string PwSupplyNetwork::getHierName() const {
    return (m_scope->isTopScope() && m_scope->getName() == "/")? getName() : m_scope->getHierName() + "/" + getName();
}

PwSupplyNet::PwSupplyNet(PwScope *scope, UpfCommand *cmd, const std::string &name)
: PwSupplyNetwork(scope, cmd, name), m_driver(nullptr)
{
}

void PwSupplyNet::setDriver(PwSupplyConn *driver) {
    if (!driver) {
        return;
    }
    if (m_driver != nullptr) {
        std::cout << "Error: Multiple driver of supply net: " << getName() << std::endl;
        std::cout << "We cannot support multi-driven net. ";
        return;
    }
    m_driver = driver;
}

PwSupplyState::PwSupplyState(PwSupplyPort *sp, UpfCommand *cmd, const std::string &name)
: PwObj(name, sp->getCreationScope(), cmd), m_value(0.0), m_owner(sp)
{
}

std::string PwSupplyState::getHierName() const {
    std::string path = m_owner->getHierName();
    path += ".";
    path += m_name;
    return path;
}

PwSupplyPort::PwSupplyPort(PwScope *scope, UpfCommand *cmd, const std::string &name)
: PwSupplyNetwork(scope, cmd, name), m_dir(PortDir::PORT_IN), m_hiConn(nullptr), m_loConn(nullptr)
{}

PwSupplyState* PwSupplyPort::findSupplyState(const std::string &name) {
    if (m_states.empty()) {
        return nullptr;
    }
    for (unsigned i = 0; i < m_states.size(); ++i) {
        PwSupplyState* ss = m_states[i];
        if (ss->getName() == name) {
            return ss;
        }
    }
    return nullptr;
}

void PwSupplyPort::addSupplyState(PwSupplyState *ss) {
    m_states.push_back(ss);
}

PwPstState::PwPstState(PwPst *pst, UpfCommand *cmd, const std::string &name, std::vector<PwSupplyState*>& states)
: PwObj(name, pst->getCreationScope(), cmd), m_pst(pst), m_states(states)
{
}

std::string PwPstState::getHierName() const {
    std::string path = m_pst->getHierName();
    path += ".";
    path += m_name;
    return path;
}

PwPst::PwPst(PwScope *scope, UpfCommand *cmd, const std::string &name)
: PwObj(name, scope, cmd)
{}

void PwPst::addPstHeader(PwSupplyNetwork *sn) {
    if (m_header.empty()) {
        m_header.push_back(sn);
        return;
    }
    for (int i = 0; i < m_header.size(); ++i) {
        PwSupplyNetwork* psn = m_header[i];
        if (psn == sn) {
            std::cout << "Error: Supply object named " << sn->getName() << " has been used in Pst!" << std::endl;
            return;
        }
    }
    m_header.push_back(sn);
}

PwPstState* PwPst::findPstState(const std::string &name) {
    if (m_states.empty()) {
        return nullptr;
    }
    for (unsigned i = 0; i < m_states.size(); ++i) {
        PwPstState* ps = m_states[i];
        if (ps->getName() == name) {
            return ps;
        }
    }
    return nullptr;
}

std::string PwPst::getHierName() const
{
    return (m_scope->isTopScope() && m_scope->getName() == "/")? getName() : m_scope->getHierName() + "/" + getName();
}

PwScope::PwScope(PwScope *scope, UpfCommand *cmd, const std::string &name)
: PwObj(name, scope, cmd)
{
}

PwScope::~PwScope() {
    for (auto pd : m_pds) {
        delete pd;
    }
    m_pds.clear();
    for (auto sn : m_sns) {
        delete sn;
    }
    m_sns.clear();
    for (auto sp : m_sps) {
        delete sp;
    }
    m_sps.clear();
    for (auto conn : m_supplyConns) {
        delete conn;
    }
    m_supplyConns.clear();
    for (auto state : m_states) {
        delete state;
    }
    m_states.clear();
    for (auto pst : m_psts) {
        delete pst;
    }
    m_psts.clear();
    for (auto pststate : m_pstStates) {
        delete pststate;
    }
    m_pstStates.clear();
    for (auto child : m_childs) {
        delete child;
    }
    m_childs.clear();
}

bool PwScope::isTopScope() {
    if (getCreationScope() == nullptr) {
        return true;
    }
    return false;
}

PwPowerDomain* PwScope::findPowerDomain(const std::string  &name) {
    if (m_pds.empty()) {
        return nullptr;
    }
    for (unsigned i = 0; i < m_pds.size(); ++i) {
        PwPowerDomain* pd = m_pds[i];
        if (pd->getName() == name) {
            return pd;
        }
    }
    return nullptr;
}

PwPowerDomain* PwScope::createPowerDomain(const std::string& name, UpfCommand* cmd) {
    PwPowerDomain* pd = findPowerDomain(name);
    if (pd != nullptr) {
        std::cout << "Error: Cannot create power domain with name: " << name << std::endl;
        return nullptr;
    }
    pd = new PwPowerDomain(this, cmd, name);
    m_pds.push_back(pd);
    return pd;
}

PwPst* PwScope::findPst(const std::string &name) {
    if (m_psts.empty()) {
        return nullptr;
    }
    for (int i = 0; i < m_psts.size(); ++i) {
        PwPst* pst = m_psts[i];
        if (pst->getName() == name) {
            return pst;
        }
    }
    return nullptr;
}

PwPst* PwScope::createPst(const std::string &name, UpfCommand *cmd) {
    PwPst* pst = findPst(name);
    if (pst != nullptr) {
        std::cout << "Error: Cannot create pst with name: " << name << std::endl;
        return nullptr;
    }
    pst = new PwPst(this, cmd, name);
    m_psts.push_back(pst);
    return pst;
}

PwSupplyPort* PwScope::findSupplyPort(const std::string &name) {
    if (m_sps.empty()) {
        return nullptr;
    }
    for (int i = 0; i < m_sps.size(); ++i) {
        PwSupplyPort* sp = m_sps[i];
        if (sp->getName() == name) {
            return sp;
        }
    }
    return nullptr;
}

PwSupplyPort* PwScope::createSupplyPort(const std::string &name, UpfCommand *cmd) {
    PwSupplyPort* sp = findSupplyPort(name);
    if (sp != nullptr) {
        std::cout << "Error: Cannot create supply port with name: " << name << std::endl;
        return nullptr;
    }
    sp = new PwSupplyPort(this, cmd, name);
    m_sps.push_back(sp);
    return sp;
}

PwSupplyNet* PwScope::findSupplyNet(const std::string &name) {
    if (m_sns.empty()) {
        return nullptr;
    }
    for (int i = 0; i < m_sns.size(); ++i) {
        PwSupplyNet* sn = m_sns[i];
        if (sn->getName() == name) {
            return sn;
        }
    }
    return nullptr;
}

PwSupplyNet* PwScope::createSupplyNet(const std::string &name, UpfCommand *cmd) {
    PwSupplyNet* sn = findSupplyNet(name);
    if (sn != nullptr) {
        std::cout << "Error: Cannot create supply net with name: " << name << std::endl;
        return nullptr;
    }
    sn = new PwSupplyNet(this, cmd, name);
    m_sns.push_back(sn);
    return sn;
}

PwScope* PwScope::findChildScope(const std::string &name) {
    // FIXME: for simple name;
    if (m_childs.empty()) {
        return nullptr;
    }
    for (int i = 0; i < m_childs.size(); ++i) {
        PwScope* childScope = m_childs[i];
        if (childScope->getName() == name) {
            return childScope;
        }
    }
    return nullptr;
}

PwScope* PwScope::createOrGetChildScope(const std::string &name, UpfCommand *cmd) {
    PwScope* ps = findChildScope(name);
    if (ps == nullptr) {
        ps = new PwScope(this, cmd, name);
    }
    m_childs.push_back(ps);
    return ps;
}

PwSupplyState* PwScope::createSupplyState(const std::string &name, UpfCommand *cmd, PwSupplyPort *sp) {
    if (!sp) {
        return nullptr;
    }
    if (sp->findSupplyState(name)) {
        std::cout << "Error: Cannot create supply state with name: "
                  << name << " for supply port: " << sp->getName() << std::endl;
        return nullptr;
    }
    PwSupplyState* state = new PwSupplyState(sp, cmd, name);
    m_states.push_back(state);
    sp->addSupplyState(state);
    return state;
}

PwPstState* PwScope::createPstState(const std::string &name, UpfCommand *cmd, PwPst *pst,
                                    std::vector<PwSupplyState*>& states) {
    if (!pst) {
        return nullptr;
    }
    if (pst->findPstState(name)) {
        std::cout << "Error: Cannot create pst state with name: "
                  << name << " for pst: " << pst->getName() << std::endl;
        return nullptr;
    }
    PwPstState* pss = new PwPstState(pst, cmd, name, states);
    m_pstStates.push_back(pss);
    pst->addPstState(pss);
    return pss;
}

static bool isChildScope(PwScope* ps1, PwScope* ps2) {
    // check ps2 is the child scope ps1 or not;
    if (!ps1 || !ps2) {
        return false;
    }
    PwScope* parent = ps2->getCreationScope();
    if (!parent) {
        return false;
    }
    while (parent != ps1) {
        parent = parent->getCreationScope();
    }
    if (parent) {
        return true;
    }
    return false;
}

PwSupplyConn* PwScope::createSupplyConn(UpfCommand *cmd, PwSupplyNet *sn, PwSupplyPort *sp) {
    if (!sn || !sp) {
        return nullptr;
    }
    PwSupplyConn* conn = new PwSupplyConn(this, cmd, sn, sp);
    PwScope* snscope = sn->getCreationScope();
    PwScope* spscope = sp->getCreationScope();
    if (snscope == spscope) {
        // Net and Port are created under same scope;
        if (sp->isInput()) {
            sn->setDriver(conn);
            sp->setLoConn(conn);
        } else if (sp->isOutput()) {
            sn->addReceiver(conn);
            sp->setHiConn(conn);
        }
    } else if (isChildScope(snscope, spscope)) {
        // Port is defined inside the scope of net;
        if (sp->isInput()) {
            sn->addReceiver(conn);
        } else if (sp->isOutput()) {
            sn->setDriver(conn);
        }
        sp->setHiConn(conn);
    } else if (isChildScope(spscope, snscope)) {
        // Net is defined inside the scope of port;
        if (sp->isInput()) {
            sn->setDriver(conn);
        } else if (sp->isOutput()) {
            sn->addReceiver(conn);
        }
        sp->setLoConn(conn);
    } else {
        // Net and port has no scope relationship
        // Will do more check later;
        std::cout << "Warning: the net connect to the port are cross over the power scope!" << std::endl;
    }
    m_supplyConns.push_back(conn);
    return conn;
}


std::string PwScope::getHierName() const {
    PwScope* parent = m_scope;
    if (!parent) {
        std::string name = getName();
        if (name == "/") {
            return "";
        } else {
            return name;
        }
    }
    std::string parentName = parent->getName();
    if (parent->isTopScope()) {
        std::string parentName = parent->getName();
        if (parentName == "/") {
            return getName();
        }
    }
    return parent->getHierName() + "/" + getName();
}

void PwScope::print() {
    std::cout << "Summary of Scope: " << getHierName() << std::endl;
    std::cout << "Power Domain: ";
    for (auto pd : m_pds) {
        std::cout << pd->getName() << " ";
    }
    std::cout << std::endl;
    std::cout << "Supply Net: ";
    for (auto sn: m_sns) {
        std::cout << sn->getName() << " ";
    }
    std::cout << std::endl;
    std::cout << "Supply Port: ";
    for (auto sp : m_sps) {
        std::cout << sp->getName() << " ";
    }
    std::cout << std::endl;
    std::cout << "Pst: ";
    for (auto pst : m_psts) {
        std::cout << pst->getName() << " ";
    }
    std::cout << std::endl;
    for (auto child : m_childs) {
        child->print();
    }
}

PwObjMgr::PwObjMgr() : m_topScope(nullptr), m_currentScope(nullptr) {
}

PwScope* PwObjMgr::getCurrentScope() {
    if ( !m_currentScope ) {
        createTopScope("/" , nullptr);
    }
    return m_currentScope;
}

PwObjMgr * PwObjMgr::getInstance() {
    if ( m_inst == nullptr ) {
        m_inst = new PwObjMgr();
    }
    return m_inst;
}

void PwObjMgr::createTopScope(const std::string &name, UpfCommand* cmd) {
    if (m_topScope) {
        std::cout << "Error: design top has been set!" << std::endl;
        return;
    }
    m_topScope = new PwScope(nullptr, cmd, name);
    setCurrentScope(m_topScope);
}

void PwObjMgr::clear() {
    delete m_topScope;
}

void PwObjMgr::print() {
    m_topScope->print();
}

PwObjMgr* PwObjMgr::m_inst = nullptr;
