#ifndef PARSER_TCLCOMMAND_H
#define PARSER_TCLCOMMAND_H
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <tcl.h>


namespace UpfCommandType {
    enum Enum {
        SET_DESIGN_TOP,
        SET_SCOPE,
        CREATE_POWER_DOMAIN,
        CREATE_SUPPLY_NET,
        CREATE_SUPPLY_PORT,
        CONNECT_SUPPLY_NET,
        CREATE_PST,
        ADD_PORT_STATE,
        ADD_PST_STATE,
        SET_DOMAIN_SUPPLY_NET,
        COMMAND_LAST
    };
}

class UpfCommand {
public:
    explicit UpfCommand(UpfCommandType::Enum type) : m_cmd_type(type) {
    }
    ~UpfCommand() {
    }
    void setLocation(const std::string& location) {
        m_location = location;
    }
    UpfCommandType::Enum getCmdType() const {
        return m_cmd_type;
    }
    std::string getLocation() const {
        return m_location;
    }
private:
    UpfCommand(const UpfCommand&) = delete;  // 这个的意思是禁用拷贝构造函数
    UpfCommand& operator= (const UpfCommand&) = delete; // 同上
    UpfCommandType::Enum m_cmd_type;
    std::string m_location;
};

class UpfCommandMgr {
public:
    static UpfCommandMgr* getInstance(); // 创建一个新的UpfCommandMgr管理器，是指针类型的
    bool        readUpfFile(const std::string& fileName);
    UpfCommand* createUpfCommand(UpfCommandType::Enum type);
    void        clear();
    static int  set_scope_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  create_power_domain_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  create_supply_net_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  create_supply_port_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  create_pst_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  connect_supply_net_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  add_port_state_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  add_pst_state_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  set_design_top_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
    static int  set_domain_supply_net_cmd(ClientData clientData, Tcl_Interp* interp, int argc, Tcl_Obj* const *argv);
private:
    void initialize();
    UpfCommandMgr();
    ~UpfCommandMgr() {}
    static UpfCommandMgr* m_mgr;
    Tcl_Interp* m_interp; // TCL命令解释器
    std::vector<UpfCommand*> m_commands;
};

#endif //PARSER_TCLCOMMAND_H
