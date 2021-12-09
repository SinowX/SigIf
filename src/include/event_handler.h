#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include<functional>

class EventHandler
{
	public:
		EventHandler(
				std::function<int(int)> accept_handler,
				std::function<int(int)> read_handler,
				std::function<int(int)> write_handler )
			:handle_accept_(accept_handler),
			handle_read_(read_handler),
		 	handle_write_(write_handler)
		{};
		~EventHandler();

		int Handle_Accept(int fd)
		{return handle_accept_(fd);};
		int Handle_Read(int fd)
		{return handle_read_(fd);};
		int Handle_Write(int fd)
		{return handle_write_(fd);};
	private:
		std::function<int(int)> handle_accept_;
		std::function<int(int)> handle_read_;
		std::function<int(int)> handle_write_;

};

#endif
