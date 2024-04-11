
#ifndef __NETWORKBASE_H__
#define __NETWORKBASE_H__

#ifdef BASE_SUPPORT_NETWORKING

#ifndef __PLATFORMNETWORKBASE_H__
	#include "Network/PlatformNetworkBase.h"
#endif // __PLATFORMNETWORKBASE_H__

#ifndef __BASESOCKET_H__
	#include "Network/BaseSocket.h"
#endif // __BASESOCKET_H__

#ifndef __SOCKADDR_H__
	#include "Network/SockAddr.h"
#endif // __SOCKADDR_H__

#ifndef __NETWORKFUNCTIONS_H__
	#include "Network/NetworkFunctions.h"
#endif // __NETWORKFUNCTIONS_H__

#ifndef __TCPCLIENT_H__
	#include "Network/TCPClient.h"
#endif // __TCPCLIENT_H__

#ifndef __TCPSERVER_H__
	#include "Network/TCPServer.h"
#endif // __TCPSERVER_H__

#endif // BASE_SUPPORT_NETWORKING

#endif // __NETWORKBASE_H__

