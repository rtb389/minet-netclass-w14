#include <sys/time.h>
#include <sys/types.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <unistd.h>
#include "netinet/in.h"

#include <iostream>


#include "Minet.h"


int main(int argc, char * argv[])
{
  MinetHandle dd, ip, other, arp;

  MinetInit(MINET_ETHERNET_MUX);
  
  dd=MinetIsModuleInConfig(MINET_DEVICE_DRIVER) ? MinetConnect(MINET_DEVICE_DRIVER) : MINET_NOHANDLE;
  arp=MinetIsModuleInConfig(MINET_ARP_MODULE) ? MinetAccept(MINET_ARP_MODULE) : MINET_NOHANDLE;
  ip=MinetIsModuleInConfig(MINET_IP_MODULE) ? MinetAccept(MINET_IP_MODULE) : MINET_NOHANDLE;
  other=MinetIsModuleInConfig(MINET_OTHER_MODULE) ? MinetAccept(MINET_OTHER_MODULE) : MINET_NOHANDLE;

  if (dd==MINET_NOHANDLE && MinetIsModuleInConfig(MINET_DEVICE_DRIVER)) {
    MinetSendToMonitor(MinetMonitoringEvent("Can't connect to device driver"));
    cout << "Can't connect to device driver" << endl;
    return -1;
  }
  if (arp==MINET_NOHANDLE && MinetIsModuleInConfig(MINET_ARP_MODULE)) {
    MinetSendToMonitor(MinetMonitoringEvent("Can't accept from arp module"));
    cout << "Can't accept from arp module" << endl;
    return -1;
  }
  if (ip==MINET_NOHANDLE && MinetIsModuleInConfig(MINET_IP_MODULE)) {
    MinetSendToMonitor(MinetMonitoringEvent("Can't accept from ip module"));
    cout << "Can't accept from ip module" << endl;
    return -1;
  }
  if (other==MINET_NOHANDLE && MinetIsModuleInConfig(MINET_OTHER_MODULE)) {
    MinetSendToMonitor(MinetMonitoringEvent("Can't accept from other module"));
    cout << "Can't accept from other module" << endl;
    return -1;
  }


  MinetSendToMonitor(MinetMonitoringEvent("ethernet_mux operating"));
  cout << "ethernet_mux operating" << endl;

  MinetEvent event;

  int rc;

  while (MinetGetNextEvent(event)==0) {
    if (event.eventtype!=MinetEvent::Dataflow 
	|| event.direction!=MinetEvent::IN) {
      MinetSendToMonitor(MinetMonitoringEvent("Unknown event ignored."));
      cout << "Unknown event ignored." << endl;
    } else {
      if (event.handle==dd) {
	RawEthernetPacket raw;
	unsigned short type;
	MinetReceive(dd,raw);
	memcpy((char*)(&type),&(raw.data[12]),2);
	type=ntohs(type);
	switch (type) {
	case PROTO_ARP:
	  MinetSend(arp,raw);
	  break;
	case PROTO_IP:
	  MinetSend(ip,raw);
	  break;
	default:
	  MinetSend(other,raw);
	  break;
	}
      }
      if (event.handle==arp) {
	RawEthernetPacket p;
	MinetReceive(arp,p);
	cout << "Writing out ARP Packet: " << p << endl;
	MinetSend(dd,p);
      }
      if (event.handle==ip) {
	RawEthernetPacket p;
	MinetReceive(ip,p);
	MinetSend(dd,p);
      }
      if (event.handle==other) {
	RawEthernetPacket p;
	MinetReceive(other,p);
	MinetSend(dd,p);
      }
    }
  }
}





