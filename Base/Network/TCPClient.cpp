
/*===================================================================
	File: TCPClient.cpp
	Library: NetworkLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_NETWORKING

#include "CoreBase.h"

#include "Network/PlatformNetworkBase.h"
#include "Network/SockAddr.h"
#include "Network/BaseSocket.h"
#include "Network/NetworkFunctions.h"
#include "Network/TCPClient.h"

using network::TCPClient;

const float DEFAULT_CONNECTION_TIME = 10.0f;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
TCPClient::TCPClient()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
TCPClient::~TCPClient()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void TCPClient::Initialise()
{
	m_ClientConnected = false;
	m_IsConnected = false;
	m_IsConnecting = false;

	ClientCallback = 0;
	m_NetworkID = 0;

	nIncomingSequence = 0;
	nOutgoingSequence = 1;
	pingTimeMillisecs = 0.0;

	m_ConnectionTimer = 0.0f;

	m_ClientAddress.Clear();
	m_ServerAddress.Clear();
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void TCPClient::Release()
{
	if( m_ConnectedSocket.IsOpen() )
		m_ConnectedSocket.Close();

	m_ClientConnected = false;

}

/////////////////////////////////////////////////////
/// Method: ConnectToServer
/// Params: [in]port, [in]szServerIP, [in]FuncCallback
///
/////////////////////////////////////////////////////
int TCPClient::ConnectToServer( int port, const char *szServerIP, void (*FuncCallback)( void *pData, int size, int packetID ) )
{
	int errorVal = 0;

	// make sure not connecting, or already connected
	if( m_IsConnecting || 
		m_IsConnected )
	{
		DBGLOG( "TCPCLIENT: *WARNING* already trying to connect or connected\n" );
		return 1;
	}

	// initialise the client socket
	m_ConnectedSocket.Close();
	m_ConnectedSocket.Open( network::BaseSocket::SocketType_TCP );

	if( !m_ConnectedSocket.IsOpen() )
	{
		DBGLOG( "TCPCLIENT: *ERROR* Client socket not created\n" );
		return 1;
	}

	// reset sequence numbers
	nIncomingSequence = 0;
	nOutgoingSequence = 1;

	pingTimeMillisecs = 0.0;

	u_long inetAddr = inet_addr(szServerIP);
	//u_long inetAddr = network::TextIPAddressToULong( szServerIP );

	// create client address
	m_ServerAddress = SockAddr( inetAddr, port );

	// store the callback locally
	ClientCallback = FuncCallback;

    // want the socket to be non-blocking
	/*if( m_ConnectedSocket.Fcntl( F_SETFL, O_NONBLOCK ) == SOCKET_ERROR )
	{
		m_ConnectedSocket.Close();
        
		DBGLOG( "TCPCLIENT: *ERROR* Could not set client socket to nonblocking\n" );
		return 1;
	}*/
    
	// want the socket to be non-blocking
	int nonblocking = 1;
	if( m_ConnectedSocket.IoCtlSocket( FIONBIO, (u_long *)&nonblocking ) == SOCKET_ERROR )
	{
		m_ConnectedSocket.Close();
        
		DBGLOG( "TCPCLIENT: *ERROR* Could not set client socket to nonblocking\n" );
		return 1;
	}

	errno = 0;
	m_ConnectionTimer = 0.0f;

	if( m_ConnectedSocket.Connect( m_ServerAddress.GetPtr() ) == SOCKET_ERROR )
	{
#ifdef BASE_PLATFORM_WINDOWS
		int err = WSAGetLastError();
		if( err != WSAEWOULDBLOCK ) // WSAEWOULDBLOCK is normal if time is needed on a non-blocking connect socket
		{
            m_ConnectedSocket.Close();
            DBGLOG( "TCPCLIENT: *ERROR* Could not connect to server (WSAGetLastError %d)\n", err );
            return 1;			
		}
#endif // BASE_PLATFORM_WINDOWS

		int errNum = errno;
        if( errNum != 0 &&
			errNum != EINPROGRESS )
        {
            m_ConnectedSocket.Close();
            DBGLOG( "TCPCLIENT: *ERROR* Could not connect to server (errno %d)\n", errNum );
            return 1;
        }
	}

	m_IsConnecting = true;
	DBGLOG( "TCPCLIENT: Client is connecting\n" );

	return 0;
}

/////////////////////////////////////////////////////
/// Method: Send
/// Params: [in]packetID, [in]buffer, [in]size
///
/////////////////////////////////////////////////////
int TCPClient::Send( unsigned short packetID, void *buffer, unsigned int size )
{
	if( m_ConnectedSocket.IsOpen() )
	{
		return m_ConnectedSocket.Send( buffer, size );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void TCPClient::Update( float deltaTime )
{
	int bRead=1, bWrite=0, bError=0;
	int nSet=0;

	char buffer[DEFAULT_READWRITESIZE];
	memset( buffer, 0, sizeof(buffer) );
    
    if( !m_IsConnecting &&
       !m_IsConnected )
        return;
    
    if( m_IsConnecting &&
       m_IsConnected == false)
    {
        int result = 0;
        int resultLen = static_cast<int>(sizeof(result));
        if (m_ConnectedSocket.GetSockOpt( SOL_SOCKET, SO_ERROR, &result, &resultLen) < 0 )
        {
            m_IsConnecting = false;
            m_ConnectedSocket.Close();
        }
        
        if(result == 0)
        {
			nSet = m_ConnectedSocket.Select( 0, &bWrite, 0 );

            if( nSet < 0 )
            {
				DBGLOG( "TCPCLIENT: Not connected\n" );

				m_IsConnecting = false;
				m_ConnectedSocket.Close();
			}
			else
			{
				if( nSet > 0 )
				{
					// socket is ready for read()/write()
					m_IsConnecting = false;
					m_IsConnected = true;

					DBGLOG( "TCPCLIENT: Is connected\n" );
				}
				else
				{
					// keep waiting
					m_ConnectionTimer += deltaTime;
					if( m_ConnectionTimer >= DEFAULT_CONNECTION_TIME )
					{
						DBGLOG( "TCPCLIENT: Timeout\n" );

						m_IsConnecting = false;
						m_ConnectedSocket.Close();
					}
				}
			}
        }
        else
        {
			DBGLOG( "TCPCLIENT: Not connected\n" );

            m_IsConnecting = false;
            m_ConnectedSocket.Close();
        }
    }
    else
    {
        // socket open?
        if( m_IsConnected &&
           m_ConnectedSocket.IsOpen() )
        {
            nSet = m_ConnectedSocket.Select( &bRead, 0, 0 );

            if( nSet > 0 )
            {
                int nReadSize = -1;

                nReadSize = m_ConnectedSocket.Recv( (char *)buffer, DEFAULT_READWRITESIZE );

                if( nReadSize > 0 )
                {
                    // 
                    DBGLOG( "TCPCLIENT: Received %d-bytes (%s)\n", nReadSize, buffer );
                }
				else
				{
					// client has disconnected
					DBGLOG( "TCPCLIENT: Server disconnect\n" );	
					DisconnectFromServer();
				}
            }
        }
    }
}

/////////////////////////////////////////////////////
/// Method: DisconnectFromServer
/// Params: None
///
/////////////////////////////////////////////////////
int TCPClient::DisconnectFromServer()
{
	if( m_ConnectedSocket.IsOpen() )
		m_ConnectedSocket.Close();

	m_IsConnected = false;
	m_IsConnecting = false;

	m_ClientConnected = false;
	m_ClientAddress.Clear();
	m_ServerAddress.Clear();
	ClientCallback = 0;

	DBGLOG( "TCPCLIENT: Disconnected\n" );

	return 0;
}

#endif // BASE_SUPPORT_NETWORKING
