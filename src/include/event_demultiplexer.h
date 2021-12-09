#ifndef EVENT_DEMUTIPLEXER_H
#define EVENT_DEMUTIPLEXER_H

#include<map>

class EventDemultiplexer
{
	public:
		EventDemultiplexer(){}
		virtual ~EventDemultiplexer(){}
		virtual int wait_event(
					std::map<Handle, typename _Tp, typename _Compare, typename _Alloc>				
				)
}


#endif
