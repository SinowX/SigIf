#include "handler.h"

int HandleRead(std::shared_ptr<ConnectionInfo> conn)
{
	char buffer[4096];
	ssize_t n = 0;
	while((n = read(conn->fd, buffer, 4096))>0)
	{
		// conn->buffer need lock to avoid race condition
		conn->buffer.append(buffer,n);
	}

	/* TaskInfo* task = new TaskInfo; */
	std::shared_ptr<TaskInfo> task = std::make_shared<TaskInfo>();
	task->buffer=conn->buffer;
	task->conn=conn;
	task->type=TASKTYPE::TCPPACKET;
	
	TaskQ::getInstance().Push(task);

}
int HandleError(std::shared_ptr<ConnectionInfo> conn)
{

}
