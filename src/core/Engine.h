/*********************************************************\
File:           Engine.h
Purpose:        The main engine
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/17/2011 5:35:00 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ENGINE_H_
#define _ENGINE_H_
#include "common.h"
#include "IListener.h"
#include "timer.h"

namespace Riot
{
    class CTaskManager;
    class CMessageDispatcher;
    class CWindow;
    class CInputManager;
    class CRenderer;
    class CView;
    class CComponentManager;
    class CObjectManager;
    class CTerrain;

    class Engine : public IListener
    {
    private:
        // Engine constructor
        Engine() { }
        Engine( const Engine& ref ) { }
        Engine& operator=( const Engine& ref ) { return *this; }

        // Engine destructor
        ~Engine() { }
    public:
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  GetInstance
        //  Returns the global instance
        //-----------------------------------------------------------------------------
        static Engine* GetInstance( void );

        //-----------------------------------------------------------------------------
        //  Run
        //  Starts the engine/game. All variables are set programatically
        //-----------------------------------------------------------------------------
        void Run( void );

        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );
        
        //-----------------------------------------------------------------------------
        //  PostMsg
        //  Posts a message to be processed later
        //-----------------------------------------------------------------------------
        static void PostMsg( const TMessage& msg );
        static void PostMsg( MessageType nType );
        
        //-----------------------------------------------------------------------------
        //  SendMsg
        //  Sends a message immediately
        //-----------------------------------------------------------------------------
        static void SendMsg( const TMessage& msg );
        static void SendMsg( MessageType nType );
        
        //-----------------------------------------------------------------------------
        //  Accessors/Mutators
        //-----------------------------------------------------------------------------
        static CTaskManager*        GetTaskManager( void );
        static CRenderer*           GetRenderer( void );
        static CComponentManager*   GetComponentManager( void );

    private:    
        //-----------------------------------------------------------------------------
        //  Initialize
        //  Initializes the engine. This is called from Run
        //-----------------------------------------------------------------------------
        void Initialize( void );

        //-----------------------------------------------------------------------------
        //  Shutdown
        //  Shuts down and cleans up the engine
        //-----------------------------------------------------------------------------
        void Shutdown( void );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        static CTimer               m_MainTimer;
        static Engine*              m_pInstance;
        static CTaskManager*        m_pTaskManager;
        static CMessageDispatcher*  m_pMessageDispatcher;
        static CInputManager*       m_pInputManager;
        static CComponentManager*   m_pComponentManager;
        static CObjectManager*      m_pObjectManager;

        static CWindow*     m_pMainWindow;
        static CRenderer*   m_pRenderer;

        static CView*       m_pMainView;

        static CTerrain*    m_pTerrain;

        static uint     m_nFrame;
        static bool     m_bRunning;

    public:
        static float    m_fElapsedTime;
    };


} // namespace Riot

#endif // #ifndef _ENGINE_H_
