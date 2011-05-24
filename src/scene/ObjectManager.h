/*********************************************************\
File:           ObjectManager.h
Purpose:        Handles allocation of objects
Author:         Kyle Weicht
Created:        3/31/2011
Modified:       5/20/2011 4:16:57 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_
#include "common.h"
#include "IListener.h"
#include "Thread.h"
#include "Object.h"

namespace Riot
{    
    class CObjectManager : public IListener
    {
        friend class Engine;
    public:
        // CObjectManager constructor
        CObjectManager();

        // CObjectManager destructor
        ~CObjectManager();
        /***************************************\
        | class methods                         |
        \***************************************/
        
        //-----------------------------------------------------------------------------
        //  Initialize
        //-----------------------------------------------------------------------------
        void Initialize( void );

        //-----------------------------------------------------------------------------
        //  Shutdown
        //-----------------------------------------------------------------------------
        void Shutdown( void );

        //-----------------------------------------------------------------------------
        //  CreateObject
        //  Creates a new object and returns its index
        //-----------------------------------------------------------------------------
        uint CreateObject( const char* szName, const char* szType );
        uint CreateObject( uint32 nNameHash, uint32 nTypeHash );
        uint CreateObjectFromFile( const char* szFilename );
        
        //-----------------------------------------------------------------------------
        //  DeleteObject
        //  Deletes an object
        //-----------------------------------------------------------------------------
        void DeleteObject( uint nIndex );

        //-----------------------------------------------------------------------------
        //  LoadObjectDeclaration
        //  Loads an object declaration from a file
        //-----------------------------------------------------------------------------
        void LoadObjectDeclaration( const char* szFilename );
        
        //-----------------------------------------------------------------------------
        //  RegisterFunc
        //  Registers an object function
        //-----------------------------------------------------------------------------
        void RegisterFunc( const char* szFunc, ObjectFunc* pFunc );
        
        //-----------------------------------------------------------------------------
        //  GetObject
        //  Returns an object
        //-----------------------------------------------------------------------------
        inline CObject& GetObject( uint nIndex );
        
        //-----------------------------------------------------------------------------
        //  GetFunction
        //  Returns the function
        //-----------------------------------------------------------------------------
        ObjectFunc* GetFunction( const char* szFunc );
        ObjectFunc* GetFunction( uint32 nHash );
           
        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );
                
        //-----------------------------------------------------------------------------
        //  UpdateObjects
        //  Updates the objects
        //-----------------------------------------------------------------------------
        void UpdateObjects( float fDt );

    private:
        //-----------------------------------------------------------------------------
        //  Parallel functions
        //  The task functions
        //-----------------------------------------------------------------------------
        static void ParallelProcessComponents( void* pData, uint nThreadId, uint nStart, uint nCount );
        static void ParallelProcessComponentMessages( void* pData, uint nThreadId, uint nStart, uint nCount );
        static void PipelineObjectUpdate( void* pData, uint nThreadId, uint nStart, uint nCount );
    
    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        CObject             m_Objects[MAX_OBJECTS];
        uint32              m_nFreeSlots[ MAX_OBJECTS ];
        uint32              m_nActiveObjects[ MAX_OBJECTS ];

        TObjectDefinition   m_pObjectTypes[ 128 ];
        uint32              m_nNumObjectTypes;
        uint32              m_pFuncNameHashs[ 128 ];
        ObjectFunc*         m_pFuncs[ 128 ];
        uint32              m_nNumFuncs;

        atomic_t            m_nNumObjects;
        sint32              m_nNumFreeSlots;

    };

    //-----------------------------------------------------------------------------
    //  GetObject
    //  Returns an object
    //-----------------------------------------------------------------------------
    CObject& CObjectManager::GetObject( uint nIndex )
    {
        return m_Objects[ nIndex ];
    }

} // namespace Riot

#endif // #ifndef _OBJECTMANAGER_H_
