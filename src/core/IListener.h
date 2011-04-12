/*********************************************************\
File:           IListener.h
Purpose:        
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/11/2011 9:52:32 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ILISTENER_H_
#define _ILISTENER_H_
#include "common.h"
#include "Message.h"

namespace Riot
{

    class IListener
    {
    public:
        // IListener constructor
        IListener() { }

        // IListener destructor
        virtual ~IListener() { }
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        inline virtual void ProcessMessage( const TMessage& msg ) { }

    private:
        /***************************************\
        | class members                         |
        \***************************************/

    };

} // namespace Riot

#endif // #ifndef _ILISTENER_H_
