#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include"event_handler.h"
#include<sys/epoll.h>
#include<cstring>
#include"errhandle.h"

class EventLoop
{
	public:
		// listenfd need to be non_blocking
		EventLoop(int listenfd)
		:handler(nullptr),epollfd_(-1),listen_fd_(listenfd)
		{
			epollfd_ = epoll_create(1024); // maybe need change
			if(listen_fd_<0)
			{
				pFatalError("RunLoop: Invalid listen fd\n");
			}
			struct epoll_event ev;
			memset(&ev, 0, sizeof(ev));
			ev.events = EPOLLIN|EPOLLET;
			ev.data.fd=listen_fd_;
			if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, listen_fd_, &ev)<0)	
			{
				pFatalErrno(errno);
			}
		};
		~EventLoop(){
			if(handler!=nullptr) delete handler;
		};
		int SetHandler(std::function<int(int)> accept_handler,
				std::function<int(int)> read_handler,
				std::function<int(int)> write_handler )
		{
			handler = new EventHandler(accept_handler,
					read_handler,write_handler);
		};
		int RunLoop()
		{
			struct epoll_event activeEvs[100]; // how many fits best ?
			int maxevents=100;
			int num{0};
			while(true)
			{
				num = epoll_wait(epollfd_,activeEvs,maxevents,-1);// timeout may need change
				for(int i=0; i<num; i++)
				{
					/* if() */
				}
				
			}
		}
		
	private:
		int epollfd_;
		int listen_fd_;
		EventHandler* handler;
};

#endif
