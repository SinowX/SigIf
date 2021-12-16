#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "connection_info.h"
#include <memory>
#include <string>

enum TASKTYPE
{
	NEWCLIENT=0,
	TCPPACKET,
	UDPPACKET
};

typedef struct TaskInfo
{
	std::shared_ptr<ConnectionInfo>conn;
	int type; // TASKTYPE
	/* std::shared_ptr<std::string> buffer; */
	std::string buffer;
	/* char* buffer; */
	/* int buffer_len; */
}TaskInfo;


class TaskQ
{
	public:
		~TaskQ(){};
		TaskQ(TaskQ&)=delete;
		void operator=(TaskQ&)=delete;
		
		static TaskQ& getInstance()
		{
			static TaskQ instance_;
			return instance_;
		}
		int Push(std::shared_ptr<TaskInfo> task);
		std::shared_ptr<TaskInfo> Pop();
		int Size()const;
	private:
		TaskQ(){};

};

#endif
