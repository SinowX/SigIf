#include"tcp_wrap.h"
#include"errhandle.h"

int Tcp_Wrap::SetAddr(const char *ip, const int port)
{
	/* if(status_==CONNECTED) */
	/* { */
	/* 	pWarningError("SetAddr: connection established."); */
	/* 	return RETURN::ERROR; */
	/* } */
	// close fd_ to avoid leaking fds
	if(status_==DISCONNECTED||status_==INITADDR||status_==OCCUPYFD)
	{
		close(fd_);
	}
	if(IsValidIpv4Length(ip))
	{
		pWarningError("SetAddr: ip too long\n");
		return RETURN::ERROR;
	}
	if(IsValidPortNumber(port))
	{
		pWarningError("SetAddr: invalid port\n");
		return RETURN::ERROR;
	}
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(fcntl(fd_,F_SETFD, O_NONBLOCK)<0)
	{
		pWarningErrno(errno);
		close(fd_);
		return RETURN::ERROR;
	}
	if(fd_<0)
	{
		pWarningErrno(errno);
		return RETURN::ERROR;
	}
	status_=OCCUPYFD;

	strcpy(ipv4_addr_, ip);
	port_ = port;	
	struct sockaddr_in addr_struct;
	memset(&addr_struct, 0, sizeof(addr_struct));
	addr_struct.sin_family = AF_INET;
	addr_struct.sin_port = htons(port_);
	addr_struct.sin_addr.s_addr = inet_addr(ipv4_addr_);
	if(bind(fd_,(struct sockaddr*)&addr_struct, sizeof(addr_struct))<0)
	{
		pWarningErrno(errno);
		return RETURN::ERROR;
	}
	status_=INITADDR;
	return true;
}

int Tcp_Wrap::Listen(const int max_listen_queue)
{
	if(status_!=INITADDR)
	{
		pWarningError("Listen: should invoke after SetAddr()\n");
		return RETURN::ERROR;
	}
	if(listen(fd_, max_listen_queue)<0)
	{
		pWarningErrno(errno);
		return RETURN::ERROR;
	}
	status_=LISTENING;
	return true;
}


int Tcp_Wrap::Accept()
{
	if(status_!=LISTENING)
	{
		pWarningError("Accept: should invoke after Listen()\n");
		return -1;
	}
	int client_fd;
	client_fd = accept(fd_,NULL,NULL);
	if(client_fd<0)
	{
		pWarningErrno(errno);
		return RETURN::ERROR;
	}
	// set unblock
	if(fcntl(client_fd,F_SETFD, O_NONBLOCK)<0)
	{
		pWarningErrno(errno);
		close(client_fd);
		return RETURN::ERROR;
	}
	return client_fd;
}

int Tcp_Wrap::Connect(const char* ip, const int port)
{
	if(status_!=ORIGINAL)
	{
		pWarningError("Connect: should invoke after construction\n");
		return RETURN::ERROR;
	}
	if(IsValidIpv4Length(ip))
	{
		pWarningError("SetAddr: ip too long\n");
		return RETURN::ERROR;
	}
	if(IsValidPortNumber(port))
	{
		pWarningError("SetAddr: invalid port\n");
		return RETURN::ERROR;
	}
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_<0)
	{
		pWarningErrno(errno);
		return RETURN::ERROR;
	}
	status_=OCCUPYFD;
	
	struct sockaddr_in addr_struct;
	memset(&addr_struct, 0, sizeof(addr_struct));
	addr_struct.sin_family = AF_INET;
	addr_struct.sin_port = htons(port);
	addr_struct.sin_addr.s_addr = inet_addr(ip);

	op_fd_ = connect(fd_, (struct sockaddr*)&addr_struct, sizeof(addr_struct));
	if(op_fd_<0)
	{
		pWarningErrno(errno);
		return RETURN::ERROR;
	}
	// set unblock	
	if(fcntl(op_fd_,F_SETFD, O_NONBLOCK)<0)
	{
		pWarningErrno(errno);
		close(op_fd_);
		return RETURN::ERROR;
	}
	return op_fd_;
}


