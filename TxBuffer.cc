/*
 * buffer.cc
 *
 *  Created on: Jul 6, 2015
 *      Author: fyan
 */
/*
 * head.cc
 *
 *  Created on: Jul 3, 2015
 *      Author: fyan
 */

#include<stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "pkt_m.h"
#include "Optical_m.h"

class TxBuffer : public cSimpleModule
{
    private:
        int BackupQueueopticalNum;
        int OccupyMax;
        int DirectTrans;
        int TXSend;

        int Down2ServerBufferSize;
        int ToR2OPS_BufferSize;

        int Queue2CoreAveragelost ;
        int Queue2AgreAveragelost ;
        int Queue2ServerAveragelost ;
        int getGroupToRIndex;
        double HeadProcessdelay;
        int GroupToR;
		int GroupToRCore;
        int OpticalPacketNum;

        double slot;
        int NumTx;
		int NumTx_ToR2ToR;
        simtime_t t0;

        pkt *local_clock;

        cQueue Queue2OPS[48];
        cQueue BackupQueue[48];
        bool Queue2OPS_busy;
        int Queue2OPSlost;

        int Queue2OPSPacketlost;

        int Received_Packets ;
        int SimulatedPacketNum;

        int SimulatedClock;
        bool finishflag;

        int test;

        bool QueueArrayflag; // 1 is backup queue;

        double linkDataRate;

        int CellSize;
        double BufferTime;

        int BackupQueueIndex;

        int app_pkt_num;
    protected:

         virtual void initialize();
         virtual void handleMessage(cMessage *msg);
         virtual void finish();
    public:
         TxBuffer();
         virtual ~TxBuffer();
};

Define_Module(TxBuffer);

TxBuffer::TxBuffer(){
    local_clock = NULL;
}
TxBuffer::~TxBuffer(){
    cancelAndDelete(local_clock);
    for(int i=0;i<48;i++){
        while( !Queue2OPS[i].empty() ){
                    delete (pkt *)Queue2OPS[i].pop();
         }

         while( !BackupQueue[i].empty() ){
                    delete (pkt *)BackupQueue[i].pop();
         }
    }
}
void TxBuffer::initialize()
{
    BackupQueueopticalNum = 0;
    linkDataRate = par("linkDataRate");
    ToR2OPS_BufferSize = par("BufferSize2OPS");
    CellSize = par("CellSize");
    BufferTime =  CellSize*8/linkDataRate;
    GroupToR = par("GroupToR");
	GroupToRCore = par("GroupToRCore");
    NumTx = par("NumTx");
	NumTx_ToR2ToR = par("NumTx_ToR2ToR");
    OpticalPacketNum = par("OpticalPacketNum");
//    endToEndDelayVec.setName("End-to-End Dealy");
    Queue2CoreAveragelost = 0;
    Queue2AgreAveragelost = 0;
    Queue2ServerAveragelost = 0;

    Queue2OPSlost = 0;
    Queue2OPSPacketlost = 0;
    finishflag = false;
    QueueArrayflag = false;


    HeadProcessdelay = 80e-9;
    slot = 1200e-9;
    t0 = 0;
    Received_Packets = 0;
    SimulatedPacketNum =  par("SimulatedPacketNum");
    SimulatedClock = 0;
    test = 0;

    TXSend = 0;
    OccupyMax = 0;
    DirectTrans = 0;
    local_clock = new pkt ("local_clock");
    local_clock->setIndex(0);
    scheduleAt(t0, local_clock);

    app_pkt_num = 0;
}

void TxBuffer::handleMessage(cMessage *msg)
{

    pkt *pkt_temp =  check_and_cast<pkt*>(msg);

    if(pkt_temp->getIndex() == 1){

  //      EV<<"one packet arrived";
        Received_Packets++;
 //       EV<<"Received_Packets is "<<Received_Packets<<"\n";
 //       EV<<"SimulatedPacketNum is "<<SimulatedPacketNum<<"\n";
        int CurrentToR = pkt_temp->getHop(pkt_temp->getHopCountNow());
        int TxIndex = pkt_temp->getTxIndex();
        int QueueOfOPSIndex = (NumTx+1)*CurrentToR + TxIndex;

//        EV<<"CurrentToR is"<<CurrentToR <<"QueueOfOPSIndex is" <<QueueOfOPSIndex <<"TxIndex is" <<TxIndex;
//        EV<<"Package size is "<< pkt_temp->getPacketSize();
//        EV<< "received a packet from" <<pkt_temp->getSrc_Server() <<" to "<<pkt_temp->getDst_Server()<<"\n";
        int OccupyTX = 0;
        getGroupToRIndex = pkt_temp->getGroupToRIndex();

        if(( getIndex()%(NumTx+NumTx_ToR2ToR) )<NumTx){
            for(int i=0;i<GroupToR;i++){
                OccupyTX += Queue2OPS[i].length();
            }
        }
        else{
            for(int i=0;i<GroupToRCore;i++){
                OccupyTX += Queue2OPS[i].length();
            }
        }

        if(pkt_temp->getPacketSize()<=1500){
  //          EV<<"Package size is 300";
            if( OccupyTX >= ToR2OPS_BufferSize ){
                Queue2OPSlost++;
                Queue2OPSPacketlost++;
                delete msg;
            }
            else{
                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime( simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 1500 && pkt_temp->getPacketSize() <= 3000){

            if( OccupyTX >= ToR2OPS_BufferSize-1 ){
                   Queue2OPSlost = Queue2OPSlost + 2;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 3000 && pkt_temp->getPacketSize() <= 4500){

            if( OccupyTX >= ToR2OPS_BufferSize-2 ){
                   Queue2OPSlost = Queue2OPSlost + 3;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 4500 && pkt_temp->getPacketSize() <= 6000){

            if( OccupyTX >= ToR2OPS_BufferSize-3 ){
                   Queue2OPSlost = Queue2OPSlost + 4;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 18000 && pkt_temp->getPacketSize() <= 19500){

            if( OccupyTX >= ToR2OPS_BufferSize-12 ){
                   Queue2OPSlost = Queue2OPSlost + 13;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                pkt *pkt_temp_4 = pkt_temp->dup();
                pkt *pkt_temp_5 = pkt_temp->dup();
                pkt *pkt_temp_6 = pkt_temp->dup();
                pkt *pkt_temp_7 = pkt_temp->dup();
                pkt *pkt_temp_8 = pkt_temp->dup();
                pkt *pkt_temp_9 = pkt_temp->dup();
                pkt *pkt_temp_10 = pkt_temp->dup();
                pkt *pkt_temp_11 = pkt_temp->dup();
                pkt *pkt_temp_12 = pkt_temp->dup();

                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 21000 && pkt_temp->getPacketSize() <= 22500){

            if( OccupyTX >= ToR2OPS_BufferSize-14 ){
                   Queue2OPSlost = Queue2OPSlost + 15;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                pkt *pkt_temp_4 = pkt_temp->dup();
                pkt *pkt_temp_5 = pkt_temp->dup();
                pkt *pkt_temp_6 = pkt_temp->dup();
                pkt *pkt_temp_7 = pkt_temp->dup();
                pkt *pkt_temp_8 = pkt_temp->dup();
                pkt *pkt_temp_9 = pkt_temp->dup();
                pkt *pkt_temp_10 = pkt_temp->dup();
                pkt *pkt_temp_11 = pkt_temp->dup();
                pkt *pkt_temp_12 = pkt_temp->dup();
                pkt *pkt_temp_13 = pkt_temp->dup();
                pkt *pkt_temp_14 = pkt_temp->dup();
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 24000 && pkt_temp->getPacketSize() <= 25500){

             if( OccupyTX >= ToR2OPS_BufferSize-16 ){
                    Queue2OPSlost = Queue2OPSlost + 17;
                    Queue2OPSPacketlost++;
                    delete msg;
               }
             else{
                 pkt_temp->setPacketSize(1500);
                 pkt *pkt_temp_1 = pkt_temp->dup();
                 pkt *pkt_temp_2 = pkt_temp->dup();
                 pkt *pkt_temp_3 = pkt_temp->dup();
                 pkt *pkt_temp_4 = pkt_temp->dup();
                 pkt *pkt_temp_5 = pkt_temp->dup();
                 pkt *pkt_temp_6 = pkt_temp->dup();
                 pkt *pkt_temp_7 = pkt_temp->dup();
                 pkt *pkt_temp_8 = pkt_temp->dup();
                 pkt *pkt_temp_9 = pkt_temp->dup();
                 pkt *pkt_temp_10 = pkt_temp->dup();
                 pkt *pkt_temp_11 = pkt_temp->dup();
                 pkt *pkt_temp_12 = pkt_temp->dup();
                 pkt *pkt_temp_13 = pkt_temp->dup();
                 pkt *pkt_temp_14 = pkt_temp->dup();
                 pkt *pkt_temp_15 = pkt_temp->dup();
                 pkt *pkt_temp_16 = pkt_temp->dup();
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_15);
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp_16);
                 if(pkt_temp->getHopCountNow() == 0){
                     pkt_temp->setFlag_last(1);
                 }
                 Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                 pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
             }
         }
        else if(pkt_temp->getPacketSize() > 27000 && pkt_temp->getPacketSize() <= 28500){

            if( OccupyTX >= ToR2OPS_BufferSize-18 ){
                   Queue2OPSlost = Queue2OPSlost + 19;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                pkt *pkt_temp_4 = pkt_temp->dup();
                pkt *pkt_temp_5 = pkt_temp->dup();
                pkt *pkt_temp_6 = pkt_temp->dup();
                pkt *pkt_temp_7 = pkt_temp->dup();
                pkt *pkt_temp_8 = pkt_temp->dup();
                pkt *pkt_temp_9 = pkt_temp->dup();
                pkt *pkt_temp_10 = pkt_temp->dup();
                pkt *pkt_temp_11 = pkt_temp->dup();
                pkt *pkt_temp_12 = pkt_temp->dup();
                pkt *pkt_temp_13 = pkt_temp->dup();
                pkt *pkt_temp_14 = pkt_temp->dup();
                pkt *pkt_temp_15 = pkt_temp->dup();
                pkt *pkt_temp_16 = pkt_temp->dup();
                pkt *pkt_temp_17 = pkt_temp->dup();
                pkt *pkt_temp_18 = pkt_temp->dup();

                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_15);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_16);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_17);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_18);

                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 28500 && pkt_temp->getPacketSize() <= 30000){

            if( OccupyTX >= ToR2OPS_BufferSize-19 ){
                   Queue2OPSlost = Queue2OPSlost + 20;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                pkt *pkt_temp_4 = pkt_temp->dup();
                pkt *pkt_temp_5 = pkt_temp->dup();
                pkt *pkt_temp_6 = pkt_temp->dup();
                pkt *pkt_temp_7 = pkt_temp->dup();
                pkt *pkt_temp_8 = pkt_temp->dup();
                pkt *pkt_temp_9 = pkt_temp->dup();
                pkt *pkt_temp_10 = pkt_temp->dup();
                pkt *pkt_temp_11 = pkt_temp->dup();
                pkt *pkt_temp_12 = pkt_temp->dup();
                pkt *pkt_temp_13 = pkt_temp->dup();
                pkt *pkt_temp_14 = pkt_temp->dup();
                pkt *pkt_temp_15 = pkt_temp->dup();
                pkt *pkt_temp_16 = pkt_temp->dup();
                pkt *pkt_temp_17 = pkt_temp->dup();
                pkt *pkt_temp_18 = pkt_temp->dup();
                pkt *pkt_temp_19 = pkt_temp->dup();

                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_15);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_16);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_17);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_18);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_19);

                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 54000 && pkt_temp->getPacketSize() <= 55500){

            if( OccupyTX >= ToR2OPS_BufferSize-36 ){
                   Queue2OPSlost = Queue2OPSlost + 37;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                pkt *pkt_temp_4 = pkt_temp->dup();
                pkt *pkt_temp_5 = pkt_temp->dup();
                pkt *pkt_temp_6 = pkt_temp->dup();
                pkt *pkt_temp_7 = pkt_temp->dup();
                pkt *pkt_temp_8 = pkt_temp->dup();
                pkt *pkt_temp_9 = pkt_temp->dup();
                pkt *pkt_temp_10 = pkt_temp->dup();
                pkt *pkt_temp_11 = pkt_temp->dup();
                pkt *pkt_temp_12 = pkt_temp->dup();
                pkt *pkt_temp_13 = pkt_temp->dup();
                pkt *pkt_temp_14 = pkt_temp->dup();
                pkt *pkt_temp_15 = pkt_temp->dup();
                pkt *pkt_temp_16 = pkt_temp->dup();
                pkt *pkt_temp_17 = pkt_temp->dup();
                pkt *pkt_temp_18 = pkt_temp->dup();
                pkt *pkt_temp_19 = pkt_temp->dup();
                pkt *pkt_temp_20 = pkt_temp->dup();
                pkt *pkt_temp_21 = pkt_temp->dup();
                pkt *pkt_temp_22 = pkt_temp->dup();
                pkt *pkt_temp_23 = pkt_temp->dup();
                pkt *pkt_temp_24 = pkt_temp->dup();
                pkt *pkt_temp_25 = pkt_temp->dup();
                pkt *pkt_temp_26 = pkt_temp->dup();
                pkt *pkt_temp_27 = pkt_temp->dup();
                pkt *pkt_temp_28 = pkt_temp->dup();
                pkt *pkt_temp_29 = pkt_temp->dup();
                pkt *pkt_temp_30 = pkt_temp->dup();
                pkt *pkt_temp_31 = pkt_temp->dup();
                pkt *pkt_temp_32 = pkt_temp->dup();
                pkt *pkt_temp_33 = pkt_temp->dup();
                pkt *pkt_temp_34 = pkt_temp->dup();
                pkt *pkt_temp_35 = pkt_temp->dup();
                pkt *pkt_temp_36 = pkt_temp->dup();

                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_15);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_16);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_17);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_18);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_19);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_20);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_21);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_22);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_23);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_24);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_25);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_26);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_27);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_28);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_29);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_30);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_31);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_32);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_33);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_34);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_35);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_36);

                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 64500 && pkt_temp->getPacketSize() <= 66000){

            if( OccupyTX >= ToR2OPS_BufferSize-43 ){
                   Queue2OPSlost = Queue2OPSlost + 44;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                pkt *pkt_temp_4 = pkt_temp->dup();
                pkt *pkt_temp_5 = pkt_temp->dup();
                pkt *pkt_temp_6 = pkt_temp->dup();
                pkt *pkt_temp_7 = pkt_temp->dup();
                pkt *pkt_temp_8 = pkt_temp->dup();
                pkt *pkt_temp_9 = pkt_temp->dup();
                pkt *pkt_temp_10 = pkt_temp->dup();
                pkt *pkt_temp_11 = pkt_temp->dup();
                pkt *pkt_temp_12 = pkt_temp->dup();
                pkt *pkt_temp_13 = pkt_temp->dup();
                pkt *pkt_temp_14 = pkt_temp->dup();
                pkt *pkt_temp_15 = pkt_temp->dup();
                pkt *pkt_temp_16 = pkt_temp->dup();
                pkt *pkt_temp_17 = pkt_temp->dup();
                pkt *pkt_temp_18 = pkt_temp->dup();
                pkt *pkt_temp_19 = pkt_temp->dup();
                pkt *pkt_temp_20 = pkt_temp->dup();
                pkt *pkt_temp_21 = pkt_temp->dup();
                pkt *pkt_temp_22 = pkt_temp->dup();
                pkt *pkt_temp_23 = pkt_temp->dup();
                pkt *pkt_temp_24 = pkt_temp->dup();
                pkt *pkt_temp_25 = pkt_temp->dup();
                pkt *pkt_temp_26 = pkt_temp->dup();
                pkt *pkt_temp_27 = pkt_temp->dup();
                pkt *pkt_temp_28 = pkt_temp->dup();
                pkt *pkt_temp_29 = pkt_temp->dup();
                pkt *pkt_temp_30 = pkt_temp->dup();
                pkt *pkt_temp_31 = pkt_temp->dup();
                pkt *pkt_temp_32 = pkt_temp->dup();
                pkt *pkt_temp_33 = pkt_temp->dup();
                pkt *pkt_temp_34 = pkt_temp->dup();
                pkt *pkt_temp_35 = pkt_temp->dup();
                pkt *pkt_temp_36 = pkt_temp->dup();
                pkt *pkt_temp_37 = pkt_temp->dup();
                pkt *pkt_temp_38 = pkt_temp->dup();
                pkt *pkt_temp_39 = pkt_temp->dup();
                pkt *pkt_temp_40 = pkt_temp->dup();
                pkt *pkt_temp_41 = pkt_temp->dup();
                pkt *pkt_temp_42 = pkt_temp->dup();
                pkt *pkt_temp_43 = pkt_temp->dup();
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_15);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_16);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_17);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_18);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_19);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_20);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_21);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_22);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_23);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_24);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_25);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_26);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_27);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_28);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_29);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_30);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_31);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_32);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_33);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_34);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_35);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_36);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_37);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_38);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_39);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_40);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_41);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_42);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_43);

                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 66000 && pkt_temp->getPacketSize() <= 67500){

                if( OccupyTX >= ToR2OPS_BufferSize-44 ){
                       Queue2OPSlost = Queue2OPSlost + 45;
                       Queue2OPSPacketlost++;
                       delete msg;
                  }
                else{
                    pkt_temp->setPacketSize(1500);
                    pkt *pkt_temp_1 = pkt_temp->dup();
                    pkt *pkt_temp_2 = pkt_temp->dup();
                    pkt *pkt_temp_3 = pkt_temp->dup();
                    pkt *pkt_temp_4 = pkt_temp->dup();
                    pkt *pkt_temp_5 = pkt_temp->dup();
                    pkt *pkt_temp_6 = pkt_temp->dup();
                    pkt *pkt_temp_7 = pkt_temp->dup();
                    pkt *pkt_temp_8 = pkt_temp->dup();
                    pkt *pkt_temp_9 = pkt_temp->dup();
                    pkt *pkt_temp_10 = pkt_temp->dup();
                    pkt *pkt_temp_11 = pkt_temp->dup();
                    pkt *pkt_temp_12 = pkt_temp->dup();
                    pkt *pkt_temp_13 = pkt_temp->dup();
                    pkt *pkt_temp_14 = pkt_temp->dup();
                    pkt *pkt_temp_15 = pkt_temp->dup();
                    pkt *pkt_temp_16 = pkt_temp->dup();
                    pkt *pkt_temp_17 = pkt_temp->dup();
                    pkt *pkt_temp_18 = pkt_temp->dup();
                    pkt *pkt_temp_19 = pkt_temp->dup();
                    pkt *pkt_temp_20 = pkt_temp->dup();
                    pkt *pkt_temp_21 = pkt_temp->dup();
                    pkt *pkt_temp_22 = pkt_temp->dup();
                    pkt *pkt_temp_23 = pkt_temp->dup();
                    pkt *pkt_temp_24 = pkt_temp->dup();
                    pkt *pkt_temp_25 = pkt_temp->dup();
                    pkt *pkt_temp_26 = pkt_temp->dup();
                    pkt *pkt_temp_27 = pkt_temp->dup();
                    pkt *pkt_temp_28 = pkt_temp->dup();
                    pkt *pkt_temp_29 = pkt_temp->dup();
                    pkt *pkt_temp_30 = pkt_temp->dup();
                    pkt *pkt_temp_31 = pkt_temp->dup();
                    pkt *pkt_temp_32 = pkt_temp->dup();
                    pkt *pkt_temp_33 = pkt_temp->dup();
                    pkt *pkt_temp_34 = pkt_temp->dup();
                    pkt *pkt_temp_35 = pkt_temp->dup();
                    pkt *pkt_temp_36 = pkt_temp->dup();
                    pkt *pkt_temp_37 = pkt_temp->dup();
                    pkt *pkt_temp_38 = pkt_temp->dup();
                    pkt *pkt_temp_39 = pkt_temp->dup();
                    pkt *pkt_temp_40 = pkt_temp->dup();
                    pkt *pkt_temp_41 = pkt_temp->dup();
                    pkt *pkt_temp_42 = pkt_temp->dup();
                    pkt *pkt_temp_43 = pkt_temp->dup();
                    pkt *pkt_temp_44 = pkt_temp->dup();
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_15);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_16);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_17);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_18);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_19);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_20);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_21);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_22);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_23);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_24);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_25);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_26);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_27);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_28);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_29);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_30);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_31);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_32);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_33);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_34);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_35);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_36);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_37);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_38);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_39);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_40);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_41);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_42);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_43);
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp_44);
                    if(pkt_temp->getHopCountNow() == 0){
                        pkt_temp->setFlag_last(1);
                    }
                    Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                    pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
                }
            }
        else if(pkt_temp->getPacketSize() > 72000 && pkt_temp->getPacketSize() <= 73500){

            if( OccupyTX >= ToR2OPS_BufferSize-48 ){
                   Queue2OPSlost = Queue2OPSlost + 49;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                pkt *pkt_temp_4 = pkt_temp->dup();
                pkt *pkt_temp_5 = pkt_temp->dup();
                pkt *pkt_temp_6 = pkt_temp->dup();
                pkt *pkt_temp_7 = pkt_temp->dup();
                pkt *pkt_temp_8 = pkt_temp->dup();
                pkt *pkt_temp_9 = pkt_temp->dup();
                pkt *pkt_temp_10 = pkt_temp->dup();
                pkt *pkt_temp_11 = pkt_temp->dup();
                pkt *pkt_temp_12 = pkt_temp->dup();
                pkt *pkt_temp_13 = pkt_temp->dup();
                pkt *pkt_temp_14 = pkt_temp->dup();
                pkt *pkt_temp_15 = pkt_temp->dup();
                pkt *pkt_temp_16 = pkt_temp->dup();
                pkt *pkt_temp_17 = pkt_temp->dup();
                pkt *pkt_temp_18 = pkt_temp->dup();
                pkt *pkt_temp_19 = pkt_temp->dup();
                pkt *pkt_temp_20 = pkt_temp->dup();
                pkt *pkt_temp_21 = pkt_temp->dup();
                pkt *pkt_temp_22 = pkt_temp->dup();
                pkt *pkt_temp_23 = pkt_temp->dup();
                pkt *pkt_temp_24 = pkt_temp->dup();
                pkt *pkt_temp_25 = pkt_temp->dup();
                pkt *pkt_temp_26 = pkt_temp->dup();
                pkt *pkt_temp_27 = pkt_temp->dup();
                pkt *pkt_temp_28 = pkt_temp->dup();
                pkt *pkt_temp_29 = pkt_temp->dup();
                pkt *pkt_temp_30 = pkt_temp->dup();
                pkt *pkt_temp_31 = pkt_temp->dup();
                pkt *pkt_temp_32 = pkt_temp->dup();
                pkt *pkt_temp_33 = pkt_temp->dup();
                pkt *pkt_temp_34 = pkt_temp->dup();
                pkt *pkt_temp_35 = pkt_temp->dup();
                pkt *pkt_temp_36 = pkt_temp->dup();
                pkt *pkt_temp_37 = pkt_temp->dup();
                pkt *pkt_temp_38 = pkt_temp->dup();
                pkt *pkt_temp_39 = pkt_temp->dup();
                pkt *pkt_temp_40 = pkt_temp->dup();
                pkt *pkt_temp_41 = pkt_temp->dup();
                pkt *pkt_temp_42 = pkt_temp->dup();
                pkt *pkt_temp_43 = pkt_temp->dup();
                pkt *pkt_temp_44 = pkt_temp->dup();
                pkt *pkt_temp_45 = pkt_temp->dup();
                pkt *pkt_temp_46 = pkt_temp->dup();
                pkt *pkt_temp_47 = pkt_temp->dup();
                pkt *pkt_temp_48 = pkt_temp->dup();

                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_15);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_16);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_17);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_18);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_19);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_20);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_21);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_22);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_23);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_24);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_25);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_26);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_27);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_28);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_29);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_30);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_31);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_32);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_33);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_34);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_35);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_36);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_37);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_38);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_39);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_40);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_41);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_42);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_43);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_44);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_45);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_46);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_47);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_48);

                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else if(pkt_temp->getPacketSize() > 135000 && pkt_temp->getPacketSize() <= 136500){

            if( OccupyTX >= ToR2OPS_BufferSize-90 ){
                   Queue2OPSlost = Queue2OPSlost + 91;
                   Queue2OPSPacketlost++;
                   delete msg;
              }
            else{
                pkt_temp->setPacketSize(1500);
                pkt *pkt_temp_1 = pkt_temp->dup();
                pkt *pkt_temp_2 = pkt_temp->dup();
                pkt *pkt_temp_3 = pkt_temp->dup();
                pkt *pkt_temp_4 = pkt_temp->dup();
                pkt *pkt_temp_5 = pkt_temp->dup();
                pkt *pkt_temp_6 = pkt_temp->dup();
                pkt *pkt_temp_7 = pkt_temp->dup();
                pkt *pkt_temp_8 = pkt_temp->dup();
                pkt *pkt_temp_9 = pkt_temp->dup();
                pkt *pkt_temp_10 = pkt_temp->dup();
                pkt *pkt_temp_11 = pkt_temp->dup();
                pkt *pkt_temp_12 = pkt_temp->dup();
                pkt *pkt_temp_13 = pkt_temp->dup();
                pkt *pkt_temp_14 = pkt_temp->dup();
                pkt *pkt_temp_15 = pkt_temp->dup();
                pkt *pkt_temp_16 = pkt_temp->dup();
                pkt *pkt_temp_17 = pkt_temp->dup();
                pkt *pkt_temp_18 = pkt_temp->dup();
                pkt *pkt_temp_19 = pkt_temp->dup();
                pkt *pkt_temp_20 = pkt_temp->dup();
                pkt *pkt_temp_21 = pkt_temp->dup();
                pkt *pkt_temp_22 = pkt_temp->dup();
                pkt *pkt_temp_23 = pkt_temp->dup();
                pkt *pkt_temp_24 = pkt_temp->dup();
                pkt *pkt_temp_25 = pkt_temp->dup();
                pkt *pkt_temp_26 = pkt_temp->dup();
                pkt *pkt_temp_27 = pkt_temp->dup();
                pkt *pkt_temp_28 = pkt_temp->dup();
                pkt *pkt_temp_29 = pkt_temp->dup();
                pkt *pkt_temp_30 = pkt_temp->dup();
                pkt *pkt_temp_31 = pkt_temp->dup();
                pkt *pkt_temp_32 = pkt_temp->dup();
                pkt *pkt_temp_33 = pkt_temp->dup();
                pkt *pkt_temp_34 = pkt_temp->dup();
                pkt *pkt_temp_35 = pkt_temp->dup();
                pkt *pkt_temp_36 = pkt_temp->dup();
                pkt *pkt_temp_37 = pkt_temp->dup();
                pkt *pkt_temp_38 = pkt_temp->dup();
                pkt *pkt_temp_39 = pkt_temp->dup();
                pkt *pkt_temp_40 = pkt_temp->dup();
                pkt *pkt_temp_41 = pkt_temp->dup();
                pkt *pkt_temp_42 = pkt_temp->dup();
                pkt *pkt_temp_43 = pkt_temp->dup();
                pkt *pkt_temp_44 = pkt_temp->dup();
                pkt *pkt_temp_45 = pkt_temp->dup();
                pkt *pkt_temp_46 = pkt_temp->dup();
                pkt *pkt_temp_47 = pkt_temp->dup();
                pkt *pkt_temp_48 = pkt_temp->dup();
                pkt *pkt_temp_49 = pkt_temp->dup();
                pkt *pkt_temp_50 = pkt_temp->dup();
                pkt *pkt_temp_51 = pkt_temp->dup();
                pkt *pkt_temp_52 = pkt_temp->dup();
                pkt *pkt_temp_53 = pkt_temp->dup();
                pkt *pkt_temp_54 = pkt_temp->dup();
                pkt *pkt_temp_55 = pkt_temp->dup();
                pkt *pkt_temp_56 = pkt_temp->dup();
                pkt *pkt_temp_57 = pkt_temp->dup();
                pkt *pkt_temp_58 = pkt_temp->dup();
                pkt *pkt_temp_59 = pkt_temp->dup();
                pkt *pkt_temp_60 = pkt_temp->dup();
                pkt *pkt_temp_61 = pkt_temp->dup();
                pkt *pkt_temp_62 = pkt_temp->dup();
                pkt *pkt_temp_63 = pkt_temp->dup();
                pkt *pkt_temp_64 = pkt_temp->dup();
                pkt *pkt_temp_65 = pkt_temp->dup();
                pkt *pkt_temp_66 = pkt_temp->dup();
                pkt *pkt_temp_67 = pkt_temp->dup();
                pkt *pkt_temp_68 = pkt_temp->dup();
                pkt *pkt_temp_69 = pkt_temp->dup();
                pkt *pkt_temp_70 = pkt_temp->dup();
                pkt *pkt_temp_71 = pkt_temp->dup();
                pkt *pkt_temp_72 = pkt_temp->dup();
                pkt *pkt_temp_73 = pkt_temp->dup();
                pkt *pkt_temp_74 = pkt_temp->dup();
                pkt *pkt_temp_75 = pkt_temp->dup();
                pkt *pkt_temp_76 = pkt_temp->dup();
                pkt *pkt_temp_77 = pkt_temp->dup();
                pkt *pkt_temp_78 = pkt_temp->dup();
                pkt *pkt_temp_79 = pkt_temp->dup();
                pkt *pkt_temp_80 = pkt_temp->dup();
                pkt *pkt_temp_81 = pkt_temp->dup();
                pkt *pkt_temp_82 = pkt_temp->dup();
                pkt *pkt_temp_83 = pkt_temp->dup();
                pkt *pkt_temp_84 = pkt_temp->dup();
                pkt *pkt_temp_85 = pkt_temp->dup();
                pkt *pkt_temp_86 = pkt_temp->dup();
                pkt *pkt_temp_87 = pkt_temp->dup();
                pkt *pkt_temp_88 = pkt_temp->dup();
                pkt *pkt_temp_89 = pkt_temp->dup();
                pkt *pkt_temp_90 = pkt_temp->dup();

                Queue2OPS[getGroupToRIndex].insert(pkt_temp_1);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_2);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_3);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_4);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_5);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_6);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_7);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_8);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_9);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_10);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_11);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_12);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_13);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_14);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_15);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_16);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_17);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_18);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_19);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_20);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_21);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_22);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_23);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_24);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_25);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_26);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_27);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_28);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_29);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_30);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_31);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_32);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_33);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_34);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_35);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_36);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_37);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_38);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_39);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_40);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_41);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_42);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_43);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_44);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_45);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_46);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_47);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_48);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_49);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_50);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_51);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_52);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_53);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_54);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_55);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_56);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_57);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_58);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_59);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_60);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_61);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_62);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_63);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_64);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_65);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_66);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_67);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_68);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_69);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_70);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_71);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_72);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_73);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_74);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_75);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_76);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_77);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_78);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_79);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_80);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_81);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_82);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_83);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_84);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_85);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_86);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_87);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_88);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_89);
                Queue2OPS[getGroupToRIndex].insert(pkt_temp_90);

                if(pkt_temp->getHopCountNow() == 0){
                    pkt_temp->setFlag_last(1);
                }
                Queue2OPS[getGroupToRIndex].insert(pkt_temp);
                pkt_temp->setPacketNetworkStartTime(simTime()  ) ;
            }
        }
        else {
            delete msg;
        }
            if(getIndex()<NumTx){
                for(int i=0;i<GroupToR;i++){
                    OccupyTX += Queue2OPS[i].length();
                }
                if(OccupyTX > OccupyMax){
                    OccupyMax = OccupyTX;
                }
            }
            else{
                for(int i=0;i<GroupToRCore;i++){
                    OccupyTX += Queue2OPS[i].length();
                }
                if(OccupyTX > OccupyMax){
                    OccupyMax = OccupyTX;
                }
            }
        if(finishflag == true){
        	if(( getIndex()%(NumTx+NumTx_ToR2ToR) )<NumTx){
				for(int i=0;i<GroupToR;i++){			
		            if( (Queue2OPS[i].length() != 0) ){
		 //               EV<<"Queue2OPS length before the last send is"<<Queue2OPS.length()<<"\n";
		                pkt *pkt_send = check_and_cast<pkt*>(Queue2OPS[i].pop());

		                if(pkt_temp->getHopCountNow() == 1)
		                    msg->setTimestamp(simTime());
		                int countNow = pkt_send->getHopCountNow();
		  //              EV<< "countNow is" <<countNow;
		                pkt_send->setHopCountNow(countNow + 1);

                        sendDelayed(pkt_send,slot,"out");
		  //              EV<<"Queue2OPS length after the last send is"<<Queue2OPS.length()<<"\n";
		                test++;
		                while( (BackupQueue[i].length() != 0) ){
							delete BackupQueue[i].pop();
						}
		               // wait(slot);
		            }
		        }
		     }

		  else{
		  		for(int i=0;i<GroupToRCore;i++){			
		            if( (Queue2OPS[i].length() != 0) ){
		 //               EV<<"Queue2OPS length before the last send is"<<Queue2OPS.length()<<"\n";
		                pkt *pkt_send = check_and_cast<pkt*>(Queue2OPS[i].pop());

		                if(pkt_temp->getHopCountNow() == 1)
		                    msg->setTimestamp(simTime());
		                int countNow = pkt_send->getHopCountNow();
		  //              EV<< "countNow is" <<countNow;
		                pkt_send->setHopCountNow(countNow + 1);

                        sendDelayed(pkt_send,slot,"out");
		  //              EV<<"Queue2OPS length after the last send is"<<Queue2OPS.length()<<"\n";
		                test++;
		                while( (BackupQueue[i].length() != 0) ){
							delete BackupQueue[i].pop();
						}
		               // wait(slot);
		            }
		        }
		  
		  }
        }
    }
    else if(pkt_temp->getIndex() == 0){

//        if(Received_Packets < SimulatedPacketNum){
        if(finishflag != true){
            int maxQueueIndex = 0;
            int maxQueuelength = -1;
            if((getIndex()%(NumTx+NumTx_ToR2ToR) )<NumTx){
                for(int i=0;i<GroupToR;i++){
                    if( Queue2OPS[i].length() > maxQueuelength ){
                        maxQueuelength = Queue2OPS[i].length();
                        maxQueueIndex = i;
                    }
                }
            }
            else{
                for(int i=0;i<GroupToRCore;i++){
                    if( Queue2OPS[i].length() > maxQueuelength ){
                        maxQueuelength = Queue2OPS[i].length();
                        maxQueueIndex = i;
                    }
                }
            }

                if(QueueArrayflag == true){
                    app_pkt_num = 0;
                    Optical *pkt_optical_temp_send = new Optical();
                    pkt_optical_temp_send->setKind(1000);
                    for(int i=0;i<BackupQueueopticalNum;i++){
                              if(BackupQueue[BackupQueueIndex].length()>0){
                                  switch(i){
                                  case 0:
                                   {
                                       pkt *msgresend = check_and_cast<pkt*>(BackupQueue[BackupQueueIndex].pop());
                                       pkt *msgresend_copy = msgresend->dup();

                                   	   BackupQueue[BackupQueueIndex].insert(msgresend);

                                   	   if(msgresend_copy->getFlag_app()){
                                   	       app_pkt_num++;
                                   	   }
                                       int countNow = msgresend_copy->getHopCountNow();
                                       pkt_optical_temp_send->setNextToR( msgresend_copy->getHop(countNow + 2));
                                       pkt_optical_temp_send->setPreviousToR(msgresend_copy->getHop(countNow ));
                                       pkt_optical_temp_send->setTxIndex(msgresend_copy->getTxIndex());
                                       pkt_optical_temp_send->setGroupToRIndex( msgresend_copy->getGroupToRIndex() );
                                       msgresend_copy->setHopCountNow(countNow + 1);


                                       pkt_optical_temp_send->setPkt1(msgresend_copy);
                                       pkt_optical_temp_send->insertMessage(msgresend_copy);
                                       pkt_optical_temp_send->setOptical_num(1);
                                   }
                                       break;
                                   case 1:
                                       {
                                       pkt *msgresend1 = check_and_cast<pkt*>(BackupQueue[BackupQueueIndex].pop());
                                       pkt *msgresend1_copy = msgresend1->dup();

                                   	   BackupQueue[BackupQueueIndex].insert(msgresend1);
                                       if(msgresend1_copy->getFlag_app()){
                                           app_pkt_num++;
                                       }
                                       int countNow1 = msgresend1_copy->getHopCountNow();
                                       pkt_optical_temp_send->setNextToR( msgresend1_copy->getHop(countNow1 + 2));
                                       pkt_optical_temp_send->setPreviousToR(msgresend1_copy->getHop(countNow1 ));
                                       pkt_optical_temp_send->setTxIndex(msgresend1_copy->getTxIndex());
                                       pkt_optical_temp_send->setGroupToRIndex( msgresend1_copy->getGroupToRIndex() );
                                       msgresend1_copy->setHopCountNow(countNow1 + 1);



                                       pkt_optical_temp_send->setPkt2(msgresend1_copy);
                                       pkt_optical_temp_send->insertMessage(msgresend1_copy);
                                       pkt_optical_temp_send->setOptical_num(2);
                                       }
                                       break;
                                   case 2:
                                   {
                                       pkt *msgresend2 = check_and_cast<pkt*>(BackupQueue[BackupQueueIndex].pop());
                                       pkt *msgresend2_copy = msgresend2->dup();

                                       BackupQueue[BackupQueueIndex].insert(msgresend2);
                                       if(msgresend2_copy->getFlag_app()){
                                           app_pkt_num++;
                                       }
                                       int countNow2 = msgresend2_copy->getHopCountNow();
                                       pkt_optical_temp_send->setNextToR( msgresend2_copy->getHop(countNow2 + 2));
                                       pkt_optical_temp_send->setPreviousToR(msgresend2_copy->getHop(countNow2 ));
                                       pkt_optical_temp_send->setTxIndex(msgresend2_copy->getTxIndex());
                                       pkt_optical_temp_send->setGroupToRIndex( msgresend2_copy->getGroupToRIndex() );
                                       msgresend2_copy->setHopCountNow(countNow2 + 1);

                                       pkt_optical_temp_send->setPkt3(msgresend2_copy);
                                       pkt_optical_temp_send->insertMessage(msgresend2_copy);
                                       pkt_optical_temp_send->setOptical_num(3);
                                   }
                                       break;
                                   case 3:
                                   {
                                       pkt *msgresend3 = check_and_cast<pkt*>(BackupQueue[BackupQueueIndex].pop());
                                       pkt *msgresend3_copy = msgresend3->dup();

                                       BackupQueue[BackupQueueIndex].insert(msgresend3);
                                       if(msgresend3_copy->getFlag_app()){
                                           app_pkt_num++;
                                       }
                                       int countNow3 = msgresend3_copy->getHopCountNow();
                                       pkt_optical_temp_send->setNextToR( msgresend3_copy->getHop(countNow3 + 2));
                                       pkt_optical_temp_send->setPreviousToR(msgresend3_copy->getHop(countNow3 ));
                                       pkt_optical_temp_send->setTxIndex(msgresend3_copy->getTxIndex());
                                       pkt_optical_temp_send->setGroupToRIndex( msgresend3_copy->getGroupToRIndex() );
                                       msgresend3_copy->setHopCountNow(countNow3 + 1);

                                       pkt_optical_temp_send->setPkt4(msgresend3_copy);
                                       pkt_optical_temp_send->insertMessage(msgresend3_copy);
                                       pkt_optical_temp_send->setOptical_num(4);
                                   }
                                       break;
                                   case 4:
                                   {
                                       pkt *msgresend4 = check_and_cast<pkt*>(BackupQueue[BackupQueueIndex].pop());
                                       pkt *msgresend4_copy = msgresend4->dup();

                                       BackupQueue[BackupQueueIndex].insert(msgresend4);
                                       if(msgresend4_copy->getFlag_app()){
                                           app_pkt_num++;
                                       }
                                       int countNow4 = msgresend4_copy->getHopCountNow();
                                       pkt_optical_temp_send->setNextToR( msgresend4_copy->getHop(countNow4 + 2));
                                       pkt_optical_temp_send->setPreviousToR(msgresend4_copy->getHop(countNow4 ));
                                       pkt_optical_temp_send->setTxIndex(msgresend4_copy->getTxIndex());
                                       pkt_optical_temp_send->setGroupToRIndex( msgresend4_copy->getGroupToRIndex() );
                                       msgresend4_copy->setHopCountNow(countNow4 + 1);

                                       pkt_optical_temp_send->setPkt5(msgresend4_copy);
                                       pkt_optical_temp_send->insertMessage(msgresend4_copy);
                                       pkt_optical_temp_send->setOptical_num(5);
                                   }
                                       break;
                               }
                            }
                    }

                    TXSend++;
                    QueueArrayflag = false;
                    pkt_optical_temp_send->setApp_pkt_num(app_pkt_num);
                    send(pkt_optical_temp_send,"out");
//                    EV<<"Send Backup Queue"<<"\n"<<"Agre_Queue2OPS "<<getIndex()<<"  Buffer size is"<<Queue2OPS.length()<<"\n" ;
                }
                else if(QueueArrayflag == false){
                    app_pkt_num = 0;
                    if(maxQueuelength > 0){ 
	                    Optical *pkt_optical_temp_send = new Optical();
	                    pkt_optical_temp_send->setKind(1000);
	                    for(int i=0;i<OpticalPacketNum;i++){
	                            if(Queue2OPS[maxQueueIndex].length()>0){
	                               switch(i){
	                                   case 0:
	                                   {
	                                       pkt *pkt_temp_send = check_and_cast<pkt*>(Queue2OPS[maxQueueIndex].pop());
	                                       int countNow0f = pkt_temp_send->getHopCountNow();

	                                       pkt *pkt_temp_copy = pkt_temp_send->dup();
	                                       BackupQueue[maxQueueIndex].insert(pkt_temp_copy);

	                                       if(pkt_temp_send->getFlag_app()){
	                                           app_pkt_num++;
	                                       }
	                                       pkt_optical_temp_send->setNextToR( pkt_temp_send->getHop(countNow0f + 2));
	                                       pkt_optical_temp_send->setPreviousToR(pkt_temp_send->getHop(countNow0f ));
	                                       pkt_optical_temp_send->setTxIndex(pkt_temp_send->getTxIndex());
	                                       pkt_optical_temp_send->setGroupToRIndex( pkt_temp_send->getGroupToRIndex() );


	                                       pkt_temp_send->setHopCountNow(countNow0f + 1);

	                                       pkt_optical_temp_send->setPkt1(pkt_temp_send);
	                                       pkt_optical_temp_send->insertMessage(pkt_temp_send);
	                                       pkt_optical_temp_send->setOptical_num(1);
	                                   }
	                                       break;
	                                   case 1:
	                                   {
	                                       pkt *pkt_temp1_send = check_and_cast<pkt*>(Queue2OPS[maxQueueIndex].pop());
	                                       int countNow1f = pkt_temp1_send->getHopCountNow();

	                                       pkt *pkt_temp1_copy = pkt_temp1_send->dup();
	                                       BackupQueue[maxQueueIndex].insert(pkt_temp1_copy);

	                                       if(pkt_temp1_send->getFlag_app()){
	                                           app_pkt_num++;
	                                       }
	                                       pkt_optical_temp_send->setNextToR( pkt_temp1_send->getHop(countNow1f + 2));
	                                       pkt_optical_temp_send->setPreviousToR(pkt_temp1_send->getHop(countNow1f ));
	                                       pkt_optical_temp_send->setTxIndex(pkt_temp1_send->getTxIndex());
	                                       pkt_optical_temp_send->setGroupToRIndex( pkt_temp1_send->getGroupToRIndex() );

	                                       pkt_temp1_send->setHopCountNow(countNow1f + 1);
	                                       pkt_optical_temp_send->setPkt2(pkt_temp1_send);
	                                       pkt_optical_temp_send->insertMessage(pkt_temp1_send);
	                                       pkt_optical_temp_send->setOptical_num(2);
	                                   }
	                                       break;
	                                   case 2:
	                                   {
	                                       pkt *pkt_temp2_send = check_and_cast<pkt*>(Queue2OPS[maxQueueIndex].pop());
	                                       int countNow2f = pkt_temp2_send->getHopCountNow();

	                                       pkt *pkt_temp2_copy = pkt_temp2_send->dup();
	                                       BackupQueue[maxQueueIndex].insert(pkt_temp2_copy);

	                                       if(pkt_temp2_send->getFlag_app()){
                                               app_pkt_num++;
                                           }

	                                       pkt_optical_temp_send->setNextToR( pkt_temp2_send->getHop(countNow2f + 2));
	                                       pkt_optical_temp_send->setPreviousToR(pkt_temp2_send->getHop(countNow2f ));
	                                       pkt_optical_temp_send->setTxIndex(pkt_temp2_send->getTxIndex());
	                                       pkt_optical_temp_send->setGroupToRIndex( pkt_temp2_send->getGroupToRIndex() );

	                                       pkt_temp2_send->setHopCountNow(countNow2f + 1);
	                                       pkt_optical_temp_send->setPkt3(pkt_temp2_send);
	                                       pkt_optical_temp_send->insertMessage(pkt_temp2_send);
	                                       pkt_optical_temp_send->setOptical_num(3);
	                                   }
	                                       break;
	                                   case 3:
	                                   {
	                                       pkt *pkt_temp3_send = check_and_cast<pkt*>(Queue2OPS[maxQueueIndex].pop());
	                                       int countNow3f = pkt_temp3_send->getHopCountNow();

	                                       pkt *pkt_temp3_copy = pkt_temp3_send->dup();
	                                       BackupQueue[maxQueueIndex].insert(pkt_temp3_copy);

	                                       if(pkt_temp3_send->getFlag_app()){
                                               app_pkt_num++;
                                           }

	                                       pkt_optical_temp_send->setNextToR( pkt_temp3_send->getHop(countNow3f + 2));
	                                       pkt_optical_temp_send->setPreviousToR(pkt_temp3_send->getHop(countNow3f ));
	                                       pkt_optical_temp_send->setTxIndex(pkt_temp3_send->getTxIndex());
	                                       pkt_optical_temp_send->setGroupToRIndex( pkt_temp3_send->getGroupToRIndex() );

	                                       pkt_temp3_send->setHopCountNow(countNow3f + 1);
	                                       pkt_optical_temp_send->setPkt4(pkt_temp3_send);
	                                       pkt_optical_temp_send->insertMessage(pkt_temp3_send);
	                                       pkt_optical_temp_send->setOptical_num(4);
	                                   }
	                                       break;
	                                   case 4:
	                                   {
	                                       pkt *pkt_temp4_send = check_and_cast<pkt*>(Queue2OPS[maxQueueIndex].pop());
	                                       int countNow4f = pkt_temp4_send->getHopCountNow();

	                                       pkt *pkt_temp4_copy = pkt_temp4_send->dup();
	                                       BackupQueue[maxQueueIndex].insert(pkt_temp4_copy);

	                                       if(pkt_temp4_send->getFlag_app()){
                                               app_pkt_num++;
                                           }
	                                       pkt_optical_temp_send->setNextToR( pkt_temp4_send->getHop(countNow4f + 2));
	                                       pkt_optical_temp_send->setPreviousToR(pkt_temp4_send->getHop(countNow4f ));
	                                       pkt_optical_temp_send->setTxIndex(pkt_temp4_send->getTxIndex());
	                                       pkt_optical_temp_send->setGroupToRIndex( pkt_temp4_send->getGroupToRIndex() );

	                                       pkt_temp4_send->setHopCountNow(countNow4f + 1);
	                                       pkt_optical_temp_send->setPkt5(pkt_temp4_send);
	                                       pkt_optical_temp_send->insertMessage(pkt_temp4_send);
	                                       pkt_optical_temp_send->setOptical_num(5);
	                                   }
	                                       break;
	                               }
	                            }
	                    }

	                    TXSend++;
						gate("out")->getIndex();
						pkt_optical_temp_send->setApp_pkt_num(app_pkt_num);
	                    send(pkt_optical_temp_send,"out");
	                }
			}
            scheduleAt(simTime() + slot, local_clock);
            SimulatedClock ++;
        }
        else
        {
    //        EV<<"I am run out of msg, delete local_clock"<<"\n";
            cancelEvent(local_clock);
        }


    }
    else if(pkt_temp->getIndex() == 5){ // finish msg from headprocess
        finishflag = true;
//        EV<<"finishflag now is"<<finishflag<<"\n";
        delete msg;
    }
    else if(pkt_temp->getIndex() == 8){ // the ACK

            if(pkt_temp->getRetransmission() == 0){
                int Num = pkt_temp->getOptNum();
                int TORIndex = pkt_temp->getGroupToRIndex();
                for(int i=0;i<Num;i++){
                    if(BackupQueue[TORIndex].length()>0)
                        delete BackupQueue[TORIndex].pop();
                }
            }
            else{
                DirectTrans = DirectTrans + pkt_temp->getApp_pkt_num();
                QueueArrayflag = true;
                BackupQueueIndex = pkt_temp->getGroupToRIndex();
                BackupQueueopticalNum = pkt_temp->getOptNum();
//                EV<<"ACK = 1"<<"\n"<<"Agre_Queue2OPS"<<getIndex()<<"  Buffer size is"<<Queue2OPS.length()<<"\n" ;
//                pkt *msgresend = check_and_cast<pkt*>(BackupQueue.pop());
//                BackupQueue.insert(msgresend);
            }
            delete msg;
    }
    else if(pkt_temp->getIndex() == 10){
            if(pkt_temp->getRetransmission() == 0){
                int Num = pkt_temp->getOptNum();
                int TORIndex = pkt_temp->getGroupToRIndex();
                for(int i=0;i<Num;i++){
                    if(BackupQueue[TORIndex].length()>0)
                        delete BackupQueue[TORIndex].pop();
                }
            }
            else{
                DirectTrans = DirectTrans + pkt_temp->getApp_pkt_num();
                QueueArrayflag = true;
                BackupQueueIndex = pkt_temp->getGroupToRIndex();
                BackupQueueopticalNum = pkt_temp->getOptNum();
//                pkt *msgresend = check_and_cast<pkt*>(BackupQueue.pop());
//                BackupQueue.insert(msgresend);
            }
            delete msg;
    }
}

void TxBuffer::finish()
{
    recordScalar("Packet-lost-----Queue2OPS", Queue2OPSPacketlost);
    recordScalar("Cell-lost-From--Queue2OPS", Queue2OPSlost);
    recordScalar("Retrans---------------num",DirectTrans);
    recordScalar("Occupy----------------num",OccupyMax);
}





