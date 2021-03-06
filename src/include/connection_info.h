#ifndef CONNECTION_INFO_H
#define CONNECTION_INFO_H

#include <utility>
/* #include <vector> */
#include <string>


enum ConnectionType
{
	TCP=1,
	UDP,
	SNMP
};

enum ConnectionStatus
{
  CONNECTED=1,
  DISCONNECTED
};

typedef struct ConnectionInfo
{
	int type;
	// end with '\0'
	char ipv4_addr[16];
	int port;
	int fd;
	int status;
	std::string buffer;
}ConnectionInfo;

typedef std::pair<const char*, ConnectionInfo*> Pair;


#endif
