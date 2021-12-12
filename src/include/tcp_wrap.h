#ifndef TCP_WRAP_H
#define TCP_WRAP_H
#include <fcntl.h>

#include<cstddef>
// htonl, htons, ntohl, ntohs 
// convert values between host and network byte order
#include<arpa/inet.h>
// socket() bind() listen() accept() connect()
// recvfrom() sendto()
// recv() send()
#include<sys/socket.h> 
// struct sockaddr_in
#include<netinet/in.h>
// close()
#include<unistd.h>
#include"constants.h"

/* #include<sys/wait.h> */

#include"validate_inline.h"

class Tcp_Wrap
{
	public:
		Tcp_Wrap()
			:ipv4_addr_(new char[20]),fd_(-1),port_(-1),status_(ORIGINAL)
		{};
		~Tcp_Wrap()
		{
			delete [] ipv4_addr_;
		};
		int Listen(const int max_listen_queue=MAX_LISTEN_QUEUE);

		int Accept();
		// actively connect
		int Connect(const char* ip, const int port);
		int GetFd()const {return fd_;};
		// send len length of data, and return successfully sent length, -1 for errors 
		/* int Send(const char* data, const std::size_t len); */
		// set ip and port, ip should be v4 and terminate with '\0'
		int SetAddr(const char* ip, const int port);


		// connect status check
		/* bool IsConnected()const {return connected_;} */
	private:
		int fd_; // self fd
		int op_fd_; // only valid when actively connect
		char* ipv4_addr_;
		int port_;
		int status_;
		enum{
			ORIGINAL=0,
			OCCUPYFD,
			INITADDR,
			LISTENING,
			CONNECTED,
			DISCONNECTED
		};
};



#endif
