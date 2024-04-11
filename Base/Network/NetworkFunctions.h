
#ifndef __NETWORKFUNCTIONS_H__
#define __NETWORKFUNCTIONS_H__

#ifdef BASE_SUPPORT_NETWORKING

namespace network
{
	int Initialise();

	void Shutdown();

	/// TextIPAddressToULong - Converts a text based web address into a in_addr safe unsigned long
	/// \param szAddress - address to lookup
	/// \return unsigned long - in_addr of the address
	unsigned long TextIPAddressToULong( const char *szAddress );
	/// GetLocalIPAddress - Fills an array of all the machines network adaptor IPs
	void GetLocalIPAddress();
	/// ResolveNameToIP - tries to resolve the IP from a name
	/// \param hostName - host name to look up
	/// \return const char* - dotted IP address or 0
	const char* ResolveNameToIP( const char* hostName );
	/// ResolveNameToIP - tries to resolve the IP from a name using specific socket detail hints
	/// \param hostName - host name to look up
	/// \param family - what socket family
	/// \param socketType - what socket type
	/// \return const char* - dotted IP address or 0
	const char* ResolveNameToIP( const char* hostName, int family, int socketType );
	/// GetMachineName - gets the current host name
	/// \param machineName - buffer to copy name into
	void GetMachineName(char* machineName);
}

#endif // BASE_SUPPORT_NETWORKING

#endif // __NETWORKFUNCTIONS_H__
