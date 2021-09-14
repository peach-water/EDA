#include "TclCommand.h"
#include "UpfModel.h"
#include "Utility.h"
#include <iostream>

UpfCommandMgr* UpfCommandMgr::m_mgr=nullptr;

UpfCommandMgr * UpfCommandMgr::getInstance() {
    if (m_mgr == nullptr) {
        m_mgr = new UpfCommandMgr;
    }
    return m_mgr;
}

UpfCommandMgr::UpfCommandMgr() {
    initialize();
}

void UpfCommandMgr::clear() {
    for (auto cmd : m_commands) {
        delete cmd;
    }
    m_commands.clear();
}

UpfCommand* UpfCommandMgr::createUpfCommand(UpfCommandType::Enum type) {
    UpfCommand* cmd = new UpfCommand(type);
    m_commands.push_back(cmd);
    return cmd;
}

void UpfCommandMgr::initialize() {
    if (!m_interp) {
        m_interp = Tcl_CreateInterp();
    }
    Tcl_CreateObjCommand(m_interp,"set_design_top",set_design_top_cmd,nullptr, nullptr);
    Tcl_CreateObjCommand(m_interp,"set_scope",set_scope_cmd,nullptr, nullptr);
    Tcl_CreateObjCommand(m_interp,"create_power_domain",create_power_domain_cmd,nullptr,nullptr);
    Tcl_CreateObjCommand(m_interp,"set_domain_supply_net",set_domain_supply_net_cmd,nullptr,nullptr);
    Tcl_CreateObjCommand(m_interp,"create_supply_port",create_supply_port_cmd,nullptr,nullptr);
    Tcl_CreateObjCommand(m_interp,"create_supply_net",create_supply_net_cmd,nullptr,nullptr);
    Tcl_CreateObjCommand(m_interp,"create_pst",create_pst_cmd,nullptr,nullptr);
    Tcl_CreateObjCommand(m_interp,"add_port_state",add_port_state_cmd,nullptr,nullptr);
    Tcl_CreateObjCommand(m_interp,"connect_supply_net",connect_supply_net_cmd,nullptr,nullptr);
    Tcl_CreateObjCommand(m_interp,"add_pst_state",add_pst_state_cmd,nullptr,nullptr);
}

bool UpfCommandMgr::readUpfFile(const std::string &fileName) {
    if (fileName.empty()) {
        return false;
    }
    if (m_interp) {
        std::string cmdName = "source ";
        cmdName += fileName;
        if (Tcl_Eval(m_interp, cmdName.c_str())) {
            return true;
        }
    }
    return false;
}

static PwScope* getPwScopeByHierName(PwScope* cScope, const std::string& scopeName) {
    if (!cScope) {
        return nullptr;
    }
    size_t pos = scopeName.find("/");
    if (pos != std::string::npos) {
        std::string subName = scopeName.substr(0, pos);
        std::string resName = scopeName.substr(pos+1);
        PwScope* scope = cScope->findChildScope(subName);
        if (scope) {
            return getPwScopeByHierName(scope, resName);
        } else {
            return nullptr;
        }
    } else {
        return cScope->findChildScope(scopeName);
    }
}

int UpfCommandMgr::set_scope_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // set_scope scope
    PwScope* cScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::SET_SCOPE);
    std::string scopeName = Tcl_GetString(argv[1]);
    std::list<std::string> scopeList;
    splitStringIntoTokens(scopeName, scopeList, "/");
    while (!scopeList.empty()) {
        std::string scope = scopeList.front();
        if (scope == "..") {
            if (cScope->isTopScope()) {
                std::cout << "Error: Cannot create the scope!" << std::endl;
                return TCL_ERROR;
            } else {
                cScope = cScope->getCreationScope();
            }
        } else if (scope == ".") {

        } else {
            PwScope* nScope = cScope->findChildScope(scope);
            if (!nScope) {
                nScope = cScope->createOrGetChildScope(scope, command);
            }
            cScope = nScope;
        }
        scopeList.pop_front();
    }
    PwObjMgr::getInstance()->setCurrentScope(cScope);
    return TCL_OK;
}

int UpfCommandMgr::create_power_domain_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // create_power_domain PD_TOP -elements {...}
    PwScope* cScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::CREATE_POWER_DOMAIN);
    std::string pdName = Tcl_GetString(argv[1]);
    PwPowerDomain* pd = cScope->findPowerDomain(pdName);
    if (pd) {
        std::cout << "Error: Cannot create power domain with name: " << pdName << std::endl;
        return TCL_ERROR;
    }
    pd = cScope->createPowerDomain(pdName, command);
    return TCL_OK;
    //TODO: add elemenets;
}

int UpfCommandMgr::create_pst_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // create_pst pst -supplies {...}
    if (argc < 4) {
        return TCL_ERROR;
    }
    std::string pstName = Tcl_GetString(argv[1]);
    std::string pstHeader = Tcl_GetString(argv[3]);
    if (pstHeader.empty()) {
        std::cout << "Error: Please add -supplies option!" << std::endl;
        return TCL_ERROR;
    }
    PwScope* currentScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::CREATE_PST);

    if (currentScope->findPst(pstName)) {
        std::cout << "Error: Pst with name: "<< pstName << " has been defined before!" << std::endl;
        return TCL_ERROR;
    }
    PwPst* pst = currentScope->createPst(pstName, command);
    std::list<std::string> supplyList;
    trimSpace(pstHeader);
    splitStringIntoTokens(pstHeader, supplyList, " ");
    for (auto supplyName : supplyList) {
        PwScope* scope = currentScope;
        if (supplyName.find("/") != std::string::npos) {
            size_t pos = supplyName.find_last_of("/");
            std::string substr = supplyName.substr(0, pos);
            scope = getPwScopeByHierName(scope, substr);
            supplyName = supplyName.substr(pos+1);
        }
        if (!scope) {
            continue;
        }
        PwSupplyNetwork* snw = scope->findSupplyPort(supplyName);
        if (!snw) {
            snw = scope->findSupplyNet(supplyName);
        }
        if (!snw) {
            std::cout << "Error: Supply with name: " << supplyName <<
            " used in Pst cannot be found in scope: " << currentScope->getHierName() << std::endl;
            continue;
        }
        pst->addPstHeader(snw);
    }
    if (pst->getPstHeaderSize() == 0) {
        std::cout << "Error: The supply list used in Pst is empty!" << std::endl;
        return TCL_ERROR;
    }
    return TCL_OK;
}

int UpfCommandMgr::create_supply_net_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // create_supply_net VDD
    PwScope* cScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::CREATE_SUPPLY_NET);
    std::string netName = Tcl_GetString(argv[1]);
    if (cScope->findSupplyNet(netName)) {
        std::cout << "Error: Supply Net with name " << netName << " has been defined before!" << std::endl;
        return TCL_ERROR;
    }
    cScope->createSupplyNet(netName, command);
    return TCL_OK;
}

int UpfCommandMgr::create_supply_port_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // create_supply_port VDD -direction in
 
    PwScope* cScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::CREATE_SUPPLY_PORT);
    std::string portName = Tcl_GetString(argv[1]);
    if (cScope->findSupplyPort(portName)) {
        std::cout << "Error: Supply Port with name " << portName << " has been defined before!" << std::endl;
        return TCL_ERROR;
    }
    PwSupplyPort* sp = cScope->createSupplyPort(portName, command);
    if (argc > 2) {
        std::string arg2 = Tcl_GetString(argv[2]);
        if (arg2 == "-direction") {
            std::string arg3 = Tcl_GetString(argv[3]);
            if (arg3 == "out") {
                sp->setAsOutputPort();
            }
        }
    }
    return TCL_OK;
}

int UpfCommandMgr::connect_supply_net_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // connect_supply_net VDD -ports {VDD SUB/VSS}
    PwScope* cScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::CONNECT_SUPPLY_NET);
    std::string netName = Tcl_GetString(argv[1]);
    PwSupplyNet* sn = cScope->findSupplyNet(netName);
    if (!sn) {
        std::cout << "Error: Cannot find the supply net with name: " << netName <<
        " for connect_supply_net command! " << std::endl;
        return TCL_ERROR;
    }
    std::string portName = Tcl_GetString(argv[3]);
    std::list<std::string> portNameList;
    trimSpace(portName);
    splitStringIntoTokens(portName, portNameList, " ");
    if (!portNameList.empty()) {
        for (auto port : portNameList) {
            PwScope* scope = cScope;
            if (port.find("/") != std::string::npos) {
                size_t pos = port.find_last_of("/");
                std::string substr = port.substr(0, pos);
                scope = getPwScopeByHierName(scope, substr);
                port = port.substr(pos+1);
            }
            PwSupplyPort* sp = scope->findSupplyPort(port);
            if (sp) {
                cScope->createSupplyConn(command, sn, sp);
            } else {
                std::cout << "Warning: Cannot find the supply port with name: " << port << std::endl;
                continue;
            }
        }
    }
    return TCL_OK;
}

int UpfCommandMgr::set_design_top_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // set_design_top TOP

    std::string topScope = Tcl_GetString(argv[1]);;
    UpfCommand* cmd = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::SET_DESIGN_TOP);
    PwObjMgr::getInstance()->createTopScope(topScope, cmd);
    return TCL_OK;
}

int UpfCommandMgr::add_port_state_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // add_port_state VDD -state {ON 1.0}
    PwScope* cScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::ADD_PORT_STATE);
    std::string portName = Tcl_GetString(argv[1]);
    PwSupplyPort* sp = cScope->findSupplyPort(portName);
    if (!sp) {
        std::cout << "Error: Cannot find the port with name: " << portName;
        return TCL_ERROR;
    }
    bool status = true;
    for (unsigned i = 2; i != argc; ++i) {
        std::string argName = Tcl_GetString(argv[i]);
        if (argName == "-state") {
            std::string state = Tcl_GetString(argv[++i]);
            std::list<std::string> stateList;
            trimSpace(state);
            splitStringIntoTokens(state, stateList, " ");
            if (stateList.size() != 2) {
                status = false;
                std::cout << "Error: The argument number should be 2 of -state option!" << std::endl;
                continue;
            }
            std::string stateName = stateList.front();
            std::string stateValue = stateList.back();
            PwSupplyState* pss = sp->findSupplyState(stateName);
            if (pss) {
                status = false;
                std::cout << "Error: The port state with name: " << stateName
                << " has been defined before!" << std::endl;
                continue;
            } else {
                pss = cScope->createSupplyState(stateName, command, sp);
                float value = 0.0;
                if (stateValue != "off") {
                    value = std::atof(stateValue.c_str());
                }
                pss->setValue(value);
            }
        }
    }
    if (!status) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

int UpfCommandMgr::add_pst_state_cmd(ClientData clientData, Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {
    // add_pst_state S1 -pst pst -state {ON OFF HV LV}
    if (argc != 6) {
        std::cout << "Error: argument number error!" << std::endl;
        return TCL_ERROR;
    }
    PwScope* cScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::ADD_PST_STATE);
    std::string stateName = Tcl_GetString(argv[1]);
    std::string pstName;
    std::string supplyStates;
    for (unsigned i = 2; i != argc; ++i) {
        std::string arg = Tcl_GetString(argv[i]);
        if (arg == "-pst") {
            pstName = Tcl_GetString(argv[++i]);
        }
        if (arg == "-state") {
            supplyStates = Tcl_GetString(argv[++i]);
        }
    }
    PwPst* pst = cScope->findPst(pstName);
    if (!pst) {
        std::cout << "Error: Cannot find the pst with name: " << pst->getName() << std::endl;
        return TCL_ERROR;
    }
    PwPstState* pps = pst->findPstState(stateName);
    if (pps) {
        std::cout << "Error: The pst state with name: " << stateName << " has been used before!" << std::endl;
        return TCL_ERROR;
    }
    std::list<std::string> stateList;
    trimSpace(supplyStates);
    splitStringIntoTokens(supplyStates, stateList, " ");
    if (stateList.size() != pst->getPstHeaderSize()) {
        std::cout << "Error: The size of supplies and states is mismatch, please check and fix it!" << std::endl;
    }
    std::vector<PwSupplyState*> ssVec;
    for (std::vector<PwSupplyNetwork*>::iterator iter = pst->beginHeaders(); iter != pst->endHeaders(); ++iter) {
        PwSupplyNetwork* psn = *iter;
        std::string supplyState = stateList.front();
        stateList.pop_front();
        PwSupplyState* pss = nullptr;
        if (psn->isSupplyPort()) {
            PwSupplyPort* sp = dynamic_cast<PwSupplyPort*>(psn);
            pss = sp->findSupplyState(supplyState);
        } else {
            PwSupplyNet* sn = dynamic_cast<PwSupplyNet*>(psn);
            PwSupplyConn* conn = sn->getDriver();
            if (conn) {
                PwSupplyPort* sp = conn->getSupplyPort();
                pss = sp->findSupplyState(supplyState);
            }
        }
        if (!pss) {
            std::cout << "Error: Cannot find supply state: " << supplyState <<
            " for supply object: " << psn->getName() << std::endl;
            return TCL_ERROR;
        } else {
            ssVec.push_back(pss);
        }
    }
    pps = cScope->createPstState(stateName, command, pst, ssVec);
    return TCL_OK;
}

int UpfCommandMgr::set_domain_supply_net_cmd(ClientData clientData,
                                             Tcl_Interp *interp, int argc, Tcl_Obj *const *argv) {

    // set_domain_supply_net PD_TOP -primary_power_net VDD -primary_ground_net VSS
    if (argc != 6) {
        std::cout << "Error: argument number error!" << std::endl;
        return TCL_ERROR;
    }
    PwScope* cScope = PwObjMgr::getInstance()->getCurrentScope();
    UpfCommand* command = UpfCommandMgr::getInstance()->createUpfCommand(UpfCommandType::SET_DOMAIN_SUPPLY_NET);

    std::string pdName = Tcl_GetString(argv[1]);
    PwPowerDomain* pd = cScope->findPowerDomain(pdName);
    if (!pd) {
        std::cout << "Error: Cannot find the power domain with name: " << pd->getName() << std::endl;
        return TCL_ERROR;
    }
    for (unsigned i = 2; i != argc; ++i) {
        std::string arg = Tcl_GetString(argv[i]);
        if (arg == "-primary_power_net") {
            std::string powerName = Tcl_GetString(argv[++i]);
            PwSupplyNet* psn = cScope->findSupplyNet(powerName);
            if (!psn) {
                std::cout << "Error: Cannot find the supply net with name: " << powerName << std::endl;
                return TCL_ERROR;
            }
            pd->setPowerNet(psn);
        }
        if (arg == "-primary_ground_net") {
            std::string groundName = Tcl_GetString(argv[++i]);
            PwSupplyNet* gsn = cScope->findSupplyNet(groundName);
            if (!gsn) {
                std::cout << "Error: Cannot find the supply net with name: " << groundName << std::endl;
                return TCL_ERROR;
            }
            pd->setGroundNet(gsn);
        }
    }
    return TCL_OK;
}
