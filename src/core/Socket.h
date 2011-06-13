/*********************************************************\
File:           Socket.h
Purpose:        
Author:         Kyle Weicht
Created:        6/3/2011
Modified:       6/3/2011 9:44:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SOCKET_H_
#define _SOCKET_H_
#include "common.h"

namespace Riot
{

typedef uint32 Address;

inline uint32 BuildAddress( uint8 a, uint8 b, uint8 c, uint8 d )
{
    uint32 nAddress = 0;
    nAddress = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d ;

    return nAddress;
}

class CSocket
{
public:
    // CSocket constructor
    CSocket();

    // CSocket destructor
    ~CSocket();
    /***************************************\
    | class methods                         |
    \***************************************/

    void OpenSocket( uint16 nPort );
    void CloseSocket( void );
    void SendData( void* pData, uint nDataSize, Address nAddress, uint16 nPort );
    void ReceiveData( void* pData, uint nDataSize );
    
private:
    /***************************************\
    | class members                         |
    \***************************************/
    sint32  m_nHandle;
};

} // namespace Riot

#endif // #ifndef _SOCKET_H_
