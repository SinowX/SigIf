#ifndef NEWFD_QUEUE_H
#define NEWFD_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <memory>
#include "connection_info.h"
#include <queue>


class NewConnQ
{
	public:
		~NewConnQ()=default;
		NewConnQ(NewConnQ&)=delete;
		void operator=(NewConnQ&)=delete;	
		
		static NewConnQ& getInstance()
		{
			static NewConnQ newConnQ_;
			return newConnQ_;
		}

		int Push(const std::shared_ptr<ConnectionInfo>conn )
		{
			std::lock_guard<std::mutex> lock(mtx_);
			ConnQ_.push(conn);
			if(ConnQ_.size()==1)
			{

			}
			return ConnQ_.size();
		}
		
		std::shared_ptr<ConnectionInfo> Pop()
		{
			if(Size()==0)
			{
				
			}
			std::lock_guard<std::mutex> lock(mtx_);
			std::shared_ptr<ConnectionInfo> conn=ConnQ_.front();
			ConnQ_.pop();
			return conn;
		}

		int Size()
		{
			std::lock_guard<std::mutex> lock(mtx_);
			return ConnQ_.size();
		}

	private:
		std::queue<std::shared_ptr<ConnectionInfo>, std::deque<std::shared_ptr<ConnectionInfo>>> ConnQ_;
		/* std::queue<ConnectionInfo*,std::deque<ConnectionInfo*>> ConnQ_; */
		NewConnQ(){}
		
		std::mutex mtx_;
		std::condition_variable condi_;

};

#endif
