#ifndef THREAD_FUNC_H
#define THREAD_FUNC_H

#include<pthread.h>


// create an Acceptor, an IOReceiver, 3 Worker, (num of SigMechines) HBeatKeeper (each type one HBeatKeeper)
// then pause (later we may add dynamic manager threads functionalities)
void Leader();

// no epoll
// accept blocking mode
void Acceptor();

// Type: tcp receiver, raw_udp receiver
// epoll receive from 
void IOReceiver();


// general worker , serve all type of SigMechines and Clients
// Get msg from a task queue which shared by multi worker and one IOReceiver
// Write back clients Request or active Warnings, Errors
void Worker();


// Type: raw_udp keeper, snmp keeper
// raw_udp keeper send heart beat request only, and this request have to be after waiting_for_response being appended
// snmp keeper send and receive heartbeat because of the net-snmp api features. judge if disconnected on its own thread
// one HBeatKeeper act as only one type of HBeatKeeper
// SigMachines all use udp or snmp, so HBeatKeeper should be create by Leader
void HBKeeper();

#endif
