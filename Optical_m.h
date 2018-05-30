//
// Generated file, do not edit! Created by nedtool 4.6 from Optical.msg.
//

#ifndef _OPTICAL_M_H_
#define _OPTICAL_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
 
#include "pkt_m.h"
#include <list>
typedef pkt * pkt_Ptr;
typedef std::list<pkt *> PktVector;

 
// }}

/**
 * Class generated from <tt>Optical.msg:30</tt> by nedtool.
 * <pre>
 * packet Optical
 * {
 *     //pkt_Ptr  pkt1;
 *     pkt_Ptr pkt2;
 *     pkt_Ptr pkt3;
 *     pkt_Ptr pkt4;
 *     pkt_Ptr pkt5;
 *     //pkt  pkt1;
 *     //pkt  pkt2;
 *     //pkt  pkt3;
 *     //pkt  pkt4;
 *     //pkt  pkt5;
 *     pkt_Ptr pkt1;
 *     int PreviousToR;
 *     int NextToR;
 *     int TxIndex;
 *     int GroupToRIndex;
 *     int optical_num;
 *     int index;
 *     PktVector Optical_pkts;
 *     int app_pkt_num;
 * }
 * </pre>
 */
class Optical : public ::cPacket
{
  protected:
    pkt_Ptr pkt2_var;
    pkt_Ptr pkt3_var;
    pkt_Ptr pkt4_var;
    pkt_Ptr pkt5_var;
    pkt_Ptr pkt1_var;
    int PreviousToR_var;
    int NextToR_var;
    int TxIndex_var;
    int GroupToRIndex_var;
    int optical_num_var;
    int index_var;
    PktVector Optical_pkts_var;
    int app_pkt_num_var;

  private:
    void copy(const Optical& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Optical&);

  public:
    Optical(const char *name=NULL, int kind=0);
    Optical(const Optical& other);
    virtual ~Optical();
    Optical& operator=(const Optical& other);
    virtual Optical *dup() const {return new Optical(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual pkt_Ptr& getPkt2();
    virtual const pkt_Ptr& getPkt2() const {return const_cast<Optical*>(this)->getPkt2();}
    virtual void setPkt2(const pkt_Ptr& pkt2);
    virtual pkt_Ptr& getPkt3();
    virtual const pkt_Ptr& getPkt3() const {return const_cast<Optical*>(this)->getPkt3();}
    virtual void setPkt3(const pkt_Ptr& pkt3);
    virtual pkt_Ptr& getPkt4();
    virtual const pkt_Ptr& getPkt4() const {return const_cast<Optical*>(this)->getPkt4();}
    virtual void setPkt4(const pkt_Ptr& pkt4);
    virtual pkt_Ptr& getPkt5();
    virtual const pkt_Ptr& getPkt5() const {return const_cast<Optical*>(this)->getPkt5();}
    virtual void setPkt5(const pkt_Ptr& pkt5);
    virtual pkt_Ptr& getPkt1();
    virtual const pkt_Ptr& getPkt1() const {return const_cast<Optical*>(this)->getPkt1();}
    virtual void setPkt1(const pkt_Ptr& pkt1);
    virtual int getPreviousToR() const;
    virtual void setPreviousToR(int PreviousToR);
    virtual int getNextToR() const;
    virtual void setNextToR(int NextToR);
    virtual int getTxIndex() const;
    virtual void setTxIndex(int TxIndex);
    virtual int getGroupToRIndex() const;
    virtual void setGroupToRIndex(int GroupToRIndex);
    virtual int getOptical_num() const;
    virtual void setOptical_num(int optical_num);
    virtual int getIndex() const;
    virtual void setIndex(int index);
    virtual PktVector& getOptical_pkts();
    virtual const PktVector& getOptical_pkts() const {return const_cast<Optical*>(this)->getOptical_pkts();}
    virtual void setOptical_pkts(const PktVector& Optical_pkts);
    virtual int getApp_pkt_num() const;
    virtual void setApp_pkt_num(int app_pkt_num);

    virtual void insertMessage(pkt *msg);
    virtual void removeMessage(pkt *msg);
};

inline void doPacking(cCommBuffer *b, Optical& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Optical& obj) {obj.parsimUnpack(b);}


#endif // ifndef _OPTICAL_M_H_
