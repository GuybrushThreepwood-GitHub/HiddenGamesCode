
#ifndef __PLATFORMNETWORKBASE_H__
#define __PLATFORMNETWORKBASE_H__

const int DEFAULT_MAX_CLIENTS = 32;
const int DEFAULT_READWRITESIZE = 1024*8;

#if !defined(BASE_PLATFORM_WINDOWS)
	#include <sys/types.h>
	#include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <sys/unistd.h>
    #include <sys/fcntl.h>

	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h> 

	#define SOCKET				int
	#define INVALID_SOCKET		-1
	#define SOCKET_ERROR		-1
#else

#endif

#endif // __PLATFORMNETWORKBASE_H__


