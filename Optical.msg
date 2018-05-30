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
cplusplus {{ 
#include "pkt_m.h"
#include <list>
typedef pkt * pkt_Ptr;
typedef std::list<pkt *> PktVector;

 }} // C++ typedef
class noncobject pkt_Ptr; 
class noncobject PktVector;
packet pkt;

packet Optical {
    
    //pkt_Ptr  pkt1;
    pkt_Ptr  pkt2;
    pkt_Ptr  pkt3;
    pkt_Ptr  pkt4;
    pkt_Ptr  pkt5;
    //pkt  pkt1;
    //pkt  pkt2;
    //pkt  pkt3;
    //pkt  pkt4;
    //pkt  pkt5;
    pkt_Ptr pkt1;
    int PreviousToR;
    int NextToR;
    int TxIndex;
    int GroupToRIndex;
	int optical_num;
	int index;
	PktVector Optical_pkts;
	int app_pkt_num;
//	cQueue x;
}