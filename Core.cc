#include<stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "pkt_m.h"
#include "Optical_m.h"
//bool Core_Channel_busy[256];

bool Core_Channel_busy[4*4096];

class CoreY : public cSimpleModule
{
   private:
        simtime_t finish_time;
        int totalserverNum;
        int ToRserverNum;
        int ClusterToRNum;
        int AllToRNum;
        int AgreRadix;
        int CoreRadix;
        int PreviousToR;
        int NextToR;
        int NumTx;
        int NumTx_ToR2ToR;
        double slot_OPS;
        cQueue SendLaterCore;
        int Core_queueAveragelost ;

        bool test;

        simtime_t delayCore;
        simtime_t delayforACK;

        simtime_t dealy_MSG;
        simtime_t dealy_ACK;

        pkt *local_CorePkt_MSG;
        pkt *slot_OPS_MSG;

        int OutputPortNum;
        int InputPortNum;

        int OPSPortNum;
        int CurrentOPSIndex;

        int GroupToRIndex;
        int TxIndex;

        int GroupToRCore;
        int app_pkt_num;
   protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
//        virtual void finish();

   public:
        virtual ~CoreY();
};

Define_Module(CoreY);

CoreY::~CoreY(){
    cancelAndDelete(local_CorePkt_MSG);
    cancelAndDelete(slot_OPS_MSG);
    while(!SendLaterCore.empty()){
        Optical *OPticla_pkt_temp = check_and_cast<Optical *>(SendLaterCore.pop());
        int optNum = OPticla_pkt_temp->getOptical_num();
        for(int i=0;i<optNum;i++){
            switch(i){
                case 0:
                    OPticla_pkt_temp->removeMessage(OPticla_pkt_temp->getPkt1());
                    delete OPticla_pkt_temp->getPkt1();
                    break;
                case 1:
                    OPticla_pkt_temp->removeMessage(OPticla_pkt_temp->getPkt2());
                    delete OPticla_pkt_temp->getPkt2();
                    break;
                case 2:
                    OPticla_pkt_temp->removeMessage(OPticla_pkt_temp->getPkt3());
                    delete OPticla_pkt_temp->getPkt3();
                    break;
                case 3:
                    OPticla_pkt_temp->removeMessage(OPticla_pkt_temp->getPkt4());
                    delete OPticla_pkt_temp->getPkt4();
                    break;
                case 4:
                    OPticla_pkt_temp->removeMessage(OPticla_pkt_temp->getPkt5());
                    delete OPticla_pkt_temp->getPkt5();
                    break;
            }
        }
        delete OPticla_pkt_temp;
    }
}

void CoreY::initialize()
{
    EV << "Core initialize ";

    Core_queueAveragelost = 0;

    totalserverNum = par("ServerNum");
    ToRserverNum = par("ToRDownRadix");
    AllToRNum = totalserverNum/ToRserverNum;

//    ClusterToRNum = sqrt(AllToRNum);
    ClusterToRNum = par("AgreRadix");
    AgreRadix = par("AgreRadix");
    CoreRadix = par("CoreRadix");
    NumTx_ToR2ToR=par("NumTx_ToR2ToR");
    NumTx=par("NumTx");
    GroupToRCore=par("GroupToRCore");
//    OPSPortNum = AgreRadix;
    OPSPortNum = CoreRadix*NumTx_ToR2ToR;
//    delayCore = 20e-9;

    delayforACK = 20e-9;

    dealy_MSG = 20e-9;
//    dealy_ACK = 1e-9;
    test = false;

    slot_OPS = 10e-9;

    OutputPortNum = 0;
    InputPortNum = 0;

    GroupToRIndex = 0;
    TxIndex = 0;
    if(getIndex()==0){
        for(int j=0;j<4*4096;j++){
            Core_Channel_busy[j] = false;
        }
    }

//    Core_Channel_busy = false;
    local_CorePkt_MSG = new pkt ("local_CorePkt_MSG");
    local_CorePkt_MSG->setKind(0);

    slot_OPS_MSG = new pkt ("slot_OPS_MSG");
    slot_OPS_MSG->setKind(400);

    CurrentOPSIndex = 0;
    app_pkt_num = 0;
}

void CoreY::handleMessage(cMessage *msg)
{
//    pkt *pkt_temp =  check_and_cast<pkt*>(msg);

    if(msg->getKind() == 1000){

        //PreviousToR = pkt_temp->getHop(pkt_temp->getHopCountNow() - 1);
        //NextToR = pkt_temp->getHop(pkt_temp->getHopCountNow() + 1);
        Optical *Optical_pkt_temp =  check_and_cast<Optical *>(msg);

        PreviousToR = Optical_pkt_temp->getPreviousToR() ;

        NextToR = Optical_pkt_temp->getNextToR() ;

        TxIndex = Optical_pkt_temp->getTxIndex();
        GroupToRIndex = Optical_pkt_temp->getGroupToRIndex();

        InputPortNum  = (PreviousToR/CoreRadix)*NumTx_ToR2ToR + TxIndex;

        OutputPortNum = (NextToR/CoreRadix)*NumTx_ToR2ToR + (PreviousToR/CoreRadix)/GroupToRCore;;
        CurrentOPSIndex = PreviousToR%CoreRadix;
        slot_OPS_MSG->setDst_ToR(CurrentOPSIndex*OPSPortNum + OutputPortNum); //set output port
        scheduleAt(simTime() + slot_OPS,slot_OPS_MSG->dup() );
        int optNum = Optical_pkt_temp->getOptical_num();
        app_pkt_num = Optical_pkt_temp->getApp_pkt_num();
        /////////////////////////////////////////////////////
        //int tt = getIndex();
                ////////////////////////////////////////////////////////////////////////////////////////
        if(Core_Channel_busy[CurrentOPSIndex*OPSPortNum +OutputPortNum] == false){

            Core_Channel_busy[CurrentOPSIndex*OPSPortNum +OutputPortNum] = true;

            SendLaterCore.insert(Optical_pkt_temp);

            pkt *AckFromCore = new pkt("AckFromCore");

            AckFromCore->setSrc_ToR(PreviousToR);
            AckFromCore->setDst_ToR(NextToR);
            AckFromCore->setTxIndex(TxIndex);
            AckFromCore->setGroupToRIndex(GroupToRIndex);
            AckFromCore->setIndex(10);
            AckFromCore->setKind(10);// from Agre, ACK Index is 8;
            AckFromCore->setRetransmission(0);
            AckFromCore->setSchedulingPriority(0);
            AckFromCore->setOptNum(optNum);
            AckFromCore->setApp_pkt_num(app_pkt_num);
            sendDelayed(AckFromCore,delayforACK,"ackout",InputPortNum);
            gate("ackout",InputPortNum)->getTransmissionChannel()->forceTransmissionFinishTime(simTime());
            finish_time= gate("ackout",InputPortNum)->getTransmissionChannel()->getTransmissionFinishTime();

            local_CorePkt_MSG->setDst_ToR(NextToR);
            scheduleAt(simTime() + dealy_MSG,local_CorePkt_MSG->dup());

        }
        else{
             delete msg;
             pkt *AckFromCore  = new pkt("local_pkt_ToR");
             AckFromCore ->setSrc_ToR(PreviousToR);
             AckFromCore ->setDst_ToR(NextToR);
             AckFromCore->setTxIndex(TxIndex);
             AckFromCore->setGroupToRIndex(GroupToRIndex);
             AckFromCore ->setIndex(10);
             AckFromCore->setKind(10);
             AckFromCore ->setRetransmission(1);
             AckFromCore ->setSchedulingPriority(0);
             AckFromCore->setOptNum(optNum);
             AckFromCore->setApp_pkt_num(app_pkt_num);
             sendDelayed(AckFromCore ,delayforACK,"ackout",InputPortNum);
             gate("ackout",InputPortNum)->getTransmissionChannel()->forceTransmissionFinishTime(simTime());
        }
    }

    else if(msg->getKind() == 0){


        if(SendLaterCore.length() > 0){
            Optical * Optical_pkt_to_send = (Optical *)SendLaterCore.pop();  //1111


            PreviousToR = Optical_pkt_to_send->getPreviousToR() ;

            NextToR = Optical_pkt_to_send->getNextToR() ;

            TxIndex = Optical_pkt_to_send->getTxIndex();
            GroupToRIndex = Optical_pkt_to_send->getGroupToRIndex();


            InputPortNum  = (PreviousToR/CoreRadix)*NumTx_ToR2ToR + TxIndex;

            OutputPortNum = (NextToR/CoreRadix)*NumTx_ToR2ToR + (PreviousToR/AgreRadix)/GroupToRCore;

//            int countNow = Optical_pkt_to_send->getPkt1()->getHopCountNow();
//            EV<< "in FOS , countNow is" <<countNow;
//            EV<< "in FOS , HopNow is" <<Optical_pkt_to_send->getPkt1()->getHop(countNow);

//            AGG_Channel_busy[OutputPortNum] = false;  // set to false???

            int optNum = Optical_pkt_to_send->getOptical_num();
            for(int i=0;i<optNum;i++){
                switch(i){
                    case 0:
                    {
                        int countNow1 =  Optical_pkt_to_send->getPkt1()->getHopCountNow();
                        Optical_pkt_to_send->getPkt1()->setHopCountNow(countNow1+1);
                    }
                        break;
                    case 1:
                    {
                        int countNow2 =  Optical_pkt_to_send->getPkt2()->getHopCountNow();
                        Optical_pkt_to_send->getPkt2()->setHopCountNow(countNow2+1);
                    }
                        break;
                    case 2:
                    {
                        int countNow3 =  Optical_pkt_to_send->getPkt3()->getHopCountNow();
                        Optical_pkt_to_send->getPkt3()->setHopCountNow(countNow3+1);
                    }
                        break;
                    case 3:
                    {
                        int countNow4 =  Optical_pkt_to_send->getPkt4()->getHopCountNow();
                        Optical_pkt_to_send->getPkt4()->setHopCountNow(countNow4+1);
                    }
                        break;
                    case 4:
                    {
                        int countNow5 =  Optical_pkt_to_send->getPkt5()->getHopCountNow();
                        Optical_pkt_to_send->getPkt5()->setHopCountNow(countNow5+1);
                    }
                        break;
                }
            }

            send(Optical_pkt_to_send,"out",OutputPortNum);

            finish_time= gate("out",OutputPortNum)->getTransmissionChannel()->getTransmissionFinishTime();
            EV<<"For Core, msg finish time is "<< finish_time <<"\n";

            delete msg;
        }
    }
    else if(msg->getKind() == 400){
        pkt *pkt_temp =  check_and_cast<pkt*>(msg);
        int kk = pkt_temp->getDst_ToR();
        Core_Channel_busy[kk] = false;  // set to false
        delete msg;
    }


}
