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
int count2ToR[73728];
cQueue Queue2Server[73728];
bool Queue2Server_busy[73728];
int Queue2Serverlost[73728];
bool msgfromserver2headprocessorfinish[73728];

class HeadProcessor : public cSimpleModule
{
    private:
        simtime_t HeadProcessorfinishtime;
        int totalserverNum;
        int ToRserverNum;
        int ClusterToRNum;
        int AllToRNum;
        int GroupToR ;
        int AgreRadix;
        int CoreRadix;

        int Queue2ServerAveragelost;

        simtime_t Networktime_latency;
        simtime_t ToR2ToR_Latency;
        simtime_t finishtime_Agre;
        simtime_t finishtime_Core;
        simtime_t finishtime;

        int Down2ServerBufferSize;

        double slot;
        double slot_ToR;

        int NumTx;
		int NumTx_ToR2ToR;

		int GroupToRCore;

        int TxIndex;
        int GroupToRIndex;
        int numReceived_Total;
        int numReceived_ToR;
        int PnumReceived_ToR;
        int FinishserverNum;
        int coutserver;

        double DelayOfProcessor;

        double TxBufferTime;
        double PreambleInsertdelay;
        double BurstProcessDelay;

        simtime_t AverageNetworkLatency;
        simtime_t PAverageNetworkLatency;
        simtime_t PToR2ToR_Latency;

        simtime_t interval_0;

        double UplinkdataRate;
        double DownlinkDataRate;
        int CellSize;
        int CellNum;

        int tt0;
        int receivedPackets;
        int xx;
        int caseofcellsize;

        bool FlagToRFlowControl;

        pkt * local_pkt_ToR;
    protected:

         virtual void initialize();
         virtual void handleMessage(cMessage *msg);
         virtual void finish();
    public:
         HeadProcessor();
         virtual ~HeadProcessor();
};

Define_Module(HeadProcessor);

HeadProcessor::HeadProcessor(){
    local_pkt_ToR = NULL;

}

HeadProcessor::~HeadProcessor(){

 //   cancelAndDelete(local_pkt_ToR);
    cancelAndDelete(local_pkt_ToR);
    if(getIndex()==0){
        for(int i=0;i<73728;i++){
            while(!Queue2Server[i].empty())
                delete (pkt *)Queue2Server[i].pop();
        }
    }
}
void HeadProcessor::initialize()
{
    xx = 0;
    GroupToRIndex = 0;
    HeadProcessorfinishtime = 0;
    CellSize = par("CellSize");
    totalserverNum = par("ServerNum");
    ToRserverNum = par("ToRDownRadix");
    AllToRNum = totalserverNum/ToRserverNum;
//    ClusterToRNum = sqrt(AllToRNum);
    ClusterToRNum = par("AgreRadix");
    GroupToR = par("GroupToR");
    AgreRadix = par("AgreRadix");
    CoreRadix = par("CoreRadix");
    NumTx = par("NumTx");
    NumTx_ToR2ToR = par("NumTx_ToR2ToR");
    GroupToRCore = par("GroupToRCore");
    Down2ServerBufferSize = par("Down2ServerBufferSize");
    UplinkdataRate = par("dataRate");
    DownlinkDataRate = par("linkDataRate");

    if(getIndex()==0){
        for(int i=0;i<73728;i++){
            msgfromserver2headprocessorfinish[i] = false;
            count2ToR[i] = 0;
            Queue2Server_busy[i]=0;
            Queue2Serverlost[i]=0;
        }
        FinishserverNum = 0;
    }

    receivedPackets = 0;
    HeadProcessorfinishtime = 0;
    numReceived_Total = 0;
    numReceived_ToR = 0;
    PnumReceived_ToR = 0;
    Networktime_latency = 0;
    ToR2ToR_Latency = 0;

    AverageNetworkLatency = 0;
    PAverageNetworkLatency = 0;
    coutserver = 0;


 //   slot = 1.2e-6;
    slot_ToR = 2.4e-7;

    DelayOfProcessor = 80e-9;

    Queue2ServerAveragelost = 0;

    caseofcellsize = 0;

    CellNum = 0;
    PreambleInsertdelay = par("PreambleInsertdelay");
    BurstProcessDelay = par("BurstProcessDelay");
    TxBufferTime = CellSize*8/DownlinkDataRate;
    finishtime = 0;
    finishtime_Agre = 0;
    finishtime_Core = 0;
}


void HeadProcessor::handleMessage(cMessage *msg)
{

    if(msg->getKind() == 1000){

        Optical *Optical_pkt_temp =  check_and_cast<Optical*>(msg);

        int xoptnum1 = Optical_pkt_temp->getOptical_num();
        cModule *targetModule = getParentModule();
        int tt = targetModule->getIndex();
        ///////////////////////////////////////
        //cModule *targetModule = getParentModule();
        //int tt = getIndex()
        //////////////////////////////////////////////////////////////////////////////////

        pkt *pkt_temp0, *pkt_temp1, *pkt_temp2, *pkt_temp3, *pkt_temp4;
        //int countNow = 0;
        //countNow =  Optical_pkt_temp->getPkt1()->getHopCountNow();
     /*   int xGroupToRIndex = pkt_temp->getGroupToRIndex();
        int xTXIndex = pkt_temp->getTxIndex() ;
        int xBufferindex = xTXIndex*NumTx+xGroupToRIndex;*/


        for(int i=0;i<xoptnum1;i++){
            /*if(i==1){
                Optical_pkt_temp->removeMessage(Optical_pkt_temp->getPkt2());
                pkt_temp1 = check_and_cast<pkt*>(Optical_pkt_temp->getPkt2());
                delete pkt_temp1;
                break;
            }*/
            switch(i){
                 case 0:
                     Optical_pkt_temp->removeMessage(Optical_pkt_temp->getPkt1());
                     pkt_temp0 =  check_and_cast<pkt*>(Optical_pkt_temp->getPkt1());

                     if(  ( pkt_temp0->getHop(pkt_temp0->getHopCountNow() + 1)  >= AllToRNum ) && ( pkt_temp0->getHop(pkt_temp0->getHopCountNow() + 1 ) <  AllToRNum +  ClusterToRNum )  ){
                             int Next_ToR = pkt_temp0->getHop(pkt_temp0->getHopCountNow() + 2);
                             int TxIndex_0 = (Next_ToR % AgreRadix)/GroupToR;
                             GroupToRIndex = (Next_ToR % AgreRadix)%GroupToR;
                             pkt_temp0->setTxIndex(TxIndex_0);
                             pkt_temp0->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp0,DelayOfProcessor,"bufferout",TxIndex_0);
                             //finishtime_Agre = gate("bufferout",TxIndex_0)->getTransmissionChannel()->getTransmissionFinishTime();
                     }
                     else if(   (pkt_temp0->getHop(pkt_temp0->getHopCountNow() + 1)  >= AllToRNum + ClusterToRNum ) && ( pkt_temp0->getHop(pkt_temp0->getHopCountNow() + 1 ) <  AllToRNum + 2* ClusterToRNum )   ){
                         int Next_ToR = pkt_temp0->getHop(pkt_temp0->getHopCountNow() + 2);
                         int TxIndex_0 =  (Next_ToR / CoreRadix)/GroupToRCore;       // go to the direct connected ToR
                             pkt_temp0->setTxIndex(TxIndex_0);
                             GroupToRIndex = (Next_ToR / CoreRadix)%GroupToRCore;
                             pkt_temp0->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp0,DelayOfProcessor,"bufferout",NumTx+TxIndex_0);
                             //finishtime_Core = gate("bufferout",NumTx+TxIndex_0)->getTransmissionChannel()->getTransmissionFinishTime();
                     }
                     else if(pkt_temp0->getHop(pkt_temp0->getHopCountNow() + 1)  == -100){
                         if(pkt_temp0->getFlag_last() == 1){
                             PnumReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp0->getTimestamp();
                             PToR2ToR_Latency = PToR2ToR_Latency + Networktime_latency;
                             //PAverageNetworkLatency = ToR2ToR_Latency/PnumReceived_ToR;
                         }
                             numReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp0->getTimestamp();
                             ToR2ToR_Latency =  ToR2ToR_Latency + Networktime_latency;
                             Networktime_latency = 0;
                             //AverageNetworkLatency = ToR2ToR_Latency/numReceived_ToR;
                             numReceived_Total++;
                         int DestServer = pkt_temp0->getDst_Server();
                         if(Queue2Server_busy[DestServer] == false){
                             Queue2Server_busy[DestServer] = true;
                             sendDelayed(pkt_temp0,DelayOfProcessor,"serverout",DestServer%ToRserverNum);
                             finishtime = gate("serverout",DestServer%ToRserverNum)->getTransmissionChannel()->getTransmissionFinishTime();

                             pkt *pkt2l = new pkt("local_pkt_ToR");
                             pkt2l->setDst_Server(DestServer);
                             pkt2l->setIndex(2);
                             pkt2l->setSchedulingPriority(0);
                             scheduleAt(simTime()+slot_ToR + DelayOfProcessor,pkt2l);
                             pkt_temp0->setFlag_intraToR(1);
                         }
                         else{
                             int temporyOccupy = 0;
                             for(int i = (DestServer/ToRserverNum)*ToRserverNum;i< (DestServer/ToRserverNum)*ToRserverNum + ToRserverNum ;i++){
                                 temporyOccupy +=  Queue2Server[i].length();
                             }
                             if(temporyOccupy >= Down2ServerBufferSize*40 ){
                                 Queue2Serverlost[DestServer] = Queue2Serverlost[DestServer] + 1;
                                 delete pkt_temp0;
                             }
                             else{

                                 pkt_temp0->setPacketSize(1500);
                                 Queue2Server[DestServer].insert(pkt_temp0);

                             }
                         }
                     }
                     break;
                 case 1:
                     Optical_pkt_temp->removeMessage(Optical_pkt_temp->getPkt2());
                     pkt_temp1 =  check_and_cast<pkt*>(Optical_pkt_temp->getPkt2());
                     if(  ( pkt_temp1->getHop(pkt_temp1->getHopCountNow() + 1)  >= AllToRNum ) && ( pkt_temp1->getHop(pkt_temp1->getHopCountNow() + 1 ) <  AllToRNum +  ClusterToRNum )  ){
                             int Next_ToR = pkt_temp1->getHop(pkt_temp1->getHopCountNow() + 2);
                             int TxIndex_1 = (Next_ToR % AgreRadix)/GroupToR;
                             GroupToRIndex = (Next_ToR % AgreRadix)%GroupToR;
                             pkt_temp1->setTxIndex(TxIndex_1);
                             pkt_temp1->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp1, DelayOfProcessor,"bufferout",TxIndex_1);
                     }
                     else if(   (pkt_temp1->getHop(pkt_temp1->getHopCountNow() + 1)  >= AllToRNum + ClusterToRNum ) && ( pkt_temp1->getHop(pkt_temp1->getHopCountNow() + 1 ) <  AllToRNum + 2* ClusterToRNum )   ){
                         int Next_ToR = pkt_temp1->getHop(pkt_temp1->getHopCountNow() + 2);
                         int TxIndex_1 =  (Next_ToR / CoreRadix)/GroupToRCore;       // go to the direct connected ToR
                             pkt_temp1->setTxIndex(TxIndex_1);
                             GroupToRIndex = (Next_ToR / CoreRadix)%GroupToRCore;
                             pkt_temp1->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp1,DelayOfProcessor,"bufferout",NumTx+TxIndex_1);
                             //finishtime_Core = gate("bufferout",NumTx+TxIndex_0)->getTransmissionChannel()->getTransmissionFinishTime();
                     }
                     else if(pkt_temp1->getHop(pkt_temp1->getHopCountNow() + 1)  == -100){

                         if(pkt_temp1->getFlag_last() == 1){
                             PnumReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp1->getTimestamp();
                             PToR2ToR_Latency = PToR2ToR_Latency + Networktime_latency;
                             //PAverageNetworkLatency = ToR2ToR_Latency/PnumReceived_ToR;
                         }
                             numReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp1->getTimestamp();
                             ToR2ToR_Latency =  ToR2ToR_Latency + Networktime_latency;
                             Networktime_latency = 0;
                             //AverageNetworkLatency = ToR2ToR_Latency/numReceived_ToR;
                             numReceived_Total++;
                         int DestServer = pkt_temp1->getDst_Server();
                         if(Queue2Server_busy[DestServer] == false){
                             Queue2Server_busy[DestServer] = true;
                             sendDelayed(pkt_temp1,DelayOfProcessor,"serverout",DestServer%ToRserverNum);
                             //finishtime = gate("serverout",DestServer%ToRserverNum)->getTransmissionChannel()->getTransmissionFinishTime();

                             pkt *pkt2l1 = new pkt("local_pkt_ToR");
                             pkt2l1->setDst_Server(DestServer);
                             pkt2l1->setIndex(2);
                             pkt2l1->setSchedulingPriority(0);
                             scheduleAt(simTime()+slot_ToR + DelayOfProcessor,pkt2l1);
                             pkt_temp1->setFlag_intraToR(1);
                         }
                         else{
                             int temporyOccupy = 0;
                             for(int i = (DestServer/ToRserverNum)*ToRserverNum;i< (DestServer/ToRserverNum)*ToRserverNum + ToRserverNum ;i++){
                                 temporyOccupy +=  Queue2Server[i].length();
                             }
                             if(temporyOccupy >= Down2ServerBufferSize*40 ){
                                 Queue2Serverlost[DestServer] = Queue2Serverlost[DestServer] + 1;
                                 delete pkt_temp1;
                             }
                             else{

                                 pkt_temp1->setPacketSize(1500);

                                 Queue2Server[DestServer].insert(pkt_temp1);

                             }
                         }
                     }
                     break;
                 case 2:
                     Optical_pkt_temp->removeMessage(Optical_pkt_temp->getPkt3());
                     pkt_temp2 =  check_and_cast<pkt*>(Optical_pkt_temp->getPkt3());
                     if(  ( pkt_temp2->getHop(pkt_temp2->getHopCountNow() + 1)  >= AllToRNum ) && ( pkt_temp2->getHop(pkt_temp2->getHopCountNow() + 1 ) <  AllToRNum +  ClusterToRNum )  ){
                             int Next_ToR = pkt_temp2->getHop(pkt_temp2->getHopCountNow() + 2);
                             int TxIndex_2 = (Next_ToR % AgreRadix)/GroupToR;
                             GroupToRIndex = (Next_ToR % AgreRadix)%GroupToR;
                             pkt_temp2->setTxIndex(TxIndex_2);
                             pkt_temp2->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp2,DelayOfProcessor,"bufferout",TxIndex_2);
                     }
					  else if(   (pkt_temp2->getHop(pkt_temp2->getHopCountNow() + 1)  >= AllToRNum + ClusterToRNum ) && ( pkt_temp2->getHop(pkt_temp2->getHopCountNow() + 1 ) <  AllToRNum + 2* ClusterToRNum )   ){
					      int Next_ToR = pkt_temp2->getHop(pkt_temp2->getHopCountNow() + 2);
					      int TxIndex_2 =  (Next_ToR / CoreRadix)/GroupToRCore;       // go to the direct connected ToR
                             pkt_temp2->setTxIndex(TxIndex_2);
                             GroupToRIndex = (Next_ToR / CoreRadix)%GroupToRCore;
                             pkt_temp2->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp2,DelayOfProcessor,"bufferout",NumTx+TxIndex_2);
                             //finishtime_Core = gate("bufferout",NumTx+TxIndex_0)->getTransmissionChannel()->getTransmissionFinishTime();
                     }
                     else if(pkt_temp2->getHop(pkt_temp2->getHopCountNow() + 1)  == -100){
                         if(pkt_temp2->getFlag_last() == 1){
                             PnumReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp2->getTimestamp();
                             PToR2ToR_Latency = PToR2ToR_Latency + Networktime_latency;
                             //PAverageNetworkLatency = ToR2ToR_Latency/PnumReceived_ToR;
                         }
                             numReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp2->getTimestamp();
                             ToR2ToR_Latency =  ToR2ToR_Latency + Networktime_latency;
                             Networktime_latency = 0;
                             //AverageNetworkLatency = ToR2ToR_Latency/numReceived_ToR;
                             numReceived_Total++;
                         int DestServer = pkt_temp2->getDst_Server();
                         if(Queue2Server_busy[DestServer] == false){
                             Queue2Server_busy[DestServer] = true;
                             sendDelayed(pkt_temp2,DelayOfProcessor,"serverout",DestServer%ToRserverNum);
                             finishtime = gate("serverout",DestServer%ToRserverNum)->getTransmissionChannel()->getTransmissionFinishTime();

                             pkt *pkt2l2 = new pkt("local_pkt_ToR");
                             pkt2l2->setDst_Server(DestServer);
                             pkt2l2->setIndex(2);
                             pkt2l2->setSchedulingPriority(0);
                             scheduleAt(simTime()+slot_ToR + DelayOfProcessor,pkt2l2);
                             pkt_temp2->setFlag_intraToR(1);
                         }
                         else{
                             int temporyOccupy = 0;
                             for(int i = (DestServer/ToRserverNum)*ToRserverNum;i< (DestServer/ToRserverNum)*ToRserverNum + ToRserverNum ;i++){
                                 temporyOccupy +=  Queue2Server[i].length();
                             }
                             if(temporyOccupy >= Down2ServerBufferSize*40 ){
                                 Queue2Serverlost[DestServer] = Queue2Serverlost[DestServer] + 1;
                                 delete pkt_temp2;
                             }
                             else{

                                 pkt_temp2->setPacketSize(1500);

                                 Queue2Server[DestServer].insert(pkt_temp2);

                             }
                         }
                     }
                     break;
                 case 3:
                     Optical_pkt_temp->removeMessage(Optical_pkt_temp->getPkt4());
                     pkt_temp3 =  check_and_cast<pkt*>(Optical_pkt_temp->getPkt4());
                     if(  ( pkt_temp3->getHop(pkt_temp3->getHopCountNow() + 1)  >= AllToRNum )  && ( pkt_temp3->getHop(pkt_temp3->getHopCountNow() + 1 ) <  AllToRNum +  ClusterToRNum )  ){
                             int Next_ToR = pkt_temp3->getHop(pkt_temp3->getHopCountNow() + 2);
                             int TxIndex_3 = (Next_ToR % AgreRadix)/GroupToR;
                             GroupToRIndex = (Next_ToR % AgreRadix)%GroupToR;
                             pkt_temp3->setTxIndex(TxIndex_3);
                             pkt_temp3->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp3,DelayOfProcessor,"bufferout",TxIndex_3);
                     }
                     else if(   (pkt_temp3->getHop(pkt_temp3->getHopCountNow() + 1)  >= AllToRNum + ClusterToRNum ) && ( pkt_temp3->getHop(pkt_temp3->getHopCountNow() + 1 ) <  AllToRNum + 2* ClusterToRNum )   ){
                         int Next_ToR = pkt_temp3->getHop(pkt_temp3->getHopCountNow() + 2);
                         int TxIndex_3 =  (Next_ToR / CoreRadix)/GroupToRCore;       // go to the direct connected ToR
                             pkt_temp3->setTxIndex(TxIndex_3);
                             GroupToRIndex = (Next_ToR / CoreRadix)%GroupToRCore;
                             pkt_temp3->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp3,DelayOfProcessor,"bufferout",NumTx+TxIndex_3);
                             //finishtime_Core = gate("bufferout",NumTx+TxIndex_0)->getTransmissionChannel()->getTransmissionFinishTime();
                     }

                     else if(pkt_temp3->getHop(pkt_temp3->getHopCountNow() + 1)  == -100){
                         if(pkt_temp3->getFlag_last() == 1){
                             PnumReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp3->getTimestamp();
                             PToR2ToR_Latency = PToR2ToR_Latency + Networktime_latency;
                             //PAverageNetworkLatency = ToR2ToR_Latency/PnumReceived_ToR;
                         }
                             numReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp3->getTimestamp();
                             ToR2ToR_Latency =  ToR2ToR_Latency + Networktime_latency;
                             Networktime_latency = 0;
                             //AverageNetworkLatency = ToR2ToR_Latency/numReceived_ToR;
                             numReceived_Total++;
                         int DestServer = pkt_temp3->getDst_Server();
                         if(Queue2Server_busy[DestServer] == false){
                             Queue2Server_busy[DestServer] = true;
                             sendDelayed(pkt_temp3,DelayOfProcessor,"serverout",DestServer%ToRserverNum);
                             finishtime = gate("serverout",DestServer%ToRserverNum)->getTransmissionChannel()->getTransmissionFinishTime();

                             pkt *pkt2l3 = new pkt("local_pkt_ToR");
                             pkt2l3->setDst_Server(DestServer);
                             pkt2l3->setIndex(2);
                             pkt2l3->setSchedulingPriority(0);
                             scheduleAt(simTime()+slot_ToR + DelayOfProcessor,pkt2l3);
                             pkt_temp3->setFlag_intraToR(1);
                         }
                         else{
                             int temporyOccupy = 0;
                             for(int i = (DestServer/ToRserverNum)*ToRserverNum;i< (DestServer/ToRserverNum)*ToRserverNum + ToRserverNum ;i++){
                                 temporyOccupy +=  Queue2Server[i].length();
                             }
                             if(temporyOccupy >= Down2ServerBufferSize*40 ){
                                 Queue2Serverlost[DestServer] = Queue2Serverlost[DestServer] + 1;
                                 delete pkt_temp3;
                             }
                             else{

                                 pkt_temp3->setPacketSize(1500);

                                 Queue2Server[DestServer].insert(pkt_temp3);

                             }
                         }
                     }
                     break;
                 case 4:
                     Optical_pkt_temp->removeMessage(Optical_pkt_temp->getPkt5());
                     pkt_temp4 =  check_and_cast<pkt*>(Optical_pkt_temp->getPkt5());
                     if(  ( pkt_temp4->getHop(pkt_temp4->getHopCountNow() + 1)  >= AllToRNum ) && ( pkt_temp4->getHop(pkt_temp4->getHopCountNow() + 1 ) <  AllToRNum +  ClusterToRNum )  ){
                             int Next_ToR = pkt_temp4->getHop(pkt_temp4->getHopCountNow() + 2);
                             int TxIndex_4 = (Next_ToR % AgreRadix)/GroupToR;
                             GroupToRIndex = (Next_ToR % AgreRadix)%GroupToR;
                             pkt_temp4->setTxIndex(TxIndex_4);
                             pkt_temp4->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp4,DelayOfProcessor,"bufferout",TxIndex_4);
                     }
                     else if(   (pkt_temp4->getHop(pkt_temp4->getHopCountNow() + 1)  >= AllToRNum + ClusterToRNum ) && ( pkt_temp4->getHop(pkt_temp4->getHopCountNow() + 1 ) <  AllToRNum + 2* ClusterToRNum )   ){
                         int Next_ToR = pkt_temp4->getHop(pkt_temp4->getHopCountNow() + 2);
                         int TxIndex_4 =  (Next_ToR / CoreRadix)/GroupToRCore;       // go to the direct connected ToR
                             pkt_temp4->setTxIndex(TxIndex_4);
                             GroupToRIndex = (Next_ToR / CoreRadix)%GroupToRCore;
                             pkt_temp4->setGroupToRIndex(GroupToRIndex);
                             sendDelayed(pkt_temp4,DelayOfProcessor,"bufferout",NumTx+TxIndex_4);
                             //finishtime_Core = gate("bufferout",NumTx+TxIndex_0)->getTransmissionChannel()->getTransmissionFinishTime();
                     }

                     else if(pkt_temp4->getHop(pkt_temp4->getHopCountNow() + 1)  == -100){
                         if(pkt_temp4->getFlag_last() == 1){
                             PnumReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp4->getTimestamp();
                             PToR2ToR_Latency = PToR2ToR_Latency + Networktime_latency;
                             //PAverageNetworkLatency = ToR2ToR_Latency/PnumReceived_ToR;
                         }
                             numReceived_ToR++;
                             Networktime_latency = simTime() - pkt_temp4->getTimestamp();
                             ToR2ToR_Latency =  ToR2ToR_Latency + Networktime_latency;
                             Networktime_latency = 0;
                             //AverageNetworkLatency = ToR2ToR_Latency/numReceived_ToR;
                             numReceived_Total++;
                         int DestServer = pkt_temp4->getDst_Server();
                         if(Queue2Server_busy[DestServer] == false){
                             Queue2Server_busy[DestServer] = true;
                             sendDelayed(pkt_temp4,DelayOfProcessor,"serverout",DestServer%ToRserverNum);
                             finishtime = gate("serverout",DestServer%ToRserverNum)->getTransmissionChannel()->getTransmissionFinishTime();

                             pkt *pkt2l4 = new pkt("local_pkt_ToR");
                             pkt2l4->setDst_Server(DestServer);
                             pkt2l4->setIndex(2);
                             pkt2l4->setSchedulingPriority(0);
                             scheduleAt(simTime()+slot_ToR + DelayOfProcessor,pkt2l4);
                             pkt_temp4->setFlag_intraToR(1);
                         }
                         else{
                             int temporyOccupy = 0;
                             for(int i = (DestServer/ToRserverNum)*ToRserverNum;i< (DestServer/ToRserverNum)*ToRserverNum + ToRserverNum ;i++){
                                 temporyOccupy +=  Queue2Server[i].length();
                             }
                             if(temporyOccupy >= Down2ServerBufferSize*40 ){
                                 Queue2Serverlost[DestServer] = Queue2Serverlost[DestServer] + 1;
                                 delete pkt_temp4;
                             }
                             else{

                                 pkt_temp4->setPacketSize(1500);
                                 Queue2Server[DestServer].insert(pkt_temp4);
                             }
                         }
                     }
                     break;
            }
        }
        delete msg;

    }
	else{
	    xx++;
			    pkt *pkt_temp =  check_and_cast<pkt*>(msg);

                if(pkt_temp->getSrc_Server() == 1  && pkt_temp->getDst_Server() == 0){
                     int axd = 1;
                }


			    if(pkt_temp->getIndex() == 1){

			            if(pkt_temp->getHopCountNow() == 0){
			                  receivedPackets++;
			                        int DstToR = pkt_temp->getDst_ToR();
			                        caseofcellsize = pkt_temp->getPacketSize()/1500 + 1;
			                        if(pkt_temp->getPacketSize()%1500==0)
			                              caseofcellsize -=1;

			                        count2ToR[DstToR] += caseofcellsize;

			            }
		                EV<< "HP received a packet from" <<pkt_temp->getSrc_Server() <<" to "<<pkt_temp->getDst_Server()<<"\n";
		                EV<<"next hop is"<<pkt_temp->getHop(pkt_temp->getHopCountNow() + 1)<<"\n";

			        if(  ( pkt_temp->getHop(pkt_temp->getHopCountNow() + 1)  >= AllToRNum ) && ( pkt_temp->getHop(pkt_temp->getHopCountNow() + 1 ) < (AllToRNum + ClusterToRNum ) ) ){
			           //     if(pkt_temp->getHopCountNow() == 1)
			//                 msg->setTimestamp(simTime());
			//                int CurrentToR = pkt_temp->getHop(pkt_temp->getHopCountNow());
			                int Next_ToR = pkt_temp->getHop(pkt_temp->getHopCountNow() + 2);
			                TxIndex = (Next_ToR % AgreRadix)/GroupToR;
                            GroupToRIndex = (Next_ToR % AgreRadix)%GroupToR;
			                pkt_temp->setTxIndex(TxIndex);
                            pkt_temp->setGroupToRIndex(GroupToRIndex);
			//                simtime_t delayOfSyn = (slot - simtime()%slot) %slot;
			                sendDelayed(pkt_temp,DelayOfProcessor,"bufferout",TxIndex);

			  //              EV<< "received a packet from" <<pkt_temp->getSrc_Server() <<" to "<<pkt_temp->getDst_Server()<<"\n";
			  //              EV<< "TxIndex is" <<TxIndex <<"\n";
			        }
			        else if(   (pkt_temp->getHop(pkt_temp->getHopCountNow() + 1)   >= AllToRNum + ClusterToRNum) && ( pkt_temp->getHop(pkt_temp->getHopCountNow() + 1 ) <  (AllToRNum + 2*ClusterToRNum) )  ){
			            //    if(pkt_temp->getHopCountNow() == 1)
			//                 msg->setTimestamp(simTime());
			//                int CurrentToR = pkt_temp->getHop(pkt_temp->getHopCountNow());
			//                int Next_ToR = pkt_temp->getHop(pkt_temp->getHopCountNow() + 2) ;
			            int Next_ToR = pkt_temp->getHop(pkt_temp->getHopCountNow() + 2);
			                TxIndex = (Next_ToR / CoreRadix)/GroupToRCore;   
			                pkt_temp->setTxIndex(TxIndex);
							
							GroupToRIndex = (Next_ToR / CoreRadix)%GroupToRCore;
                            pkt_temp->setGroupToRIndex(GroupToRIndex);
			//                simtime_t delayOfSyn_1 = (slot - simtime()%slot) %slot;
			                sendDelayed(pkt_temp,DelayOfProcessor,"bufferout",NumTx+TxIndex);
			//                EV<< "received a packet from" <<pkt_temp->getSrc_Server() <<" to "<<pkt_temp->getDst_Server()<<"\n";
			//                EV<< "TxIndex is" <<TxIndex <<"\n";
			        }
			        else if( pkt_temp->getHop(pkt_temp->getHopCountNow() + 1)  == -100){
                        if(pkt_temp->getHopCountNow() == 0  && pkt_temp->getFlag_last() == 1){
                            PnumReceived_ToR++;
                            Networktime_latency = simTime() - pkt_temp->getTimestamp();
                            PToR2ToR_Latency = PToR2ToR_Latency + Networktime_latency;
                            //PAverageNetworkLatency = ToR2ToR_Latency/PnumReceived_ToR;
                        }

			            if(pkt_temp->getHopCountNow() > 1){
			                numReceived_ToR++;
			 //               Networktime_latency = simTime() - pkt_temp->getTimestamp() + TxBufferTime + PreambleInsertdelay + BurstProcessDelay;
			                Networktime_latency = simTime() - pkt_temp->getTimestamp();

			                ToR2ToR_Latency =  ToR2ToR_Latency + Networktime_latency;
			                Networktime_latency = 0;
			                //AverageNetworkLatency = ToR2ToR_Latency/numReceived_ToR;
			                numReceived_Total++;
			            }
			            else{
			                caseofcellsize = pkt_temp->getPacketSize()/1500 + 1;
			                if(pkt_temp->getPacketSize()%1500==0)
			                      caseofcellsize -=1;
			                numReceived_Total = numReceived_Total + caseofcellsize;
			            }


			            if(pkt_temp->getHopCountNow() > 1){
			                CellNum =1;
			            }
			            else{
			                CellNum = pkt_temp->getPacketSize()/1500 + 1;
			                if(pkt_temp->getPacketSize()%1500==0)
			                    CellNum -=1;
			            }
			//            delete msg;
			            int DestServer = pkt_temp->getDst_Server();
			            EV<< "received a packet from" <<pkt_temp->getSrc_Server() <<" to "<<pkt_temp->getDst_Server()<<"\n";

			            if(Queue2Server_busy[DestServer] == false){

			                Queue2Server_busy[DestServer] = true;

	                        if(pkt_temp->getHopCountNow() == 0){
	                            pkt_temp->setFlag_last(1);
	                        }


			                sendDelayed(msg,DelayOfProcessor,"serverout",DestServer%ToRserverNum);
			                finishtime = gate("serverout",DestServer%ToRserverNum)->getTransmissionChannel()->getTransmissionFinishTime();

			                pkt *pkt2l = new pkt("local_pkt_ToR");
			                pkt2l->setDst_Server(DestServer);
			                pkt2l->setIndex(2);
			                pkt2l->setSchedulingPriority(0);
			                scheduleAt(simTime()+slot_ToR*CellNum + DelayOfProcessor,pkt2l);
			                pkt_temp->setFlag_intraToR(CellNum);
			            }
			            else{


	                        int temporyOccupy = 0;
	                        int startServer = (DestServer/ToRserverNum)*ToRserverNum;
	                        int endServer = (DestServer/ToRserverNum)*ToRserverNum + ToRserverNum;
                           /* if(xx==2){
                                 EV<<"startServer is "<<startServer<<"\n";
                                 EV<<" endServer is "<< endServer<<"\n";
                                 EV<<"Queue2Server[i].length() is "<<Queue2Server[20].length()<<"\n";
                                 EV<<" Queue2Server[i].length() is "<< Queue2Server[21].length()<<"\n";

                                 delete msg;
                                 return;
                             }*/

	                        for(int i =startServer ;i< endServer ;i++){
	                            temporyOccupy +=  Queue2Server[i].length();
	                        }


			                if(temporyOccupy >= Down2ServerBufferSize*40  +1-CellNum){
			                    Queue2Serverlost[DestServer] = Queue2Serverlost[DestServer] + CellNum;
			                    delete msg;
			                }
			                else{
			                    switch(CellNum){
			                          case 1:
			                          {
			                              pkt_temp->setPacketSize(1500);
	                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                          }
			                          case 2:
			                          {
			                              pkt_temp->setPacketSize(1500);
			                            pkt *pkt_temp_1 = pkt_temp->dup();
			                            Queue2Server[DestServer].insert(pkt_temp_1);
	                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                          }
			                          case 3:
			                          {
			                              pkt_temp->setPacketSize(1500);
			                          pkt *pkt_temp_1 = pkt_temp->dup();
			                          pkt *pkt_temp_2 = pkt_temp->dup();
			                          Queue2Server[DestServer].insert(pkt_temp_1);
			                          Queue2Server[DestServer].insert(pkt_temp_2);
	                                     if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                          }
			                          case 4:
			                          {
			                              pkt_temp->setPacketSize(1500);
			                          pkt *pkt_temp_1 = pkt_temp->dup();
			                          pkt *pkt_temp_2 = pkt_temp->dup();
			                          pkt *pkt_temp_3 = pkt_temp->dup();

			                          Queue2Server[DestServer].insert(pkt_temp_1);
			                          Queue2Server[DestServer].insert(pkt_temp_2);
			                          Queue2Server[DestServer].insert(pkt_temp_3);
	                                     if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                               break;
			                          }
			                          case 5:
			                          {
			                              pkt_temp->setPacketSize(1500);
			                            pkt *pkt_temp_1 = pkt_temp->dup();
			                            pkt *pkt_temp_2 = pkt_temp->dup();
			                            pkt *pkt_temp_3 = pkt_temp->dup();
			                            pkt *pkt_temp_4 = pkt_temp->dup();
			                            Queue2Server[DestServer].insert(pkt_temp_1);
			                            Queue2Server[DestServer].insert(pkt_temp_2);
			                            Queue2Server[DestServer].insert(pkt_temp_3);
			                            Queue2Server[DestServer].insert(pkt_temp_4);
	                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                          }
			                          case 6:
			                          {    pkt_temp->setPacketSize(1500);
			                          pkt *pkt_temp_1 = pkt_temp->dup();
			                          pkt *pkt_temp_2 = pkt_temp->dup();
			                          pkt *pkt_temp_3 = pkt_temp->dup();
			                          pkt *pkt_temp_4 = pkt_temp->dup();
			                          pkt *pkt_temp_5 = pkt_temp->dup();
			                          Queue2Server[DestServer].insert(pkt_temp_1);
			                          Queue2Server[DestServer].insert(pkt_temp_2);
			                          Queue2Server[DestServer].insert(pkt_temp_3);
			                          Queue2Server[DestServer].insert(pkt_temp_4);
			                          Queue2Server[DestServer].insert(pkt_temp_5);
	                                     if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                          }
			                          case 7:
			                          {
			                              pkt_temp->setPacketSize(1500);
			                            pkt *pkt_temp_1 = pkt_temp->dup();
			                            pkt *pkt_temp_2 = pkt_temp->dup();
			                            pkt *pkt_temp_3 = pkt_temp->dup();
			                            pkt *pkt_temp_4 = pkt_temp->dup();
			                            pkt *pkt_temp_5 = pkt_temp->dup();
			                            pkt *pkt_temp_6 = pkt_temp->dup();
			                            Queue2Server[DestServer].insert(pkt_temp_1);
			                            Queue2Server[DestServer].insert(pkt_temp_2);
			                            Queue2Server[DestServer].insert(pkt_temp_3);
			                            Queue2Server[DestServer].insert(pkt_temp_4);
			                            Queue2Server[DestServer].insert(pkt_temp_5);
			                            Queue2Server[DestServer].insert(pkt_temp_6);
	                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                             break;
			                          }
			                          case 8:
			                            {
			                                pkt_temp->setPacketSize(1500);
			                                 pkt *pkt_temp_1 = pkt_temp->dup();
			                                 pkt *pkt_temp_2 = pkt_temp->dup();
			                                 pkt *pkt_temp_3 = pkt_temp->dup();
			                                 pkt *pkt_temp_4 = pkt_temp->dup();
			                                 pkt *pkt_temp_5 = pkt_temp->dup();
			                                 pkt *pkt_temp_6 = pkt_temp->dup();
			                                 pkt *pkt_temp_7 = pkt_temp->dup();
			                                 Queue2Server[DestServer].insert(pkt_temp_1);
			                                 Queue2Server[DestServer].insert(pkt_temp_2);
			                                 Queue2Server[DestServer].insert(pkt_temp_3);
			                                 Queue2Server[DestServer].insert(pkt_temp_4);
			                                 Queue2Server[DestServer].insert(pkt_temp_5);
			                                 Queue2Server[DestServer].insert(pkt_temp_6);
			                                 Queue2Server[DestServer].insert(pkt_temp_7);
		                                      if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                break;
			                            }
			                            case 9:
			                            {
			                                pkt_temp->setPacketSize(1500);
			                                pkt *pkt_temp_1 = pkt_temp->dup();
			                                pkt *pkt_temp_2 = pkt_temp->dup();
			                                pkt *pkt_temp_3 = pkt_temp->dup();
			                                pkt *pkt_temp_4 = pkt_temp->dup();
			                                pkt *pkt_temp_5 = pkt_temp->dup();
			                                pkt *pkt_temp_6 = pkt_temp->dup();
			                                pkt *pkt_temp_7 = pkt_temp->dup();
			                                pkt *pkt_temp_8 = pkt_temp->dup();
			                                Queue2Server[DestServer].insert(pkt_temp_1);
			                                Queue2Server[DestServer].insert(pkt_temp_2);
			                                Queue2Server[DestServer].insert(pkt_temp_3);
			                                Queue2Server[DestServer].insert(pkt_temp_4);
			                                Queue2Server[DestServer].insert(pkt_temp_5);
			                                Queue2Server[DestServer].insert(pkt_temp_6);
			                                Queue2Server[DestServer].insert(pkt_temp_7);
			                                Queue2Server[DestServer].insert(pkt_temp_8);
		                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                break;
			                            }
			                            case 10:
			                            {    pkt_temp->setPacketSize(1500);
			                            pkt *pkt_temp_1 = pkt_temp->dup();
			                            pkt *pkt_temp_2 = pkt_temp->dup();
			                            pkt *pkt_temp_3 = pkt_temp->dup();
			                            pkt *pkt_temp_4 = pkt_temp->dup();
			                            pkt *pkt_temp_5 = pkt_temp->dup();
			                            pkt *pkt_temp_6 = pkt_temp->dup();
			                            pkt *pkt_temp_7 = pkt_temp->dup();
			                            pkt *pkt_temp_8 = pkt_temp->dup();
			                            pkt *pkt_temp_9 = pkt_temp->dup();
			                            Queue2Server[DestServer].insert(pkt_temp_1);
			                            Queue2Server[DestServer].insert(pkt_temp_2);
			                            Queue2Server[DestServer].insert(pkt_temp_3);
			                            Queue2Server[DestServer].insert(pkt_temp_4);
			                            Queue2Server[DestServer].insert(pkt_temp_5);
			                            Queue2Server[DestServer].insert(pkt_temp_6);
			                            Queue2Server[DestServer].insert(pkt_temp_7);
			                            Queue2Server[DestServer].insert(pkt_temp_8);
			                            Queue2Server[DestServer].insert(pkt_temp_9);
	                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                break;
			                            }
			                            case 11:
			                            {     pkt_temp->setPacketSize(1500);
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
			                            Queue2Server[DestServer].insert(pkt_temp_1);
			                            Queue2Server[DestServer].insert(pkt_temp_2);
			                            Queue2Server[DestServer].insert(pkt_temp_3);
			                            Queue2Server[DestServer].insert(pkt_temp_4);
			                            Queue2Server[DestServer].insert(pkt_temp_5);
			                            Queue2Server[DestServer].insert(pkt_temp_6);
			                            Queue2Server[DestServer].insert(pkt_temp_7);
			                            Queue2Server[DestServer].insert(pkt_temp_8);
			                            Queue2Server[DestServer].insert(pkt_temp_9);
			                            Queue2Server[DestServer].insert(pkt_temp_10);
	                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                 break;
			                            }
			                            case 12:
			                            {
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
			                                Queue2Server[DestServer].insert(pkt_temp_1);
			                                Queue2Server[DestServer].insert(pkt_temp_2);
			                                Queue2Server[DestServer].insert(pkt_temp_3);
			                                Queue2Server[DestServer].insert(pkt_temp_4);
			                                Queue2Server[DestServer].insert(pkt_temp_5);
			                                Queue2Server[DestServer].insert(pkt_temp_6);
			                                Queue2Server[DestServer].insert(pkt_temp_7);
			                                Queue2Server[DestServer].insert(pkt_temp_8);
			                                Queue2Server[DestServer].insert(pkt_temp_9);
			                                Queue2Server[DestServer].insert(pkt_temp_10);
			                                Queue2Server[DestServer].insert(pkt_temp_11);
		                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                break;
			                            }
			                            case 13:
			                            {       pkt_temp->setPacketSize(1500);
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
			                            Queue2Server[DestServer].insert(pkt_temp_1);
			                            Queue2Server[DestServer].insert(pkt_temp_2);
			                            Queue2Server[DestServer].insert(pkt_temp_3);
			                            Queue2Server[DestServer].insert(pkt_temp_4);
			                            Queue2Server[DestServer].insert(pkt_temp_5);
			                            Queue2Server[DestServer].insert(pkt_temp_6);
			                            Queue2Server[DestServer].insert(pkt_temp_7);
			                            Queue2Server[DestServer].insert(pkt_temp_8);
			                            Queue2Server[DestServer].insert(pkt_temp_9);
			                            Queue2Server[DestServer].insert(pkt_temp_10);
			                            Queue2Server[DestServer].insert(pkt_temp_11);
			                            Queue2Server[DestServer].insert(pkt_temp_12);
	                                       if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                break;
			                            }
			                            case 14:
			                            {
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
			                              Queue2Server[DestServer].insert(pkt_temp_1);
			                              Queue2Server[DestServer].insert(pkt_temp_2);
			                              Queue2Server[DestServer].insert(pkt_temp_3);
			                              Queue2Server[DestServer].insert(pkt_temp_4);
			                              Queue2Server[DestServer].insert(pkt_temp_5);
			                              Queue2Server[DestServer].insert(pkt_temp_6);
			                              Queue2Server[DestServer].insert(pkt_temp_7);
			                              Queue2Server[DestServer].insert(pkt_temp_8);
			                              Queue2Server[DestServer].insert(pkt_temp_9);
			                              Queue2Server[DestServer].insert(pkt_temp_10);
			                              Queue2Server[DestServer].insert(pkt_temp_11);
			                              Queue2Server[DestServer].insert(pkt_temp_12);
			                              Queue2Server[DestServer].insert(pkt_temp_13);
		                                     if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                               break;
			                            }
			                            case 15:
			                           {
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
			                              Queue2Server[DestServer].insert(pkt_temp_1);
			                              Queue2Server[DestServer].insert(pkt_temp_2);
			                              Queue2Server[DestServer].insert(pkt_temp_3);
			                              Queue2Server[DestServer].insert(pkt_temp_4);
			                              Queue2Server[DestServer].insert(pkt_temp_5);
			                              Queue2Server[DestServer].insert(pkt_temp_6);
			                              Queue2Server[DestServer].insert(pkt_temp_7);
			                              Queue2Server[DestServer].insert(pkt_temp_8);
			                              Queue2Server[DestServer].insert(pkt_temp_9);
			                              Queue2Server[DestServer].insert(pkt_temp_10);
			                              Queue2Server[DestServer].insert(pkt_temp_11);
			                              Queue2Server[DestServer].insert(pkt_temp_12);
			                              Queue2Server[DestServer].insert(pkt_temp_13);
			                              Queue2Server[DestServer].insert(pkt_temp_14);
		                                     if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                           }
			                          case 16:
			                           {
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

			                               Queue2Server[DestServer].insert(pkt_temp_1);
			                               Queue2Server[DestServer].insert(pkt_temp_2);
			                               Queue2Server[DestServer].insert(pkt_temp_3);
			                               Queue2Server[DestServer].insert(pkt_temp_4);
			                               Queue2Server[DestServer].insert(pkt_temp_5);
			                               Queue2Server[DestServer].insert(pkt_temp_6);
			                               Queue2Server[DestServer].insert(pkt_temp_7);
			                               Queue2Server[DestServer].insert(pkt_temp_8);
			                               Queue2Server[DestServer].insert(pkt_temp_9);
			                               Queue2Server[DestServer].insert(pkt_temp_10);
			                               Queue2Server[DestServer].insert(pkt_temp_11);
			                               Queue2Server[DestServer].insert(pkt_temp_12);
			                               Queue2Server[DestServer].insert(pkt_temp_13);
			                               Queue2Server[DestServer].insert(pkt_temp_14);
			                               Queue2Server[DestServer].insert(pkt_temp_15);
		                                    if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                           }
			                          case 17:
			                          {
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
			                              Queue2Server[DestServer].insert(pkt_temp_1);
			                              Queue2Server[DestServer].insert(pkt_temp_2);
			                              Queue2Server[DestServer].insert(pkt_temp_3);
			                              Queue2Server[DestServer].insert(pkt_temp_4);
			                              Queue2Server[DestServer].insert(pkt_temp_5);
			                              Queue2Server[DestServer].insert(pkt_temp_6);
			                              Queue2Server[DestServer].insert(pkt_temp_7);
			                              Queue2Server[DestServer].insert(pkt_temp_8);
			                              Queue2Server[DestServer].insert(pkt_temp_9);
			                              Queue2Server[DestServer].insert(pkt_temp_10);
			                              Queue2Server[DestServer].insert(pkt_temp_11);
			                              Queue2Server[DestServer].insert(pkt_temp_12);
			                              Queue2Server[DestServer].insert(pkt_temp_13);
			                              Queue2Server[DestServer].insert(pkt_temp_14);
			                              Queue2Server[DestServer].insert(pkt_temp_15);
			                              Queue2Server[DestServer].insert(pkt_temp_16);
		                                     if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                          }
			                          case 18:
			                          {
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
			                             Queue2Server[DestServer].insert(pkt_temp_1);
			                             Queue2Server[DestServer].insert(pkt_temp_2);
			                             Queue2Server[DestServer].insert(pkt_temp_3);
			                             Queue2Server[DestServer].insert(pkt_temp_4);
			                             Queue2Server[DestServer].insert(pkt_temp_5);
			                             Queue2Server[DestServer].insert(pkt_temp_6);
			                             Queue2Server[DestServer].insert(pkt_temp_7);
			                             Queue2Server[DestServer].insert(pkt_temp_8);
			                             Queue2Server[DestServer].insert(pkt_temp_9);
			                             Queue2Server[DestServer].insert(pkt_temp_10);
			                             Queue2Server[DestServer].insert(pkt_temp_11);
			                             Queue2Server[DestServer].insert(pkt_temp_12);
			                             Queue2Server[DestServer].insert(pkt_temp_13);
			                             Queue2Server[DestServer].insert(pkt_temp_14);
			                             Queue2Server[DestServer].insert(pkt_temp_15);
			                             Queue2Server[DestServer].insert(pkt_temp_16);
			                             Queue2Server[DestServer].insert(pkt_temp_17);
	                                      if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                               break;
			                          }
			                          case 19:
			                          {
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
			                           Queue2Server[DestServer].insert(pkt_temp_1);
			                           Queue2Server[DestServer].insert(pkt_temp_2);
			                           Queue2Server[DestServer].insert(pkt_temp_3);
			                           Queue2Server[DestServer].insert(pkt_temp_4);
			                           Queue2Server[DestServer].insert(pkt_temp_5);
			                           Queue2Server[DestServer].insert(pkt_temp_6);
			                           Queue2Server[DestServer].insert(pkt_temp_7);
			                           Queue2Server[DestServer].insert(pkt_temp_8);
			                           Queue2Server[DestServer].insert(pkt_temp_9);
			                           Queue2Server[DestServer].insert(pkt_temp_10);
			                           Queue2Server[DestServer].insert(pkt_temp_11);
			                           Queue2Server[DestServer].insert(pkt_temp_12);
			                           Queue2Server[DestServer].insert(pkt_temp_13);
			                           Queue2Server[DestServer].insert(pkt_temp_14);
			                           Queue2Server[DestServer].insert(pkt_temp_15);
			                           Queue2Server[DestServer].insert(pkt_temp_16);
			                           Queue2Server[DestServer].insert(pkt_temp_17);
			                           Queue2Server[DestServer].insert(pkt_temp_18);
	                                   if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                          }
			                          case 20:
			                          {
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
			                          Queue2Server[DestServer].insert(pkt_temp_1);
			                          Queue2Server[DestServer].insert(pkt_temp_2);
			                          Queue2Server[DestServer].insert(pkt_temp_3);
			                          Queue2Server[DestServer].insert(pkt_temp_4);
			                          Queue2Server[DestServer].insert(pkt_temp_5);
			                          Queue2Server[DestServer].insert(pkt_temp_6);
			                          Queue2Server[DestServer].insert(pkt_temp_7);
			                          Queue2Server[DestServer].insert(pkt_temp_8);
			                          Queue2Server[DestServer].insert(pkt_temp_9);
			                          Queue2Server[DestServer].insert(pkt_temp_10);
			                          Queue2Server[DestServer].insert(pkt_temp_11);
			                          Queue2Server[DestServer].insert(pkt_temp_12);
			                          Queue2Server[DestServer].insert(pkt_temp_13);
			                          Queue2Server[DestServer].insert(pkt_temp_14);
			                          Queue2Server[DestServer].insert(pkt_temp_15);
			                          Queue2Server[DestServer].insert(pkt_temp_16);
			                          Queue2Server[DestServer].insert(pkt_temp_17);
			                          Queue2Server[DestServer].insert(pkt_temp_18);
			                          Queue2Server[DestServer].insert(pkt_temp_19);
	                                  if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                              break;
			                          }
			                          case 21:
			                          {
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
			                          Queue2Server[DestServer].insert(pkt_temp_1);
			                          Queue2Server[DestServer].insert(pkt_temp_2);
			                          Queue2Server[DestServer].insert(pkt_temp_3);
			                          Queue2Server[DestServer].insert(pkt_temp_4);
			                          Queue2Server[DestServer].insert(pkt_temp_5);
			                          Queue2Server[DestServer].insert(pkt_temp_6);
			                          Queue2Server[DestServer].insert(pkt_temp_7);
			                          Queue2Server[DestServer].insert(pkt_temp_8);
			                          Queue2Server[DestServer].insert(pkt_temp_9);
			                          Queue2Server[DestServer].insert(pkt_temp_10);
			                          Queue2Server[DestServer].insert(pkt_temp_11);
			                          Queue2Server[DestServer].insert(pkt_temp_12);
			                          Queue2Server[DestServer].insert(pkt_temp_13);
			                          Queue2Server[DestServer].insert(pkt_temp_14);
			                          Queue2Server[DestServer].insert(pkt_temp_15);
			                          Queue2Server[DestServer].insert(pkt_temp_16);
			                          Queue2Server[DestServer].insert(pkt_temp_17);
			                          Queue2Server[DestServer].insert(pkt_temp_18);
			                          Queue2Server[DestServer].insert(pkt_temp_19);
			                          Queue2Server[DestServer].insert(pkt_temp_20);
	                                  if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                             break;
			                          }
			                          case 22:
			                            {
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
			                            Queue2Server[DestServer].insert(pkt_temp_1);
			                            Queue2Server[DestServer].insert(pkt_temp_2);
			                            Queue2Server[DestServer].insert(pkt_temp_3);
			                            Queue2Server[DestServer].insert(pkt_temp_4);
			                            Queue2Server[DestServer].insert(pkt_temp_5);
			                            Queue2Server[DestServer].insert(pkt_temp_6);
			                            Queue2Server[DestServer].insert(pkt_temp_7);
			                            Queue2Server[DestServer].insert(pkt_temp_8);
			                            Queue2Server[DestServer].insert(pkt_temp_9);
			                            Queue2Server[DestServer].insert(pkt_temp_10);
			                            Queue2Server[DestServer].insert(pkt_temp_11);
			                            Queue2Server[DestServer].insert(pkt_temp_12);
			                            Queue2Server[DestServer].insert(pkt_temp_13);
			                            Queue2Server[DestServer].insert(pkt_temp_14);
			                            Queue2Server[DestServer].insert(pkt_temp_15);
			                            Queue2Server[DestServer].insert(pkt_temp_16);
			                            Queue2Server[DestServer].insert(pkt_temp_17);
			                            Queue2Server[DestServer].insert(pkt_temp_18);
			                            Queue2Server[DestServer].insert(pkt_temp_19);
			                            Queue2Server[DestServer].insert(pkt_temp_20);
			                            Queue2Server[DestServer].insert(pkt_temp_21);
	                                    if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                break;
			                            }
			                            case 23:
			                            {
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
			                            Queue2Server[DestServer].insert(pkt_temp_1);
			                            Queue2Server[DestServer].insert(pkt_temp_2);
			                            Queue2Server[DestServer].insert(pkt_temp_3);
			                            Queue2Server[DestServer].insert(pkt_temp_4);
			                            Queue2Server[DestServer].insert(pkt_temp_5);
			                            Queue2Server[DestServer].insert(pkt_temp_6);
			                            Queue2Server[DestServer].insert(pkt_temp_7);
			                            Queue2Server[DestServer].insert(pkt_temp_8);
			                            Queue2Server[DestServer].insert(pkt_temp_9);
			                            Queue2Server[DestServer].insert(pkt_temp_10);
			                            Queue2Server[DestServer].insert(pkt_temp_11);
			                            Queue2Server[DestServer].insert(pkt_temp_12);
			                            Queue2Server[DestServer].insert(pkt_temp_13);
			                            Queue2Server[DestServer].insert(pkt_temp_14);
			                            Queue2Server[DestServer].insert(pkt_temp_15);
			                            Queue2Server[DestServer].insert(pkt_temp_16);
			                            Queue2Server[DestServer].insert(pkt_temp_17);
			                            Queue2Server[DestServer].insert(pkt_temp_18);
			                            Queue2Server[DestServer].insert(pkt_temp_19);
			                            Queue2Server[DestServer].insert(pkt_temp_20);
			                            Queue2Server[DestServer].insert(pkt_temp_21);
			                            Queue2Server[DestServer].insert(pkt_temp_22);
	                                    if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                break;
			                            }
			                            case 24:
			                            {
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
			                            Queue2Server[DestServer].insert(pkt_temp_1);
			                            Queue2Server[DestServer].insert(pkt_temp_2);
			                            Queue2Server[DestServer].insert(pkt_temp_3);
			                            Queue2Server[DestServer].insert(pkt_temp_4);
			                            Queue2Server[DestServer].insert(pkt_temp_5);
			                            Queue2Server[DestServer].insert(pkt_temp_6);
			                            Queue2Server[DestServer].insert(pkt_temp_7);
			                            Queue2Server[DestServer].insert(pkt_temp_8);
			                            Queue2Server[DestServer].insert(pkt_temp_9);
			                            Queue2Server[DestServer].insert(pkt_temp_10);
			                            Queue2Server[DestServer].insert(pkt_temp_11);
			                            Queue2Server[DestServer].insert(pkt_temp_12);
			                            Queue2Server[DestServer].insert(pkt_temp_13);
			                            Queue2Server[DestServer].insert(pkt_temp_14);
			                            Queue2Server[DestServer].insert(pkt_temp_15);
			                            Queue2Server[DestServer].insert(pkt_temp_16);
			                            Queue2Server[DestServer].insert(pkt_temp_17);
			                            Queue2Server[DestServer].insert(pkt_temp_18);
			                            Queue2Server[DestServer].insert(pkt_temp_19);
			                            Queue2Server[DestServer].insert(pkt_temp_20);
			                            Queue2Server[DestServer].insert(pkt_temp_21);
			                            Queue2Server[DestServer].insert(pkt_temp_22);
			                            Queue2Server[DestServer].insert(pkt_temp_23);
	                                    if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                break;
			                            }
			                            case 25:
			                            {
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
			                             
			                             Queue2Server[DestServer].insert(pkt_temp_1);
			                             Queue2Server[DestServer].insert(pkt_temp_2);
			                             Queue2Server[DestServer].insert(pkt_temp_3);
			                             Queue2Server[DestServer].insert(pkt_temp_4);
			                             Queue2Server[DestServer].insert(pkt_temp_5);
			                             Queue2Server[DestServer].insert(pkt_temp_6);
			                             Queue2Server[DestServer].insert(pkt_temp_7);
			                             Queue2Server[DestServer].insert(pkt_temp_8);
			                             Queue2Server[DestServer].insert(pkt_temp_9);
			                             Queue2Server[DestServer].insert(pkt_temp_10);
			                             Queue2Server[DestServer].insert(pkt_temp_11);
			                             Queue2Server[DestServer].insert(pkt_temp_12);
			                             Queue2Server[DestServer].insert(pkt_temp_13);
			                             Queue2Server[DestServer].insert(pkt_temp_14);
			                             Queue2Server[DestServer].insert(pkt_temp_15);
			                             Queue2Server[DestServer].insert(pkt_temp_16);
			                             Queue2Server[DestServer].insert(pkt_temp_17);
			                             Queue2Server[DestServer].insert(pkt_temp_18);
			                             Queue2Server[DestServer].insert(pkt_temp_19);
			                             Queue2Server[DestServer].insert(pkt_temp_20);
			                             Queue2Server[DestServer].insert(pkt_temp_21);
			                             Queue2Server[DestServer].insert(pkt_temp_22);
			                             Queue2Server[DestServer].insert(pkt_temp_23);
			                             Queue2Server[DestServer].insert(pkt_temp_24);
										 if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
			                                 break;
			                            }
                                        case 37:
                                        {
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
                                         Queue2Server[DestServer].insert(pkt_temp_1);
                                         Queue2Server[DestServer].insert(pkt_temp_2);
                                         Queue2Server[DestServer].insert(pkt_temp_3);
                                         Queue2Server[DestServer].insert(pkt_temp_4);
                                         Queue2Server[DestServer].insert(pkt_temp_5);
                                         Queue2Server[DestServer].insert(pkt_temp_6);
                                         Queue2Server[DestServer].insert(pkt_temp_7);
                                         Queue2Server[DestServer].insert(pkt_temp_8);
                                         Queue2Server[DestServer].insert(pkt_temp_9);
                                         Queue2Server[DestServer].insert(pkt_temp_10);
                                         Queue2Server[DestServer].insert(pkt_temp_11);
                                         Queue2Server[DestServer].insert(pkt_temp_12);
                                         Queue2Server[DestServer].insert(pkt_temp_13);
                                         Queue2Server[DestServer].insert(pkt_temp_14);
                                         Queue2Server[DestServer].insert(pkt_temp_15);
                                         Queue2Server[DestServer].insert(pkt_temp_16);
                                         Queue2Server[DestServer].insert(pkt_temp_17);
                                         Queue2Server[DestServer].insert(pkt_temp_18);
                                         Queue2Server[DestServer].insert(pkt_temp_19);
                                         Queue2Server[DestServer].insert(pkt_temp_20);
                                         Queue2Server[DestServer].insert(pkt_temp_21);
                                         Queue2Server[DestServer].insert(pkt_temp_22);
                                         Queue2Server[DestServer].insert(pkt_temp_23);
                                         Queue2Server[DestServer].insert(pkt_temp_24);
                                         Queue2Server[DestServer].insert(pkt_temp_25);
                                         Queue2Server[DestServer].insert(pkt_temp_26);
                                         Queue2Server[DestServer].insert(pkt_temp_27);
                                         Queue2Server[DestServer].insert(pkt_temp_28);
                                         Queue2Server[DestServer].insert(pkt_temp_29);
                                         Queue2Server[DestServer].insert(pkt_temp_30);
                                         Queue2Server[DestServer].insert(pkt_temp_31);
                                         Queue2Server[DestServer].insert(pkt_temp_32);
                                         Queue2Server[DestServer].insert(pkt_temp_33);
                                         Queue2Server[DestServer].insert(pkt_temp_34);
                                         Queue2Server[DestServer].insert(pkt_temp_35);
                                         Queue2Server[DestServer].insert(pkt_temp_36);
                                         if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
                                             break;
                                         }
                                        case 44:
                                        {
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
                                         Queue2Server[DestServer].insert(pkt_temp_1);
                                         Queue2Server[DestServer].insert(pkt_temp_2);
                                         Queue2Server[DestServer].insert(pkt_temp_3);
                                         Queue2Server[DestServer].insert(pkt_temp_4);
                                         Queue2Server[DestServer].insert(pkt_temp_5);
                                         Queue2Server[DestServer].insert(pkt_temp_6);
                                         Queue2Server[DestServer].insert(pkt_temp_7);
                                         Queue2Server[DestServer].insert(pkt_temp_8);
                                         Queue2Server[DestServer].insert(pkt_temp_9);
                                         Queue2Server[DestServer].insert(pkt_temp_10);
                                         Queue2Server[DestServer].insert(pkt_temp_11);
                                         Queue2Server[DestServer].insert(pkt_temp_12);
                                         Queue2Server[DestServer].insert(pkt_temp_13);
                                         Queue2Server[DestServer].insert(pkt_temp_14);
                                         Queue2Server[DestServer].insert(pkt_temp_15);
                                         Queue2Server[DestServer].insert(pkt_temp_16);
                                         Queue2Server[DestServer].insert(pkt_temp_17);
                                         Queue2Server[DestServer].insert(pkt_temp_18);
                                         Queue2Server[DestServer].insert(pkt_temp_19);
                                         Queue2Server[DestServer].insert(pkt_temp_20);
                                         Queue2Server[DestServer].insert(pkt_temp_21);
                                         Queue2Server[DestServer].insert(pkt_temp_22);
                                         Queue2Server[DestServer].insert(pkt_temp_23);
                                         Queue2Server[DestServer].insert(pkt_temp_24);
                                         Queue2Server[DestServer].insert(pkt_temp_25);
                                         Queue2Server[DestServer].insert(pkt_temp_26);
                                         Queue2Server[DestServer].insert(pkt_temp_27);
                                         Queue2Server[DestServer].insert(pkt_temp_28);
                                         Queue2Server[DestServer].insert(pkt_temp_29);
                                         Queue2Server[DestServer].insert(pkt_temp_30);
                                         Queue2Server[DestServer].insert(pkt_temp_31);
                                         Queue2Server[DestServer].insert(pkt_temp_32);
                                         Queue2Server[DestServer].insert(pkt_temp_33);
                                         Queue2Server[DestServer].insert(pkt_temp_34);
                                         Queue2Server[DestServer].insert(pkt_temp_35);
                                         Queue2Server[DestServer].insert(pkt_temp_36);
                                         Queue2Server[DestServer].insert(pkt_temp_37);
                                         Queue2Server[DestServer].insert(pkt_temp_38);
                                         Queue2Server[DestServer].insert(pkt_temp_39);
                                         Queue2Server[DestServer].insert(pkt_temp_40);
                                         Queue2Server[DestServer].insert(pkt_temp_41);
                                         Queue2Server[DestServer].insert(pkt_temp_42);
                                         Queue2Server[DestServer].insert(pkt_temp_43);
                                         if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
                                             break;
                                         }
                                        case 45:
                                        {
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
                                         Queue2Server[DestServer].insert(pkt_temp_1);
                                         Queue2Server[DestServer].insert(pkt_temp_2);
                                         Queue2Server[DestServer].insert(pkt_temp_3);
                                         Queue2Server[DestServer].insert(pkt_temp_4);
                                         Queue2Server[DestServer].insert(pkt_temp_5);
                                         Queue2Server[DestServer].insert(pkt_temp_6);
                                         Queue2Server[DestServer].insert(pkt_temp_7);
                                         Queue2Server[DestServer].insert(pkt_temp_8);
                                         Queue2Server[DestServer].insert(pkt_temp_9);
                                         Queue2Server[DestServer].insert(pkt_temp_10);
                                         Queue2Server[DestServer].insert(pkt_temp_11);
                                         Queue2Server[DestServer].insert(pkt_temp_12);
                                         Queue2Server[DestServer].insert(pkt_temp_13);
                                         Queue2Server[DestServer].insert(pkt_temp_14);
                                         Queue2Server[DestServer].insert(pkt_temp_15);
                                         Queue2Server[DestServer].insert(pkt_temp_16);
                                         Queue2Server[DestServer].insert(pkt_temp_17);
                                         Queue2Server[DestServer].insert(pkt_temp_18);
                                         Queue2Server[DestServer].insert(pkt_temp_19);
                                         Queue2Server[DestServer].insert(pkt_temp_20);
                                         Queue2Server[DestServer].insert(pkt_temp_21);
                                         Queue2Server[DestServer].insert(pkt_temp_22);
                                         Queue2Server[DestServer].insert(pkt_temp_23);
                                         Queue2Server[DestServer].insert(pkt_temp_24);
                                         Queue2Server[DestServer].insert(pkt_temp_25);
                                         Queue2Server[DestServer].insert(pkt_temp_26);
                                         Queue2Server[DestServer].insert(pkt_temp_27);
                                         Queue2Server[DestServer].insert(pkt_temp_28);
                                         Queue2Server[DestServer].insert(pkt_temp_29);
                                         Queue2Server[DestServer].insert(pkt_temp_30);
                                         Queue2Server[DestServer].insert(pkt_temp_31);
                                         Queue2Server[DestServer].insert(pkt_temp_32);
                                         Queue2Server[DestServer].insert(pkt_temp_33);
                                         Queue2Server[DestServer].insert(pkt_temp_34);
                                         Queue2Server[DestServer].insert(pkt_temp_35);
                                         Queue2Server[DestServer].insert(pkt_temp_36);
                                         Queue2Server[DestServer].insert(pkt_temp_37);
                                         Queue2Server[DestServer].insert(pkt_temp_38);
                                         Queue2Server[DestServer].insert(pkt_temp_39);
                                         Queue2Server[DestServer].insert(pkt_temp_40);
                                         Queue2Server[DestServer].insert(pkt_temp_41);
                                         Queue2Server[DestServer].insert(pkt_temp_42);
                                         Queue2Server[DestServer].insert(pkt_temp_43);
                                         Queue2Server[DestServer].insert(pkt_temp_44);
                                         if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
                                             break;
                                         }
                                        case 49:
                                        {
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
                                         Queue2Server[DestServer].insert(pkt_temp_1);
                                         Queue2Server[DestServer].insert(pkt_temp_2);
                                         Queue2Server[DestServer].insert(pkt_temp_3);
                                         Queue2Server[DestServer].insert(pkt_temp_4);
                                         Queue2Server[DestServer].insert(pkt_temp_5);
                                         Queue2Server[DestServer].insert(pkt_temp_6);
                                         Queue2Server[DestServer].insert(pkt_temp_7);
                                         Queue2Server[DestServer].insert(pkt_temp_8);
                                         Queue2Server[DestServer].insert(pkt_temp_9);
                                         Queue2Server[DestServer].insert(pkt_temp_10);
                                         Queue2Server[DestServer].insert(pkt_temp_11);
                                         Queue2Server[DestServer].insert(pkt_temp_12);
                                         Queue2Server[DestServer].insert(pkt_temp_13);
                                         Queue2Server[DestServer].insert(pkt_temp_14);
                                         Queue2Server[DestServer].insert(pkt_temp_15);
                                         Queue2Server[DestServer].insert(pkt_temp_16);
                                         Queue2Server[DestServer].insert(pkt_temp_17);
                                         Queue2Server[DestServer].insert(pkt_temp_18);
                                         Queue2Server[DestServer].insert(pkt_temp_19);
                                         Queue2Server[DestServer].insert(pkt_temp_20);
                                         Queue2Server[DestServer].insert(pkt_temp_21);
                                         Queue2Server[DestServer].insert(pkt_temp_22);
                                         Queue2Server[DestServer].insert(pkt_temp_23);
                                         Queue2Server[DestServer].insert(pkt_temp_24);
                                         Queue2Server[DestServer].insert(pkt_temp_25);
                                         Queue2Server[DestServer].insert(pkt_temp_26);
                                         Queue2Server[DestServer].insert(pkt_temp_27);
                                         Queue2Server[DestServer].insert(pkt_temp_28);
                                         Queue2Server[DestServer].insert(pkt_temp_29);
                                         Queue2Server[DestServer].insert(pkt_temp_30);
                                         Queue2Server[DestServer].insert(pkt_temp_31);
                                         Queue2Server[DestServer].insert(pkt_temp_32);
                                         Queue2Server[DestServer].insert(pkt_temp_33);
                                         Queue2Server[DestServer].insert(pkt_temp_34);
                                         Queue2Server[DestServer].insert(pkt_temp_35);
                                         Queue2Server[DestServer].insert(pkt_temp_36);
                                         Queue2Server[DestServer].insert(pkt_temp_37);
                                         Queue2Server[DestServer].insert(pkt_temp_38);
                                         Queue2Server[DestServer].insert(pkt_temp_39);
                                         Queue2Server[DestServer].insert(pkt_temp_40);
                                         Queue2Server[DestServer].insert(pkt_temp_41);
                                         Queue2Server[DestServer].insert(pkt_temp_42);
                                         Queue2Server[DestServer].insert(pkt_temp_43);
                                         Queue2Server[DestServer].insert(pkt_temp_44);
                                         Queue2Server[DestServer].insert(pkt_temp_45);
                                         Queue2Server[DestServer].insert(pkt_temp_46);
                                         Queue2Server[DestServer].insert(pkt_temp_47);
                                         Queue2Server[DestServer].insert(pkt_temp_48);
                                         if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
                                             break;
                                         }
                                        case 91:
                                        {
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
                                         Queue2Server[DestServer].insert(pkt_temp_1);
                                         Queue2Server[DestServer].insert(pkt_temp_2);
                                         Queue2Server[DestServer].insert(pkt_temp_3);
                                         Queue2Server[DestServer].insert(pkt_temp_4);
                                         Queue2Server[DestServer].insert(pkt_temp_5);
                                         Queue2Server[DestServer].insert(pkt_temp_6);
                                         Queue2Server[DestServer].insert(pkt_temp_7);
                                         Queue2Server[DestServer].insert(pkt_temp_8);
                                         Queue2Server[DestServer].insert(pkt_temp_9);
                                         Queue2Server[DestServer].insert(pkt_temp_10);
                                         Queue2Server[DestServer].insert(pkt_temp_11);
                                         Queue2Server[DestServer].insert(pkt_temp_12);
                                         Queue2Server[DestServer].insert(pkt_temp_13);
                                         Queue2Server[DestServer].insert(pkt_temp_14);
                                         Queue2Server[DestServer].insert(pkt_temp_15);
                                         Queue2Server[DestServer].insert(pkt_temp_16);
                                         Queue2Server[DestServer].insert(pkt_temp_17);
                                         Queue2Server[DestServer].insert(pkt_temp_18);
                                         Queue2Server[DestServer].insert(pkt_temp_19);
                                         Queue2Server[DestServer].insert(pkt_temp_20);
                                         Queue2Server[DestServer].insert(pkt_temp_21);
                                         Queue2Server[DestServer].insert(pkt_temp_22);
                                         Queue2Server[DestServer].insert(pkt_temp_23);
                                         Queue2Server[DestServer].insert(pkt_temp_24);
                                         Queue2Server[DestServer].insert(pkt_temp_25);
                                         Queue2Server[DestServer].insert(pkt_temp_26);
                                         Queue2Server[DestServer].insert(pkt_temp_27);
                                         Queue2Server[DestServer].insert(pkt_temp_28);
                                         Queue2Server[DestServer].insert(pkt_temp_29);
                                         Queue2Server[DestServer].insert(pkt_temp_30);
                                         Queue2Server[DestServer].insert(pkt_temp_31);
                                         Queue2Server[DestServer].insert(pkt_temp_32);
                                         Queue2Server[DestServer].insert(pkt_temp_33);
                                         Queue2Server[DestServer].insert(pkt_temp_34);
                                         Queue2Server[DestServer].insert(pkt_temp_35);
                                         Queue2Server[DestServer].insert(pkt_temp_36);
                                         Queue2Server[DestServer].insert(pkt_temp_37);
                                         Queue2Server[DestServer].insert(pkt_temp_38);
                                         Queue2Server[DestServer].insert(pkt_temp_39);
                                         Queue2Server[DestServer].insert(pkt_temp_40);
                                         Queue2Server[DestServer].insert(pkt_temp_41);
                                         Queue2Server[DestServer].insert(pkt_temp_42);
                                         Queue2Server[DestServer].insert(pkt_temp_43);
                                         Queue2Server[DestServer].insert(pkt_temp_44);
                                         Queue2Server[DestServer].insert(pkt_temp_45);
                                         Queue2Server[DestServer].insert(pkt_temp_46);
                                         Queue2Server[DestServer].insert(pkt_temp_47);
                                         Queue2Server[DestServer].insert(pkt_temp_48);
                                         Queue2Server[DestServer].insert(pkt_temp_49);
                                         Queue2Server[DestServer].insert(pkt_temp_50);
                                         Queue2Server[DestServer].insert(pkt_temp_51);
                                         Queue2Server[DestServer].insert(pkt_temp_52);
                                         Queue2Server[DestServer].insert(pkt_temp_53);
                                         Queue2Server[DestServer].insert(pkt_temp_54);
                                         Queue2Server[DestServer].insert(pkt_temp_55);
                                         Queue2Server[DestServer].insert(pkt_temp_56);
                                         Queue2Server[DestServer].insert(pkt_temp_57);
                                         Queue2Server[DestServer].insert(pkt_temp_58);
                                         Queue2Server[DestServer].insert(pkt_temp_59);
                                         Queue2Server[DestServer].insert(pkt_temp_60);
                                         Queue2Server[DestServer].insert(pkt_temp_61);
                                         Queue2Server[DestServer].insert(pkt_temp_62);
                                         Queue2Server[DestServer].insert(pkt_temp_63);
                                         Queue2Server[DestServer].insert(pkt_temp_64);
                                         Queue2Server[DestServer].insert(pkt_temp_65);
                                         Queue2Server[DestServer].insert(pkt_temp_66);
                                         Queue2Server[DestServer].insert(pkt_temp_67);
                                         Queue2Server[DestServer].insert(pkt_temp_68);
                                         Queue2Server[DestServer].insert(pkt_temp_69);
                                         Queue2Server[DestServer].insert(pkt_temp_70);
                                         Queue2Server[DestServer].insert(pkt_temp_71);
                                         Queue2Server[DestServer].insert(pkt_temp_72);
                                         Queue2Server[DestServer].insert(pkt_temp_73);
                                         Queue2Server[DestServer].insert(pkt_temp_74);
                                         Queue2Server[DestServer].insert(pkt_temp_75);
                                         Queue2Server[DestServer].insert(pkt_temp_76);
                                         Queue2Server[DestServer].insert(pkt_temp_77);
                                         Queue2Server[DestServer].insert(pkt_temp_78);
                                         Queue2Server[DestServer].insert(pkt_temp_79);
                                         Queue2Server[DestServer].insert(pkt_temp_80);
                                         Queue2Server[DestServer].insert(pkt_temp_81);
                                         Queue2Server[DestServer].insert(pkt_temp_82);
                                         Queue2Server[DestServer].insert(pkt_temp_83);
                                         Queue2Server[DestServer].insert(pkt_temp_84);
                                         Queue2Server[DestServer].insert(pkt_temp_85);
                                         Queue2Server[DestServer].insert(pkt_temp_86);
                                         Queue2Server[DestServer].insert(pkt_temp_87);
                                         Queue2Server[DestServer].insert(pkt_temp_88);
                                         Queue2Server[DestServer].insert(pkt_temp_89);
                                         Queue2Server[DestServer].insert(pkt_temp_90);

                                         if(pkt_temp->getHopCountNow() == 0) { pkt_temp->setFlag_last(1);}  Queue2Server[DestServer].insert(pkt_temp);
                                             break;
                                         }


			                            default:
			                                 break;
			                      }
			                    }
			            }
			        }
			    }
			    else if(pkt_temp->getIndex() == 2){
			        int Queue2ServerIndex = pkt_temp->getDst_Server();
			        if(!Queue2Server[Queue2ServerIndex].empty() == 1){  // not empty
			            pkt * pkt_to_send = (pkt *)Queue2Server[Queue2ServerIndex].pop();

			            //int countNow = pkt_to_send->getHopCountNow();
			            //pkt_to_send->setHopCountNow(countNow+1);

			            send(pkt_to_send,"serverout",Queue2ServerIndex%ToRserverNum);
			            finishtime = gate("serverout",Queue2ServerIndex%ToRserverNum)->getTransmissionChannel()->getTransmissionFinishTime();

			            pkt *pkt2l = new pkt("local_pkt_ToR");
			            pkt2l->setDst_Server(Queue2ServerIndex);
			            pkt2l->setIndex(2);
			            scheduleAt(simTime() + slot_ToR,pkt2l);
			        }
			        else{
			            Queue2Server_busy[Queue2ServerIndex] = false;
			        }

			        delete msg;
			    }
			    else if(pkt_temp->getIndex() == 5){
			        coutserver = pkt_temp->getSrc_Server();
			        msgfromserver2headprocessorfinish[coutserver] = true;
			        FinishserverNum++;
			        EV<<"FinishserverNum is"<<FinishserverNum<<"\n";
			        if(FinishserverNum == ToRserverNum){
			            pkt *finishmsg0 = new pkt();
			            finishmsg0->setIndex(5);
			            for(int i=0;i<=NumTx+NumTx_ToR2ToR-1;i++)   // all Tx agre plus core;
			                sendDelayed(finishmsg0->dup(),1e-2,"bufferout",i);

			            delete finishmsg0;
			        }
					HeadProcessorfinishtime = simTime();
			        delete msg;

			    }
			    else
			        delete msg;
	}
}
void HeadProcessor::finish()
{
    cModule *targetModule = getParentModule();
    int tt = targetModule->getIndex();
//    recordScalar("ToR-Send-To---other----",receivedPackets);
    recordScalar("Send-To-ToR-Cell-Number",count2ToR[tt]);
    for(int i=0;i<ToRserverNum;i++){

        Queue2ServerAveragelost = Queue2ServerAveragelost + Queue2Serverlost[ToRserverNum*tt+i];

    }
//    recordScalar("receivedPackets--------",receivedPackets);

    recordScalar("Received-interToR-Cells",numReceived_ToR);
    recordScalar("Received-Total----Cells",numReceived_Total);
    recordScalar("ToR-lost---Queue2server",Queue2ServerAveragelost);
    recordScalar("HeadProcesso-FinishTime",HeadProcessorfinishtime);
    recordScalar("Received-interToR--Pkts",PnumReceived_ToR);
    recordScalar("ToR2ToR-Average-Latency",ToR2ToR_Latency);
    recordScalar("PToR2ToRAverage-Latency",PToR2ToR_Latency);

}


