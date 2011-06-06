/*********************************************************\
File:           Socket.cpp
Author:         Kyle Weicht
Created:        6/3/2011
Modified:       6/3/2011 9:55:33 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "socket.h"


#ifdef OS_WINDOWS

#include <WinSock2.h>

#else

    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>

#endif // #ifndef OS_WINDOWS


namespace Riot
{    
    /***************************************\
    | class members                         |
    \***************************************/

    
    /***************************************\
    | class methods                         |
    \***************************************/
    // CSocket constructor
    CSocket::CSocket()
        : m_nHandle( 0 )
    {
    }

    // CSocket destructor
    CSocket::~CSocket()
    {
    }

    void CSocket::OpenSocket( uint16 nPort )
    {
        //////////////////////////////////////////
        m_nHandle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

        ASSERT( m_nHandle > 0 );

        sockaddr_in address;

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( nPort );

        sint32 nResult = bind( m_nHandle, (const sockaddr*)&address, sizeof( sockaddr_in ) );

        ASSERT( nResult > 0 );

        //////////////////////////////////////////
#ifndef OS_WINDOWS

        int nonBlocking = 1;
        nResult = fcntl( m_nHandle, F_SETFL, O_NONBLOCK, nonBlocking );

        ASSERT( nResult != -1 );
#else

        DWORD nonBlocking = 1;

        nResult = ioctlsocket( m_nHandle, FIONBIO, &nonBlocking );

        ASSERT( nResult == 0 );
#endif

    }

    void CSocket::SendData( void* pData, uint nDataSize, Address nAddress, uint16 nPort )
    {
        sockaddr_in;
    }


}// namespace Riot
