/*********************************************************\
File:           IListener.h
Purpose:        
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 12:45:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ILISTENER_H_
#define _ILISTENER_H_

namespace Riot
{
    
struct TMessage;

class IListener
{
public:
    // IListener constructor
    IListener();

    // IListener destructor
    virtual ~IListener() { }
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    virtual void ProcessMessage( const TMessage& msg ) { }

private:
    /***************************************\
    | class members                         |
    \***************************************/

};

} // namespace Riot

#endif // #ifndef _ILISTENER_H_
