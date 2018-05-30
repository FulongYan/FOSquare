//
// Generated file, do not edit! Created by nedtool 4.6 from Optical.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "Optical_m.h"

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

Register_Class(Optical);

Optical::Optical(const char *name, int kind) : ::cPacket(name,kind)
{
    this->PreviousToR_var = 0;
    this->NextToR_var = 0;
    this->TxIndex_var = 0;
    this->GroupToRIndex_var = 0;
    this->optical_num_var = 0;
    this->index_var = 0;
    this->app_pkt_num_var = 0;
}

Optical::Optical(const Optical& other) : ::cPacket(other)
{
    copy(other);
}

Optical::~Optical()
{
}

Optical& Optical::operator=(const Optical& other)
{
    if (this==&other) return *this;
    ::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Optical::copy(const Optical& other)
{
    this->pkt2_var = other.pkt2_var;
    this->pkt3_var = other.pkt3_var;
    this->pkt4_var = other.pkt4_var;
    this->pkt5_var = other.pkt5_var;
    this->pkt1_var = other.pkt1_var;
    this->PreviousToR_var = other.PreviousToR_var;
    this->NextToR_var = other.NextToR_var;
    this->TxIndex_var = other.TxIndex_var;
    this->GroupToRIndex_var = other.GroupToRIndex_var;
    this->optical_num_var = other.optical_num_var;
    this->index_var = other.index_var;
    this->Optical_pkts_var = other.Optical_pkts_var;
    this->app_pkt_num_var = other.app_pkt_num_var;
}

void Optical::parsimPack(cCommBuffer *b)
{
    ::cPacket::parsimPack(b);
    doPacking(b,this->pkt2_var);
    doPacking(b,this->pkt3_var);
    doPacking(b,this->pkt4_var);
    doPacking(b,this->pkt5_var);
    doPacking(b,this->pkt1_var);
    doPacking(b,this->PreviousToR_var);
    doPacking(b,this->NextToR_var);
    doPacking(b,this->TxIndex_var);
    doPacking(b,this->GroupToRIndex_var);
    doPacking(b,this->optical_num_var);
    doPacking(b,this->index_var);
    doPacking(b,this->Optical_pkts_var);
    doPacking(b,this->app_pkt_num_var);
}

void Optical::parsimUnpack(cCommBuffer *b)
{
    ::cPacket::parsimUnpack(b);
    doUnpacking(b,this->pkt2_var);
    doUnpacking(b,this->pkt3_var);
    doUnpacking(b,this->pkt4_var);
    doUnpacking(b,this->pkt5_var);
    doUnpacking(b,this->pkt1_var);
    doUnpacking(b,this->PreviousToR_var);
    doUnpacking(b,this->NextToR_var);
    doUnpacking(b,this->TxIndex_var);
    doUnpacking(b,this->GroupToRIndex_var);
    doUnpacking(b,this->optical_num_var);
    doUnpacking(b,this->index_var);
    doUnpacking(b,this->Optical_pkts_var);
    doUnpacking(b,this->app_pkt_num_var);
}

pkt_Ptr& Optical::getPkt2()
{
    return pkt2_var;
}

void Optical::setPkt2(const pkt_Ptr& pkt2)
{
    this->pkt2_var = pkt2;
}

pkt_Ptr& Optical::getPkt3()
{
    return pkt3_var;
}

void Optical::setPkt3(const pkt_Ptr& pkt3)
{
    this->pkt3_var = pkt3;
}

pkt_Ptr& Optical::getPkt4()
{
    return pkt4_var;
}

void Optical::setPkt4(const pkt_Ptr& pkt4)
{
    this->pkt4_var = pkt4;
}

pkt_Ptr& Optical::getPkt5()
{
    return pkt5_var;
}

void Optical::setPkt5(const pkt_Ptr& pkt5)
{
    this->pkt5_var = pkt5;
}

pkt_Ptr& Optical::getPkt1()
{
    return pkt1_var;
}

void Optical::setPkt1(const pkt_Ptr& pkt1)
{
    this->pkt1_var = pkt1;
}

int Optical::getPreviousToR() const
{
    return PreviousToR_var;
}

void Optical::setPreviousToR(int PreviousToR)
{
    this->PreviousToR_var = PreviousToR;
}

int Optical::getNextToR() const
{
    return NextToR_var;
}

void Optical::setNextToR(int NextToR)
{
    this->NextToR_var = NextToR;
}

int Optical::getTxIndex() const
{
    return TxIndex_var;
}

void Optical::setTxIndex(int TxIndex)
{
    this->TxIndex_var = TxIndex;
}

int Optical::getGroupToRIndex() const
{
    return GroupToRIndex_var;
}

void Optical::setGroupToRIndex(int GroupToRIndex)
{
    this->GroupToRIndex_var = GroupToRIndex;
}

int Optical::getOptical_num() const
{
    return optical_num_var;
}

void Optical::setOptical_num(int optical_num)
{
    this->optical_num_var = optical_num;
}

int Optical::getIndex() const
{
    return index_var;
}

void Optical::setIndex(int index)
{
    this->index_var = index;
}

PktVector& Optical::getOptical_pkts()
{
    return Optical_pkts_var;
}

void Optical::setOptical_pkts(const PktVector& Optical_pkts)
{
    this->Optical_pkts_var = Optical_pkts;
}

int Optical::getApp_pkt_num() const
{
    return app_pkt_num_var;
}

void Optical::setApp_pkt_num(int app_pkt_num)
{
    this->app_pkt_num_var = app_pkt_num;
}

void Optical::insertMessage(pkt *msg)
{
    take(msg); // take ownership
    Optical_pkts_var.push_back(msg); // store pointer
}
void Optical::removeMessage(pkt * msg)
{
    Optical_pkts_var.remove(msg);
    drop(msg); // release ownership
}

class OpticalDescriptor : public cClassDescriptor
{
  public:
    OpticalDescriptor();
    virtual ~OpticalDescriptor();

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

Register_ClassDescriptor(OpticalDescriptor);

OpticalDescriptor::OpticalDescriptor() : cClassDescriptor("Optical", "cPacket")
{
}

OpticalDescriptor::~OpticalDescriptor()
{
}

bool OpticalDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<Optical *>(obj)!=NULL;
}

const char *OpticalDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int OpticalDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 13+basedesc->getFieldCount(object) : 13;
}

unsigned int OpticalDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<13) ? fieldTypeFlags[field] : 0;
}

const char *OpticalDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "pkt2",
        "pkt3",
        "pkt4",
        "pkt5",
        "pkt1",
        "PreviousToR",
        "NextToR",
        "TxIndex",
        "GroupToRIndex",
        "optical_num",
        "index",
        "Optical_pkts",
        "app_pkt_num",
    };
    return (field>=0 && field<13) ? fieldNames[field] : NULL;
}

int OpticalDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "pkt2")==0) return base+0;
    if (fieldName[0]=='p' && strcmp(fieldName, "pkt3")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "pkt4")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "pkt5")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "pkt1")==0) return base+4;
    if (fieldName[0]=='P' && strcmp(fieldName, "PreviousToR")==0) return base+5;
    if (fieldName[0]=='N' && strcmp(fieldName, "NextToR")==0) return base+6;
    if (fieldName[0]=='T' && strcmp(fieldName, "TxIndex")==0) return base+7;
    if (fieldName[0]=='G' && strcmp(fieldName, "GroupToRIndex")==0) return base+8;
    if (fieldName[0]=='o' && strcmp(fieldName, "optical_num")==0) return base+9;
    if (fieldName[0]=='i' && strcmp(fieldName, "index")==0) return base+10;
    if (fieldName[0]=='O' && strcmp(fieldName, "Optical_pkts")==0) return base+11;
    if (fieldName[0]=='a' && strcmp(fieldName, "app_pkt_num")==0) return base+12;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *OpticalDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "pkt_Ptr",
        "pkt_Ptr",
        "pkt_Ptr",
        "pkt_Ptr",
        "pkt_Ptr",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "PktVector",
        "int",
    };
    return (field>=0 && field<13) ? fieldTypeStrings[field] : NULL;
}

const char *OpticalDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int OpticalDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    Optical *pp = (Optical *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string OpticalDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    Optical *pp = (Optical *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getPkt2(); return out.str();}
        case 1: {std::stringstream out; out << pp->getPkt3(); return out.str();}
        case 2: {std::stringstream out; out << pp->getPkt4(); return out.str();}
        case 3: {std::stringstream out; out << pp->getPkt5(); return out.str();}
        case 4: {std::stringstream out; out << pp->getPkt1(); return out.str();}
        case 5: return long2string(pp->getPreviousToR());
        case 6: return long2string(pp->getNextToR());
        case 7: return long2string(pp->getTxIndex());
        case 8: return long2string(pp->getGroupToRIndex());
        case 9: return long2string(pp->getOptical_num());
        case 10: return long2string(pp->getIndex());
        case 11: {std::stringstream out; out << pp->getOptical_pkts(); return out.str();}
        case 12: return long2string(pp->getApp_pkt_num());
        default: return "";
    }
}

bool OpticalDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    Optical *pp = (Optical *)object; (void)pp;
    switch (field) {
        case 5: pp->setPreviousToR(string2long(value)); return true;
        case 6: pp->setNextToR(string2long(value)); return true;
        case 7: pp->setTxIndex(string2long(value)); return true;
        case 8: pp->setGroupToRIndex(string2long(value)); return true;
        case 9: pp->setOptical_num(string2long(value)); return true;
        case 10: pp->setIndex(string2long(value)); return true;
        case 12: pp->setApp_pkt_num(string2long(value)); return true;
        default: return false;
    }
}

const char *OpticalDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0: return opp_typename(typeid(pkt_Ptr));
        case 1: return opp_typename(typeid(pkt_Ptr));
        case 2: return opp_typename(typeid(pkt_Ptr));
        case 3: return opp_typename(typeid(pkt_Ptr));
        case 4: return opp_typename(typeid(pkt_Ptr));
        case 11: return opp_typename(typeid(PktVector));
        default: return NULL;
    };
}

void *OpticalDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    Optical *pp = (Optical *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getPkt2()); break;
        case 1: return (void *)(&pp->getPkt3()); break;
        case 2: return (void *)(&pp->getPkt4()); break;
        case 3: return (void *)(&pp->getPkt5()); break;
        case 4: return (void *)(&pp->getPkt1()); break;
        case 11: return (void *)(&pp->getOptical_pkts()); break;
        default: return NULL;
    }
}


