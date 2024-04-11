
/*===================================================================
	File: TCPServer.cpp
	Library: NetworkLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_NETWORKING

#include "CoreBase.h"

#include "Network/PlatformNetworkBase.h"
#include "Network/SockAddr.h"
#include "Network/BaseSocket.h"
#include "Network/TCPClient.h"

#include "Network/TCPServer.h"

using network::TCPServer;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
TCPServer::TCPServer()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
TCPServer::~TCPServer()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void TCPServer::Initialise()
{
	ServerCallback = 0;
	m_ServerStarted = false;

	m_ClientList.clear();
	m_NumActiveClients = 0;
	m_MaxNumClients = 0;
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void TCPServer::Release()
{
	m_ServerStarted = false;

	if( m_ListenSocket.IsOpen() )
		m_ListenSocket.Close();
							
	// disconnect clients
	std::size_t i=0;
	for( i=0; i < m_ClientList.size(); i++ )
	{
		// check for connection
		if( m_ClientList[i]->IsOpen() )
			m_ClientList[i]->Close();

		delete m_ClientList[i];
	}

	// clear the list
	m_ClientList.clear();
	m_NumActiveClients = 0;
	m_MaxNumClients = 0;
}

/////////////////////////////////////////////////////
/// Method: StartServer
/// Params: [in]nPort, [in]nMaxNumClients, [in]FuncCallback
///
/////////////////////////////////////////////////////
int TCPServer::StartServer( int port, unsigned int maxNumClients, void (*FuncCallback)( void *pData, int size, int packetID ) )
{
	// server is already started
	if( m_ServerStarted )
	{
		DBGLOG( "TCPSERVER: *WARNING* server is already started\n" );
		return 1;
	}

	// create the server socket
	m_ListenSocket.Close();
	m_ListenSocket.Open( network::BaseSocket::SocketType_TCP );

	if( m_ListenSocket.IsOpen() == SOCKET_ERROR )
	{
		DBGLOG( "TCPSERVER: *ERROR* Could not create server listening socket\n" );
		return 1;
	}
    
	int reuseAddr = 1;
	if( m_ListenSocket.SetSockOpt( SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseAddr, sizeof(reuseAddr) ) == SOCKET_ERROR )
    {
		DBGLOG( "TCPSERVER: *ERROR* Failed to set server socket to reuse address\n" );
    }

	// switch to nonblocking
	int nonblocking = 1;
	if( m_ListenSocket.IoCtlSocket( FIONBIO, (u_long *)&nonblocking ) == SOCKET_ERROR )
	{
		m_ListenSocket.Close();
        
		DBGLOG( "TCPSERVER: *ERROR* Failed to set server socket to nonblocking\n" );
		return 1;
	}

	// give it an address
	SockAddr serverInAddr( INADDR_ANY, port );

	// bind the address to the socket
	if( m_ListenSocket.Bind( serverInAddr.GetPtr() ) == SOCKET_ERROR )
	{
		m_ListenSocket.Close();

		DBGLOG( "TCPSERVER: *ERROR* Failed to bind address to server socket\n" );
		return 1;
	}

	// quick check
	DBGLOG( "TCPSERVER: Socket(%d), Address(%s), Port(%d)\n", m_ListenSocket.GetSocket(), inet_ntoa(serverInAddr.GetInAddr()), serverInAddr.GetPort() );

	// clear client space
	m_ClientList.clear();

	if( maxNumClients > DEFAULT_MAX_CLIENTS )
	{
		maxNumClients = DEFAULT_MAX_CLIENTS;
		DBGLOG( "CSERVER: *WARNING* too many clients were requested, setting max clients to %d\n", DEFAULT_MAX_CLIENTS );
	}

	m_MaxNumClients = maxNumClients;

	// store the callback locally
	ServerCallback = FuncCallback;

	// bind the address to the socket
	if( m_ListenSocket.Listen() == SOCKET_ERROR )
	{
		m_ListenSocket.Close();

		DBGLOG( "TCPSERVER: *ERROR* Failed to listen on server socket\n" );
		return 1;
	}

	m_ServerStarted = true;
	
	return 0;
}

/////////////////////////////////////////////////////
/// Method: SendTo
/// Params: [in]packetID, [in]buffer, [in]size
///
/////////////////////////////////////////////////////
int TCPServer::SendTo( unsigned short packetID, void *buffer, unsigned int size )
{
	// go through the client list and send the packet to everyone
	int writeSize = 0;

	std::vector<BaseSocket *>::iterator it = m_ClientList.begin();
	while( it != m_ClientList.end() )
	{
		// client connected
		if( (*it)->IsOpen() )
		{

		}

		it++;
	}
	
	return(writeSize);

}

/////////////////////////////////////////////////////
/// Method: SendTo
/// Params: [in]packetID, [in]toID, [in]buffer, [in]size
///
/////////////////////////////////////////////////////
int TCPServer::SendTo( unsigned short packetID, unsigned short toID, void *buffer, unsigned int size )
{
	// go through the client list and send the packet to the passed network id
	std::vector<BaseSocket *>::iterator it = m_ClientList.begin();
	while( it != m_ClientList.end() )
	{
		// client connected
		if( (*it)->IsOpen() )
		{

		}

		it++;
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void TCPServer::Update( float deltaTime )
{
	if( !m_ServerStarted )
		return;

	int bRead=1, bWrite=0, bError=0;
	int nSet=0;

	char buffer[DEFAULT_READWRITESIZE];
	memset( buffer, 0, sizeof(buffer) );

	if( m_ListenSocket.IsOpen() )
	{
		struct sockaddr_in fromAddr;

		if( m_ClientList.size() > 0 )
		{
			std::vector<BaseSocket*>::iterator it = m_ClientList.begin();
			while( it != m_ClientList.end() )
			{
				bool removedClient = false;

				// check for data
				if( (*it)->IsOpen() )
				{
					nSet = (*it)->Select( &bRead, 0, 0 );

					if( nSet > 0 )
					{
						int nReadSize = -1;

						nReadSize = (*it)->Recv( (char *)buffer, DEFAULT_READWRITESIZE );

						if( nReadSize > 0 )
						{
							// 
							DBGLOG( "TCPSERVER: Received %d-bytes (%s)\n", nReadSize, buffer );		
						}
						else
						{
							// client has disconnected
							DBGLOG( "TCPSERVER: Client disconnect\n" );	
							removedClient = true;
						}
					}
				}

				if(removedClient)
				{
					delete (*it);
					it = m_ClientList.erase(it);
				}
				else
					it++;
			}
		}

		// only accept new connections if under the client max count
		if( m_ClientList.size() < m_MaxNumClients )
		{
			SOCKET accepted = m_ListenSocket.Accept( &fromAddr );

			if( accepted != SOCKET_ERROR )
			{
				BaseSocket* newClient = new BaseSocket( accepted );

				DBGLOG( "TCPSERVER: Accepted socket from (%s)\n",  inet_ntoa( fromAddr.sin_addr ) );

				m_ClientList.push_back(newClient);
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CloseServer
/// Params: None
///
/////////////////////////////////////////////////////
int TCPServer::CloseServer()
{
	// find the client and disconnect them
	std::size_t i=0;
	for( i=0; i < m_ClientList.size(); i++ )
	{
		// check for connection
		if( m_ClientList[i]->IsOpen() )
			m_ClientList[i]->Close();

		delete m_ClientList[i];
	}

	m_ClientList.clear();

	// close listening socket
	if( m_ListenSocket.IsOpen())
		m_ListenSocket.Close( );

	m_ServerStarted = false;

	DBGLOG( "TCPSERVER: Server closed\n" );

	return 0;
}

/////////////////////////////////////////////////////
/// Method: DisconnectClient
/// Params: [in]clientID
///
/////////////////////////////////////////////////////
int TCPServer::DisconnectClient( int clientID )
{
	unsigned short disconnectionID = 0;

	// find the client and disconnect them
	std::vector<BaseSocket*>::iterator it = m_ClientList.begin();
	while( it != m_ClientList.end() )
	{
		if( (*it)->IsOpen() )
			(*it)->Close();

		it++;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: GetClient
/// Params: [in]clientID
///
/////////////////////////////////////////////////////
network::TCPClient *TCPServer::GetClient( unsigned short clientID )
{
	return 0;
}

/////////////////////////////////////////////////////
/// Method: GetClientPing
/// Params: [in]clientID
///
/////////////////////////////////////////////////////
double TCPServer::GetClientPing( unsigned short clientID )
{
	return(0.0);
}

#endif // BASE_SUPPORT_NETWORKING
