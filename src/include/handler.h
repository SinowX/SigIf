#ifndef HANDLER_H
#define HANDLER_H
#include <memory>
#include "connection_info.h"
#include "task_queue.h"
#include <unistd.h>


int HandleRead(std::shared_ptr<ConnectionInfo> conn);
int HandleError(std::shared_ptr<ConnectionInfo> conn);

#endif
