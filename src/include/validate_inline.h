#ifndef VALIDATE_INLINE_H
#define VALIDATE_INLINE_H
#include <cstring>

inline bool IsValidIpv4Length(const char* ip)
{
	return strlen(ip)<=16;
}

inline bool IsValidPortNumber(const int port)
{
	return port>0;
}



#endif
