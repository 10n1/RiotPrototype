/*********************************************************\
File:           Camera.h
Purpose:        An interface for the main camera
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/4/2011 12:52:54 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "common.h"
#include "IListener.h"

#define AVERAGE_WALK_SPEED 1.4f
#define AVERAGE_RUN_SPEED  6.7f

namespace Riot
{
    class CView;

    class CCamera : public IListener
    {
        friend class Engine;
    public:
        // CCamera constructor
        CCamera();

        // CCamera destructor
        ~CCamera();
        /***************************************\
        | class methods                         |
        \***************************************/
        
        //-----------------------------------------------------------------------------
        //  Update
        //  Updates the camera
        //-----------------------------------------------------------------------------
        void Update( void );
        
        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );

        //-----------------------------------------------------------------------------
        //  Accessors/mutators
        //-----------------------------------------------------------------------------
        CView*  GetView( void );
        void    SetView( CView* pView );

    private:
        /***************************************\
        | class members                         |
        \***************************************/        
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        CView*  m_pView;
        float   m_fMovementSpeed;
        bool    m_bMouseControlled;
    };

} // namespace Riot

#endif // #ifndef _CAMERA_H_
