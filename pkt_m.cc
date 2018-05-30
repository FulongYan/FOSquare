//
// Generated file, do not edit! Created by nedtool 4.6 from pkt.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "pkt_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

Register_Class(pkt);

pkt::pkt(const char *name, int kind) : ::cPacket(name,kind)
{
    this->src_Server_var = 0;
    this->dst_Server_var = 0;
    this->src_ToR_var = 0;
    this->dst_ToR_var = 0;
    this->index_var = 0;
    this->TotalHopCount_var = 0;
    this->HopCountNow_var = 0;
    for (unsigned int i=0; i<8; i++)
        this->Hop_var[i] = 0;
    this->PacketSize_var = 0;
    this->no_var = 0;
    this->m_var = 0;
    this->retransmission_var = 0;
    this->path_var = 0;
    this->TxIndex_var = 0;
    this->GroupToRIndex_var = 0;
    this->End2EndStartTime_var = 0;
    this->PacketNetworkStartTime_var = 0;
    this->PacketEnd2EndStartTime_var = 0;
    this->flag_intraToR_var = 0;
    this->flag_ToR2ToR_var = 0;
    this->optNum_var = 0;
    this->flag_last_var = 0;
    this->flag_app_var = 0;
    this->app_pkt_num_var = 0;
}

pkt::pkt(const pkt& other) : ::cPacket(other)
{
    copy(other);
}

pkt::~pkt()
{
}

pkt& pkt::operator=(const pkt& other)
{
    if (this==&other) return *this;
    ::cPacket::operator=(other);
    copy(other);
    return *this;
}

void pkt::copy(const pkt& other)
{
    this->src_Server_var = other.src_Server_var;
    this->dst_Server_var = other.dst_Server_var;
    this->src_ToR_var = other.src_ToR_var;
    this->dst_ToR_var = other.dst_ToR_var;
    this->index_var = other.index_var;
    this->TotalHopCount_var = other.TotalHopCount_var;
    this->HopCountNow_var = other.HopCountNow_var;
    for (unsigned int i=0; i<8; i++)
        this->Hop_var[i] = other.Hop_var[i];
    this->PacketSize_var = other.PacketSize_var;
    this->no_var = other.no_var;
    this->m_var = other.m_var;
    this->retransmission_var = other.retransmission_var;
    this->path_var = other.path_var;
    this->TxIndex_var = other.TxIndex_var;
    this->GroupToRIndex_var = other.GroupToRIndex_var;
    this->End2EndStartTime_var = other.End2EndStartTime_var;
    this->PacketNetworkStartTime_var = other.PacketNetworkStartTime_var;
    this->PacketEnd2EndStartTime_var = other.PacketEnd2EndStartTime_var;
    this->flag_intraToR_var = other.flag_intraToR_var;
    this->flag_ToR2ToR_var = other.flag_ToR2ToR_var;
    this->optNum_var = other.optNum_var;
    this->flag_last_var = other.flag_last_var;
    this->flag_app_var = other.flag_app_var;
    this->app_pkt_num_var = other.app_pkt_num_var;
}

void pkt::parsimPack(cCommBuffer *b)
{
    ::cPacket::parsimPack(b);
    doPacking(b,this->src_Server_var);
    doPacking(b,this->dst_Server_var);
    doPacking(b,this->src_ToR_var);
    doPacking(b,this->dst_ToR_var);
    doPacking(b,this->index_var);
    doPacking(b,this->TotalHopCount_var);
    doPacking(b,this->HopCountNow_var);
    doPacking(b,this->Hop_var,8);
    doPacking(b,this->PacketSize_var);
    doPacking(b,this->no_var);
    doPacking(b,this->m_var);
    doPacking(b,this->retransmission_var);
    doPacking(b,this->path_var);
    doPacking(b,this->TxIndex_var);
    doPacking(b,this->GroupToRIndex_var);
    doPacking(b,this->End2EndStartTime_var);
    doPacking(b,this->PacketNetworkStartTime_var);
    doPacking(b,this->PacketEnd2EndStartTime_var);
    doPacking(b,this->flag_intraToR_var);
    doPacking(b,this->flag_ToR2ToR_var);
    doPacking(b,this->optNum_var);
    doPacking(b,this->flag_last_var);
    doPacking(b,this->flag_app_var);
    doPacking(b,this->app_pkt_num_var);
}

void pkt::parsimUnpack(cCommBuffer *b)
{
    ::cPacket::parsimUnpack(b);
    doUnpacking(b,this->src_Server_var);
    doUnpacking(b,this->dst_Server_var);
    doUnpacking(b,this->src_ToR_var);
    doUnpacking(b,this->dst_ToR_var);
    doUnpacking(b,this->index_var);
    doUnpacking(b,this->TotalHopCount_var);
    doUnpacking(b,this->HopCountNow_var);
    doUnpacking(b,this->Hop_var,8);
    doUnpacking(b,this->PacketSize_var);
    doUnpacking(b,this->no_var);
    doUnpacking(b,this->m_var);
    doUnpacking(b,this->retransmission_var);
    doUnpacking(b,this->path_var);
    doUnpacking(b,this->TxIndex_var);
    doUnpacking(b,this->GroupToRIndex_var);
    doUnpacking(b,this->End2EndStartTime_var);
    doUnpacking(b,this->PacketNetworkStartTime_var);
    doUnpacking(b,this->PacketEnd2EndStartTime_var);
    doUnpacking(b,this->flag_intraToR_var);
    doUnpacking(b,this->flag_ToR2ToR_var);
    doUnpacking(b,this->optNum_var);
    doUnpacking(b,this->flag_last_var);
    doUnpacking(b,this->flag_app_var);
    doUnpacking(b,this->app_pkt_num_var);
}

int pkt::getSrc_Server() const
{
    return src_Server_var;
}

void pkt::setSrc_Server(int src_Server)
{
    this->src_Server_var = src_Server;
}

int pkt::getDst_Server() const
{
    return dst_Server_var;
}

void pkt::setDst_Server(int dst_Server)
{
    this->dst_Server_var = dst_Server;
}

int pkt::getSrc_ToR() const
{
    return src_ToR_var;
}

void pkt::setSrc_ToR(int src_ToR)
{
    this->src_ToR_var = src_ToR;
}

int pkt::getDst_ToR() const
{
    return dst_ToR_var;
}

void pkt::setDst_ToR(int dst_ToR)
{
    this->dst_ToR_var = dst_ToR;
}

int pkt::getIndex() const
{
    return index_var;
}

void pkt::setIndex(int index)
{
    this->index_var = index;
}

int pkt::getTotalHopCount() const
{
    return TotalHopCount_var;
}

void pkt::setTotalHopCount(int TotalHopCount)
{
    this->TotalHopCount_var = TotalHopCount;
}

int pkt::getHopCountNow() const
{
    return HopCountNow_var;
}

void pkt::setHopCountNow(int HopCountNow)
{
    this->HopCountNow_var = HopCountNow;
}

unsigned int pkt::getHopArraySize() const
{
    return 8;
}

int pkt::getHop(unsigned int k) const
{
    if (k>=8) throw cRuntimeError("Array of size 8 indexed by %lu", (unsigned long)k);
    return Hop_var[k];
}

void pkt::setHop(unsigned int k, int Hop)
{
    if (k>=8) throw cRuntimeError("Array of size 8 indexed by %lu", (unsigned long)k);
    this->Hop_var[k] = Hop;
}

int pkt::getPacketSize() const
{
    return PacketSize_var;
}

void pkt::setPacketSize(int PacketSize)
{
    this->PacketSize_var = PacketSize;
}

int pkt::getNo() const
{
    return no_var;
}

void pkt::setNo(int no)
{
    this->no_var = no;
}

int pkt::getM() const
{
    return m_var;
}

void pkt::setM(int m)
{
    this->m_var = m;
}

int pkt::getRetransmission() const
{
    return retransmission_var;
}

void pkt::setRetransmission(int retransmission)
{
    this->retransmission_var = retransmission;
}

int pkt::getPath() const
{
    return path_var;
}

void pkt::setPath(int path)
{
    this->path_var = path;
}

int pkt::getTxIndex() const
{
    return TxIndex_var;
}

void pkt::setTxIndex(int TxIndex)
{
    this->TxIndex_var = TxIndex;
}

int pkt::getGroupToRIndex() const
{
    return GroupToRIndex_var;
}

void pkt::setGroupToRIndex(int GroupToRIndex)
{
    this->GroupToRIndex_var = GroupToRIndex;
}

double pkt::getEnd2EndStartTime() const
{
    return End2EndStartTime_var;
}

void pkt::setEnd2EndStartTime(double End2EndStartTime)
{
    this->End2EndStartTime_var = End2EndStartTime;
}

simtime_t pkt::getPacketNetworkStartTime() const
{
    return PacketNetworkStartTime_var;
}

void pkt::setPacketNetworkStartTime(simtime_t PacketNetworkStartTime)
{
    this->PacketNetworkStartTime_var = PacketNetworkStartTime;
}

simtime_t pkt::getPacketEnd2EndStartTime() const
{
    return PacketEnd2EndStartTime_var;
}

void pkt::setPacketEnd2EndStartTime(simtime_t PacketEnd2EndStartTime)
{
    this->PacketEnd2EndStartTime_var = PacketEnd2EndStartTime;
}

int pkt::getFlag_intraToR() const
{
    return flag_intraToR_var;
}

void pkt::setFlag_intraToR(int flag_intraToR)
{
    this->flag_intraToR_var = flag_intraToR;
}

int pkt::getFlag_ToR2ToR() const
{
    return flag_ToR2ToR_var;
}

void pkt::setFlag_ToR2ToR(int flag_ToR2ToR)
{
    this->flag_ToR2ToR_var = flag_ToR2ToR;
}

int pkt::getOptNum() const
{
    return optNum_var;
}

void pkt::setOptNum(int optNum)
{
    this->optNum_var = optNum;
}

int pkt::getFlag_last() const
{
    return flag_last_var;
}

void pkt::setFlag_last(int flag_last)
{
    this->flag_last_var = flag_last;
}

int pkt::getFlag_app() const
{
    return flag_app_var;
}

void pkt::setFlag_app(int flag_app)
{
    this->flag_app_var = flag_app;
}

int pkt::getApp_pkt_num() const
{
    return app_pkt_num_var;
}

void pkt::setApp_pkt_num(int app_pkt_num)
{
    this->app_pkt_num_var = app_pkt_num;
}

class pktDescriptor : public cClassDescriptor
{
  public:
    pktDescriptor();
    virtual ~pktDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(pktDescriptor);

pktDescriptor::pktDescriptor() : cClassDescriptor("pkt", "cPacket")
{
}

pktDescriptor::~pktDescriptor()
{
}

bool pktDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<pkt *>(obj)!=NULL;
}

const char *pktDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int pktDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 24+basedesc->getFieldCount(object) : 24;
}

unsigned int pktDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<24) ? fieldTypeFlags[field] : 0;
}

const char *pktDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "src_Server",
        "dst_Server",
        "src_ToR",
        "dst_ToR",
        "index",
        "TotalHopCount",
        "HopCountNow",
        "Hop",
        "PacketSize",
        "no",
        "m",
        "retransmission",
        "path",
        "TxIndex",
        "GroupToRIndex",
        "End2EndStartTime",
        "PacketNetworkStartTime",
        "PacketEnd2EndStartTime",
        "flag_intraToR",
        "flag_ToR2ToR",
        "optNum",
        "flag_last",
        "flag_app",
        "app_pkt_num",
    };
    return (field>=0 && field<24) ? fieldNames[field] : NULL;
}

int pktDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "src_Server")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "dst_Server")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "src_ToR")==0) return base+2;
    if (fieldName[0]=='d' && strcmp(fieldName, "dst_ToR")==0) return base+3;
    if (fieldName[0]=='i' && strcmp(fieldName, "index")==0) return base+4;
    if (fieldName[0]=='T' && strcmp(fieldName, "TotalHopCount")==0) return base+5;
    if (fieldName[0]=='H' && strcmp(fieldName, "HopCountNow")==0) return base+6;
    if (fieldName[0]=='H' && strcmp(fieldName, "Hop")==0) return base+7;
    if (fieldName[0]=='P' && strcmp(fieldName, "PacketSize")==0) return base+8;
    if (fieldName[0]=='n' && strcmp(fieldName, "no")==0) return base+9;
    if (fieldName[0]=='m' && strcmp(fieldName, "m")==0) return base+10;
    if (fieldName[0]=='r' && strcmp(fieldName, "retransmission")==0) return base+11;
    if (fieldName[0]=='p' && strcmp(fieldName, "path")==0) return base+12;
    if (fieldName[0]=='T' && strcmp(fieldName, "TxIndex")==0) return base+13;
    if (fieldName[0]=='G' && strcmp(fieldName, "GroupToRIndex")==0) return base+14;
    if (fieldName[0]=='E' && strcmp(fieldName, "End2EndStartTime")==0) return base+15;
    if (fieldName[0]=='P' && strcmp(fieldName, "PacketNetworkStartTime")==0) return base+16;
    if (fieldName[0]=='P' && strcmp(fieldName, "PacketEnd2EndStartTime")==0) return base+17;
    if (fieldName[0]=='f' && strcmp(fieldName, "flag_intraToR")==0) return base+18;
    if (fieldName[0]=='f' && strcmp(fieldName, "flag_ToR2ToR")==0) return base+19;
    if (fieldName[0]=='o' && strcmp(fieldName, "optNum")==0) return base+20;
    if (fieldName[0]=='f' && strcmp(fieldName, "flag_last")==0) return base+21;
    if (fieldName[0]=='f' && strcmp(fieldName, "flag_app")==0) return base+22;
    if (fieldName[0]=='a' && strcmp(fieldName, "app_pkt_num")==0) return base+23;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *pktDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "double",
        "simtime_t",
        "simtime_t",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<24) ? fieldTypeStrings[field] : NULL;
}

const char *pktDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int pktDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    pkt *pp = (pkt *)object; (void)pp;
    switch (field) {
        case 7: return 8;
        default: return 0;
    }
}

std::string pktDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    pkt *pp = (pkt *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSrc_Server());
        case 1: return long2string(pp->getDst_Server());
        case 2: return long2string(pp->getSrc_ToR());
        case 3: return long2string(pp->getDst_ToR());
        case 4: return long2string(pp->getIndex());
        case 5: return long2string(pp->getTotalHopCount());
        case 6: return long2string(pp->getHopCountNow());
        case 7: return long2string(pp->getHop(i));
        case 8: return long2string(pp->getPacketSize());
        case 9: return long2string(pp->getNo());
        case 10: return long2string(pp->getM());
        case 11: return long2string(pp->getRetransmission());
        case 12: return long2string(pp->getPath());
        case 13: return long2string(pp->getTxIndex());
        case 14: return long2string(pp->getGroupToRIndex());
        case 15: return double2string(pp->getEnd2EndStartTime());
        case 16: return double2string(pp->getPacketNetworkStartTime());
        case 17: return double2string(pp->getPacketEnd2EndStartTime());
        case 18: return long2string(pp->getFlag_intraToR());
        case 19: return long2string(pp->getFlag_ToR2ToR());
        case 20: return long2string(pp->getOptNum());
        case 21: return long2string(pp->getFlag_last());
        case 22: return long2string(pp->getFlag_app());
        case 23: return long2string(pp->getApp_pkt_num());
        default: return "";
    }
}

bool pktDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    pkt *pp = (pkt *)object; (void)pp;
    switch (field) {
        case 0: pp->setSrc_Server(string2long(value)); return true;
        case 1: pp->setDst_Server(string2long(value)); return true;
        case 2: pp->setSrc_ToR(string2long(value)); return true;
        case 3: pp->setDst_ToR(string2long(value)); return true;
        case 4: pp->setIndex(string2long(value)); return true;
        case 5: pp->setTotalHopCount(string2long(value)); return true;
        case 6: pp->setHopCountNow(string2long(value)); return true;
        case 7: pp->setHop(i,string2long(value)); return true;
        case 8: pp->setPacketSize(string2long(value)); return true;
        case 9: pp->setNo(string2long(value)); return true;
        case 10: pp->setM(string2long(value)); return true;
        case 11: pp->setRetransmission(string2long(value)); return true;
        case 12: pp->setPath(string2long(value)); return true;
        case 13: pp->setTxIndex(string2long(value)); return true;
        case 14: pp->setGroupToRIndex(string2long(value)); return true;
        case 15: pp->setEnd2EndStartTime(string2double(value)); return true;
        case 16: pp->setPacketNetworkStartTime(string2double(value)); return true;
        case 17: pp->setPacketEnd2EndStartTime(string2double(value)); return true;
        case 18: pp->setFlag_intraToR(string2long(value)); return true;
        case 19: pp->setFlag_ToR2ToR(string2long(value)); return true;
        case 20: pp->setOptNum(string2long(value)); return true;
        case 21: pp->setFlag_last(string2long(value)); return true;
        case 22: pp->setFlag_app(string2long(value)); return true;
        case 23: pp->setApp_pkt_num(string2long(value)); return true;
        default: return false;
    }
}

const char *pktDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *pktDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    pkt *pp = (pkt *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


