//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

//
// TODO generated message class
//
packet pkt {
    int src_Server;
    int dst_Server;
    
    int src_ToR;
    int dst_ToR;
    
    int index;
    
    int TotalHopCount; // the direct ToR is 1
    int HopCountNow;
    int Hop[8];
//    int src_AGG;
//    int dst_AGG;
//    int core;


//	int Agre[];
//	int Core[];
	int PacketSize;
    int no;
    int m;
    
    int retransmission;
//    double st_AGG;
    int path;
    
    int TxIndex;
    int GroupToRIndex;
    double End2EndStartTime;
    
    simtime_t PacketNetworkStartTime;
    simtime_t PacketEnd2EndStartTime;
    int flag_intraToR;
    
    int flag_ToR2ToR;
    
    int optNum;
    
    int flag_last;
    
    int flag_app;
    int app_pkt_num;
}