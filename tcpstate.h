/*******************************************************************************
 * Project part B
 *
 * Authors: Jason Skicewicz
 *          Kohinoor Basu
 *
 * TCPState class that performs low level TCP functionality
 *
 ******************************************************************************/

#ifndef _tcpstate
#define _tcpstate

#include <iostream>
#include "Minet.h"

// The following two constants provide the proper sequence increments for an MSL
//  of 2 mins
const unsigned int SECOND_MULTIPLIER=35791394; // (1s * 2^32) / 120s
const unsigned int MICROSEC_MULTIPLIER=36;     // (1us * 2^32) / 120s
const unsigned int MSL_TIME_SECS=120;          // MSL time is 2 minutes
const unsigned int NUM_SYN_TRIES=8;            // Send SYN 8x before fail (~80secs)
const unsigned int SEQ_LENGTH_MASK=0xFFFFFFFF; // Masks off first 32 bits

const unsigned int TCP_MAXIMUM_SEGMENT_SIZE=536;

enum eState {CLOSED, LISTEN, SYN_SENT, SYN_SENT1, SYN_RCVD, ESTABLISHED, CLOSE_WAIT,
	     LAST_ACK, FIN_WAIT1, FIN_WAIT2, CLOSING, TIME_WAIT};

class TCPState {
 public:
    unsigned int stateOfcnx;
    unsigned int tmrTries;

    // Sender side
    unsigned int last_acked, last_sent;
    unsigned short rwnd;
    Buffer SendBuffer;

    //Receiver side
    unsigned int last_recvd;
    Buffer RecvBuffer;

    unsigned int TCP_BUFFER_SIZE; //Buffer size of SendBuffer and RecvBuffer
    unsigned int N; //Window size

    TCPState();
    TCPState(unsigned int initialSequenceNum, unsigned int state, unsigned int timertries);
    virtual ~TCPState();

    unsigned int GetN();

    void SetState(unsigned int newstate);
    unsigned int GetState();

    void SetTimerTries(unsigned int numtries);
    bool ExpireTimerTries();

    bool SetLastAcked(unsigned int newack);
    unsigned int GetLastAcked();

    void SetLastSent(unsigned int lastsent);
    unsigned int GetLastSent();

    void SetSendRwnd(unsigned short window);
    unsigned int GetRwnd();
    
    void SetLastRecvd(unsigned int lastrecvd);
    bool SetLastRecvd(unsigned int lastrecvd, unsigned int length);
    unsigned int GetLastRecvd();

    void SendPacketPayload(unsigned &offsetlastsent, size_t &bytesize, unsigned bytes); 

    ostream & Print(ostream &os) const { os <<"TCPState(stateOfcnx=" << stateOfcnx
					  << ", last_acked=" << last_acked
					  << ", last_sent=" << last_sent
					  << ", rwnd=" << rwnd
					  << ", last_recvd=" << last_recvd
					  << ")"; return os;}
};
#endif _tcpstate
