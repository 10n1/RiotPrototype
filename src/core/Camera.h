/*********************************************************\
File:           Camera.h
Purpose:        An interface for the main camera
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/5/2011 9:30:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "common.h"
#include "IListener.h"
#include "View.h"

#define AVERAGE_WALK_SPEED 1.4f * 10.0f
#define AVERAGE_RUN_SPEED  6.7f * 10.0f

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
        inline const RVector3& GetPosition( void );

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

    //-----------------------------------------------------------------------------
    //  Accessors/mutators
    //-----------------------------------------------------------------------------
    const RVector3& CCamera::GetPosition( void )
    {
        return m_pView->GetPosition();
    }

} // namespace Riot

#endif // #ifndef _CAMERA_H_
