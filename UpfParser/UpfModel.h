
#ifndef PARSER_UPFMODEL_H
#define PARSER_UPFMODEL_H

#include "TclCommand.h"
#include <vector>
#include <unordered_map>

class PwScope;
class PwPowerDomain;
class PwSupplyPort;
class PwSupplyNet;
class PwSupplyConn;
class PwPst;
class PwSupplyState;
class PwPstState;

class PwObj {
public:
    PwObj(const std::string& name, PwScope* scope, UpfCommand* cmd) : m_name(name), m_scope(scope), m_command(cmd) {
    }
    virtual ~PwObj() {}
    virtual bool isScope() {
        return false;
    }
    virtual bool isPowerDomain() {
        return false;
    }
    virtual bool isPst() {
        return false;
    }
    virtual bool isSupplyNet() {
        return false;
    }
    virtual bool isSupplyNetwork() {
        return false;
    }
    virtual bool isSupplyPort() {
        return false;
    }
    virtual bool isSupplyState() {
        return false;
    }
    virtual bool isSupplyConn() {
        return false;
    }
    virtual bool isPstState() {
        return false;
    }
    std::string getName() const {
        return m_name;
    };
    virtual std::string getHierName() const = 0;
    PwScope*    getCreationScope() {
        return m_scope;
    }
    UpfCommand* getCreationCommand() {
        return m_command;
    }
protected:
    std::string m_name;
    PwScope* m_scope;
    UpfCommand* m_command;
};

class PwPowerDomain : public PwObj { // 主要电源class类
public:
    PwPowerDomain(PwScope* scope, UpfCommand* cmd, const std::string& name);
    ~PwPowerDomain() {}
    virtual bool isPowerDomain() {
        return true;
    }
    virtual std::string getHierName() const;
    void setPowerNet(PwSupplyNet* power) {
        m_power = power;
    }
    void setGroundNet(PwSupplyNet* ground) {
        m_ground = ground;
    }
    PwSupplyNet* getPowerNet() const {
        return m_power;
    }
    PwSupplyNet* getGroundNet() const {
        return m_ground;
    }
private:
    std::string  m_name;
    PwSupplyNet* m_power;
    PwSupplyNet* m_ground;
};

class PwSupplyConn : public PwObj { // 
public:
    PwSupplyConn(PwScope* scope, UpfCommand* cmd, PwSupplyNet* sn, PwSupplyPort* sp);
    ~PwSupplyConn() {}
    bool         isSupplyConn() const {
        return true;
    }
    virtual std::string getHierName() const {
        return "";
    }
    PwSupplyNet* getSupplyNet() const {
        return m_supplyNet;
    }
    PwSupplyPort* getSupplyPort() const {
        return m_supplyPort;
    }
private:
    PwSupplyNet* m_supplyNet;
    PwSupplyPort* m_supplyPort;
};

class PwSupplyNetwork : public PwObj { // 供电网络class类
public:
    PwSupplyNetwork(PwScope* scope, UpfCommand* cmd, const std::string& name);
    virtual ~PwSupplyNetwork() {};
    virtual bool isSupplyNetwork() {
        return true;
    }
    virtual std::string getHierName() const;
};

class PwSupplyNet : public PwSupplyNetwork {
public:
    PwSupplyNet(PwScope* scope, UpfCommand* cmd, const std::string& name);
    virtual ~PwSupplyNet() {}
    virtual bool isSupplyNet() {
        return true;
    }
    void setDriver(PwSupplyConn* driver);
    void addReceiver(PwSupplyConn* receiver) {
        m_receivers.push_back(receiver);
    }
    PwSupplyConn* getDriver() const {
        return m_driver;
    }
    std::vector<PwSupplyConn*>::iterator beginReceivers() {
        return m_receivers.begin();
    }
    std::vector<PwSupplyConn*>::iterator endReceivers() {
        return m_receivers.end();
    }
private:
    std::vector<PwSupplyConn*> m_receivers;
    PwSupplyConn* m_driver;
};

namespace PortDir {
    enum Enum {
        PORT_IN,
        PORT_OUT,
        NONE
    };
}

class PwSupplyState : public PwObj {
public:
    PwSupplyState(PwSupplyPort* sp, UpfCommand* cmd, const std::string& name);
    ~PwSupplyState() {}
    virtual std::string getHierName() const;
    virtual bool isSupplyState() {
        return true;
    }
    PwSupplyPort* getOwnerSupply() const {
        return m_owner;
    }
    void          setValue(float value) {
        m_value = value;
    }
    float         getValue() const {
        return m_value;
    }
private:
    std::string m_name;
    PwSupplyPort* m_owner;
    float m_value;
};

class PwSupplyPort : public PwSupplyNetwork {
public:
    typedef enum {
        PORT_IN,
        PORT_OUT,
        NON
    } PortDir;

    PwSupplyPort(PwScope* scope, UpfCommand* cmd, const std::string& name);
    virtual ~PwSupplyPort() {}
    virtual bool    isSupplyPort() {
        return true;
    }
    void           addSupplyState(PwSupplyState* ss);
    PwSupplyState* findSupplyState(const std::string& name);

    std::vector<PwSupplyState*>::iterator beginStates() {
        return m_states.begin();
    }
    std::vector<PwSupplyState*>::iterator endStates() {
        return m_states.end();
    }
    void    setLoConn(PwSupplyConn* lc) {
        m_loConn = lc;
    }
    PwSupplyConn*  getLoConn() const {
        return m_loConn;
    }
    void     setHiConn(PwSupplyConn* hi) {
        m_hiConn = hi;
    }
    PwSupplyConn*  getHiConn() const {
        return m_hiConn;
    }
    bool  isInput() const {
        return m_dir == PORT_IN;
    }
    bool  isOutput() const {
        return m_dir == PORT_OUT;
    }
    void  setAsOutputPort() {
        m_dir = PORT_OUT;
    }
private:
    std::string m_name;
    std::vector<PwSupplyState*> m_states;
    PortDir m_dir;
    PwSupplyConn* m_loConn;
    PwSupplyConn* m_hiConn;
};

class PwPstState : public PwObj {
public:
    PwPstState(PwPst* pst, UpfCommand* cmd, const std::string& name, std::vector<PwSupplyState*>& states);
    ~PwPstState() {}
    virtual std::string getHierName() const;
    virtual bool isPstState() {
        return true;
    }
    std::vector<PwSupplyState*>::iterator beginStates() {
        return m_states.begin();
    }
    std::vector<PwSupplyState*>::iterator endStates() {
        return m_states.end();
    }
    std::vector<PwSupplyState*> *getStates(){
        return &m_states;
    }
    PwPst*  getPst() const {
        return m_pst;
    }
private:
    PwPst* m_pst;
    std::vector<PwSupplyState*> m_states; // 这是每一行中的一个
};

class PwPst : public PwObj {
public:
    PwPst(PwScope* scope, UpfCommand* cmd, const std::string& name);
    ~PwPst() {}
    bool isPst() {
        return true;
    }
    virtual std::string getHierName() const;

    void addPstHeader(PwSupplyNetwork* sn);
    void addPstState(PwPstState* ps){
        m_states.push_back(ps);
    }
    PwPstState* findPstState(const std::string& name);

    std::vector<PwSupplyNetwork*>::iterator beginHeaders() {
        return m_header.begin();
    }
    std::vector<PwSupplyNetwork*>::iterator endHeaders() {
        return m_header.end();
    }
    std::vector<PwPstState*>::iterator beginPstStates() {
        return m_states.begin();
    }
    std::vector<PwPstState*>::iterator endPstStates() {
        return m_states.end();
    }
    int getPstHeaderSize() const {
        return m_header.size();
    }

private:
    std::vector<PwSupplyNetwork*> m_header;
    std::vector<PwPstState*> m_states; // 这个存放的是一行
};

class PwScope : public PwObj {
public:
    PwScope(PwScope* scope, UpfCommand* cmd, const std::string& name);
    virtual ~PwScope();
    bool isTopScope();
    PwPowerDomain* createPowerDomain(const std::string& name, UpfCommand* cmd);
    PwSupplyNet*   createSupplyNet(const std::string& name, UpfCommand* cmd);
    PwSupplyPort*  createSupplyPort(const std::string& name, UpfCommand* cmd);
    PwPst*         createPst(const std::string& name, UpfCommand* cmd);
    PwSupplyConn*  createSupplyConn(UpfCommand* cmd, PwSupplyNet* sn, PwSupplyPort* sp);
    PwSupplyState* createSupplyState(const std::string& name, UpfCommand* cmd, PwSupplyPort* sp);
    PwPstState*    createPstState(const std::string& name, UpfCommand* cmd, PwPst* pst,
                                  std::vector<PwSupplyState*>& states);
    PwScope*       createOrGetChildScope(const std::string& name, UpfCommand* cmd);

    PwPowerDomain* findPowerDomain(const std::string& name); // 同下
    PwSupplyNet*   findSupplyNet(const std::string& name); // 同下
    PwSupplyPort*  findSupplyPort(const std::string& name); // 寻找指定名称的PwSupplyPort类
    PwScope*       findChildScope(const std::string& name); // 寻找指定名称的PwScope类
    PwPst*         findPst(const std::string& name);

    std::vector<PwScope*>::iterator beginChildScopes() {
        return m_childs.begin();
    }
    std::vector<PwScope*>::iterator endChildScopes() {
        return m_childs.end();
    }
    std::vector<PwPowerDomain*>::iterator beginPowerDomains() {
        return m_pds.begin();
    }
    std::vector<PwPowerDomain*>::iterator endPowerDomains() {
        return m_pds.end();
    }
    std::vector<PwSupplyPort*>::iterator beginSupplyPorts() {
        return m_sps.begin();
    }
    std::vector<PwSupplyPort*>::iterator endSupplyPorts() {
        return m_sps.end();
    }
    std::vector<PwSupplyNet*>::iterator beginSupplyNets() {
        return m_sns.begin();
    }
    std::vector<PwSupplyNet*>::iterator endSupplyNets() {
        return m_sns.end();
    }
    std::vector<PwPst*>::iterator beginPsts() {
        return m_psts.begin();
    }
    std::vector<PwPst*>::iterator endPsts() {
        return m_psts.end();
    }

    std::vector<PwSupplyConn*>::iterator beginSupplyConn(){
        return m_supplyConns.begin();
    }
    std::vector<PwSupplyConn*>::iterator endSupplyConn(){
        return m_supplyConns.end();
    }

    virtual std::string getHierName() const;
    void    print();
private:
    std::vector<PwScope*>       m_childs;
    std::vector<PwPowerDomain*> m_pds;
    std::vector<PwSupplyPort*>  m_sps;
    std::vector<PwSupplyNet*>   m_sns;
    std::vector<PwPst*>         m_psts;
    std::vector<PwSupplyState*> m_states;
    std::vector<PwSupplyConn*>  m_supplyConns;
    std::vector<PwPstState*>    m_pstStates; // 这是一整行
};


class PwObjMgr {
public:
    static PwObjMgr* getInstance();
    PwScope*         getTopScope() const {
        return m_topScope;
    }
    PwScope*         getCurrentScope();
    void             setCurrentScope(PwScope *ps) {
        m_currentScope = ps;
    }
    void             createTopScope(const std::string& name, UpfCommand* cmd);
    void             clear();
    void             print();
private:
    PwObjMgr();
    ~PwObjMgr() {}
    static PwObjMgr* m_inst;
    PwScope *m_currentScope;
    PwScope *m_topScope;
};

#endif //PARSER_UPFMODEL_H
