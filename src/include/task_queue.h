#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "connection_info.h"

enum TASKTYPE
{
	NEWCLIENT=0,
	TCPPACKET,
	UDPPACKET
};

typedef struct TaskInfo
{
	ConnectionInfo *conn;
	int type; // TASKTYPE
	char* buffer;
	int buffer_len;
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
		int Push(const TaskInfo* task);
		TaskInfo& Get();
		int Size()const;
	private:
		TaskQ(){};

};

#endif
