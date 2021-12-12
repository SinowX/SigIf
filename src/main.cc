/* #include"tcp_wrap.h" */
/* // shared_ptr */
/* #include <memory> */
/* #include <cstdlib> */

/* int main() */
/* { */
/* 	std::shared_ptr<Tcp_Wrap> server_ptr = std::make_shared<Tcp_Wrap>(); */
/* 	if(server_ptr->SetAddr("127.0.0.1", 5500)==RETURN::ERROR) */
/* 		abort(); */	
/* 	int newclient_fd=0; */
/* 	newclient_fd = server_ptr->Accept(); */
	
/* } */



// read config file, get
// WorkLoop nums,
// Signal Machine Types , Nums and other relative configurations like each machine location infos
// Logging level, Logging dir
//
// declare global mutex, condi vars
// create all needed threads, 
// 	let them hang up (except for logging thread)
// 	at start with mutex
// later main thread can wake a certain work thread with condi vars
// main thread stores all connected fd to a global vars
// 	and protect them with mutex
// main thread listen only a server listening fd for connection
// 	when new connection comes, wrap the new fd with a object for 
// 	mutlti-threading security, and then add it to a workloop 
// 	serve-queue
// workloop thread pop this object, now add it to corresponding
// 	heart-beat maintainer
// workloop thread and heart-beat maintainer both access
//  this connection, so we need mutex to protect its fd
// if heart-beat maintainer detect one connection is dead,
// 	send info to corresponding workloop by append to a workloop queue
// so next loop before event wait, workloop could check it up and drop 			it and return Warning to corresponding client 
// main thread init Communication Protocal Wrapper
//

#include <sys/epoll.h>
#include"config_manager.h"
#include"tcp_wrap.h"

#define ConfMgr ConfigManager::GetInstance()


int main()
{
	
	ConfigManager::GetInstance().ReadConfig("path to config");
	Tcp_Wrap acceptor;
	acceptor.SetAddr(ConfMgr.GetListenIP(), ConfMgr.GetListenPort());
	int EpFd=epoll_create(1024);
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events=EPOLLIN;
	ev.data.fd=acceptor.GetFd();
	ev.data.ptr=static_cast<void*>(&acceptor);
	epoll_ctl(EpFd, EPOLL_CTL_ADD, acceptor.GetFd(), &ev);

	struct epoll_event activeEvs[100];

	int active_num = epoll_wait(EpFd,activeEvs,100,30);

	for(int i=0; i<active_num; i++)
	{
		int fd=activeEvs[i].data.fd;
		int event = activeEvs[i].events;
		if(events )
	}
	return 0;
}
