#include "thread_func.h"
#include "task_queue.h"
#include "config_manager.h"
#include "constants.h"
#include"tcp_wrap.h"
#include "newfd_queue.h"
#include "map"
#include <sys/epoll.h>
#include <algorithm>
#include <iostream>


extern pthread_mutex_t IOR_After_Valid_Mutex;
extern pthread_cond_t IOR_After_Valid_Condi;
/* extern pthread_mutex_t IOR_After_Valid_Mutex; */


void* Leader(void *arg)
{
	pthread_mutex_init(&IOR_After_Valid_Mutex, nullptr);
	
	pthread_t thrd;
	pthread_create(&thrd,nullptr,Acceptor,nullptr);
	pthread_create(&thrd,nullptr,IOReceiver,nullptr);
	pthread_create(&thrd,nullptr,Worker,nullptr);
	pthread_create(&thrd,nullptr,Worker,nullptr);
	pthread_create(&thrd,nullptr,Worker,nullptr);

	Machine machineNumConf = ConfigManager::GetInstance().GetSigMachineNum();	
	if(machineNumConf.XINGZHI>0)
	{
		const MachineInfo *info=
			ConfigManager::GetInstance().GetMachineInfoList(Machine::XINGZHI);
		pthread_create(&thrd,nullptr,HBeatKeeper,
				static_cast<void*>(const_cast<MachineInfo*>(info)));
	}
	if(machineNumConf.HAIXIN>0)
	{
		const MachineInfo *info=
			ConfigManager::GetInstance().GetMachineInfoList(Machine::HAIXIN);
		pthread_create(&thrd,nullptr,HBeatKeeper,
				static_cast<void*>(const_cast<MachineInfo*>(info)));
	}
	if(machineNumConf.HAIKANG>0)
	{
		const MachineInfo *info=
			ConfigManager::GetInstance().GetMachineInfoList(Machine::HAIKANG);
		pthread_create(&thrd,nullptr,HBeatKeeper,
				static_cast<void*>(const_cast<MachineInfo*>(info)));
	}
	if(machineNumConf.XIMENZI>0)
	{
		const MachineInfo *info=
			ConfigManager::GetInstance().GetMachineInfoList(Machine::XIMENZI);
		pthread_create(&thrd,nullptr,HBeatKeeper,
				static_cast<void*>(const_cast<MachineInfo*>(info)));
	}
	if(machineNumConf.DEVICE5>0)
	{
		const MachineInfo *info=
			ConfigManager::GetInstance().GetMachineInfoList(Machine::DEVICE5);
		pthread_create(&thrd,nullptr,HBeatKeeper,
				static_cast<void*>(const_cast<MachineInfo*>(info)));
	}

}

inline ConnectionInfo* Fd2ConnectionInfo(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	getpeername(fd,(struct sockaddr*)&addr , &addr_len);
	ConnectionInfo *connInfo = new ConnectionInfo;

	connInfo->fd=fd;
	connInfo->status=ConnectionStatus::CONNECTED;
	connInfo->type=ConnectionType::TCP;
	connInfo->port=ntohs(addr.sin_port);
	char *str = inet_ntoa(addr.sin_addr);
	strcmp(connInfo->ipv4_addr,str);
	return connInfo;
}


void* Acceptor(void* arg)
{
	Tcp_Wrap acceptor;
	acceptor.SetAddr(ConfigManager::GetInstance().GetListenIP(),
			ConfigManager::GetInstance().GetListenPort());
	acceptor.Listen();
	pthread_cond_signal(&IOR_After_Valid_Condi);
	
	int client_fd=-1;

	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	while(true)
	{
		client_fd = acceptor.Accept();
		ConnectionInfo* connInfo= Fd2ConnectionInfo(client_fd);
		NewConnQ::getInstance().Push(connInfo);
	}
}


void HandleRead(ConnectionInfo* conn)
{
	TaskInfo* task=new TaskInfo;
	char* buffer = new char[1024];
	int buffer_len=0;

	// read it in buffer
	
	task->buffer=buffer;
	task->buffer_len=buffer_len;
	task->conn = conn;	
	task->type = TASKTYPE::TCPPACKET;
	TaskQ::getInstance().Push(task);
}
void HandleErr(ConnectionInfo* conn)
{
	close(conn->fd);
}

void* IOReceiver(void *arg)
{
	std::map<const char*,ConnectionInfo*> ipv4_conn_map;	
	int EpFd = epoll_create(1024);
	struct epoll_event ev;
	ev.events=EPOLLET|EPOLLIN;

	struct epoll_event activeEvs[1024];

	while(true)
	{
		ConnectionInfo* newConn=NewConnQ::getInstance().Pop();
		ipv4_conn_map.insert(std::make_pair(newConn->ipv4_addr,newConn));
	
		for(const auto& [key, value]: ipv4_conn_map)
		{
				int activeNum = epoll_wait(EpFd,activeEvs,1024,-1);
		
				for(int i=0;i<activeNum;i++)
				{
					ConnectionInfo* conn = static_cast<ConnectionInfo*>(activeEvs[i].data.ptr);
					int events = activeEvs[i].events;

					if(events & EPOLLIN)
					{
						HandleRead(conn);	
					}else if (events & EPOLLERR)
					{
						HandleErr(conn);
					}else{
						std::cout<<"Unknown Events"<<std::endl;
					}
				}

		}	
	}


	
}


void* Worker(void *arg)
{
	TaskInfo& task= TaskQ::getInstance().Get();
	
}



void* HBeatKeeper(void *arg)
{

}
