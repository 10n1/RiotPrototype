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

private:
    /***************************************\
    | class members                         |
    \***************************************/
    sint32  m_nHandle;
};

}// namespace Riot

#endif // #ifndef _SOCKET_H_
