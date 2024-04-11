
#ifndef __SOCKADDR_H__
#define __SOCKADDR_H__

#ifdef BASE_SUPPORT_NETWORKING

namespace network
{
	class SockAddr : private sockaddr_in
	{
		public:
			/// default constructor 
			SockAddr();
			/// constructor
			/// \param sa - initialising socket address
			explicit SockAddr( const sockaddr_in& sa );
			/// constructor
			/// \param inAddr - ulong initialising address
			/// \param wPort - initialising port
			SockAddr( unsigned long inAddr, unsigned short wPort );
			/// constructor
			/// \param inAddr - in_addr initialiser
			/// \param wPort - initialising port
			SockAddr( const in_addr& inAddr, unsigned short wPort );

			/// GetInAddr - gets the internet address attached to this class
			/// \return in_addr - internet address
			in_addr				GetInAddr() const;
			/// GetPtr - gets the protocol address attachment
			/// \return sockaddr_in * - IP4 socket structure
			const sockaddr_in*	GetPtr() const;
			/// GetAddr - gets the dotted IP address
			/// \return unsigned long - IP4 dotted address
			unsigned long		GetAddr() const;
			/// GetPort - gets the port
			/// \return unsigned short - port number
			unsigned short		GetPort() const;
			/// GetStr - gets the dotted IP address as a string
			/// \param strAddr - string to recieve the dotted IP address
			/// \param includePort - attach port number onto the end of the string
			void				GetStr( unsigned short *strAddr, bool includePort=true ) const;
			/// Clear - clears the structure of any address data
			void				Clear();

		private:
	};

} // namespace

#endif // BASE_SUPPORT_NETWORKING

#endif // __SOCKADDR_H__


