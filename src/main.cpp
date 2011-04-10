/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/9/2011 10:53:09 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include <stdlib.h> // For atexit on UNIX systems
#include <stdio.h>

#include "System.h"
#include "TaskManager.h"
#include "timer.h"

using namespace Riot;

CTaskManager    gTaskManager;

#define NUM_TASKS 1024
#define NUM_LOOPS 32
#define TOTAL_WORK (NUM_TASKS*NUM_LOOPS)

uint            nTest[ NUM_TASKS ];
volatile sint   nTaskId = 0;

RMatrix4        pMatricesA[ NUM_TASKS ];
RMatrix4        pMatricesB[ NUM_TASKS ];
RMatrix4        pMatricesC[ NUM_TASKS ];

class Test : public CInternalTask
{
public:
    Test( CTaskCompletion* pCompletion )
        : CInternalTask( pCompletion )
        , m_nTaskId( nTaskId++ )
    {
    }
	bool Run(CThread* pThread)
    {
        nTest[ nTaskId ] = pThread->GetThreadId();
        
        for( uint i = 0; i < TOTAL_WORK; ++i )
        {
            pMatricesA[nTaskId].r1.x = i;
            pMatricesC[ nTaskId ] = pMatricesA[nTaskId] * pMatricesB[nTaskId];
        }
        return true;
    }

    static CMutex m_Mutex;
    uint    m_nTaskId;
};

void PrintArray( void )
{
    uint n = 0;
    for( uint i = 0; i < NUM_TASKS; i += 12 )
    {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
            nTest[n++], nTest[n++],
            nTest[n++], nTest[n++],
            nTest[n++], nTest[n++],
            nTest[n++], nTest[n++],
            nTest[n++], nTest[n++],
            nTest[n++], nTest[n++] );

        //if( !( i % 8 ) )
        //{
        //    printf( "\n" );
        //}
    }
}

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( ReleasePool );

    //////////////////////////////////////////
    // Initialize sub systems
    System::Initialize();
    gTaskManager.Initialize();

    CTimer timer;
    timer.Reset();

    CTaskCompletion completion;
    for( uint i = 0; i < NUM_TASKS; ++i )
    {
        gTaskManager.PushTask( new Test( &completion ) );
    }

    gTaskManager.WaitForCompletion( &completion );
    float fTime = timer.GetElapsedTime();

    PrintArray();

    printf("\n%f seconds\n", fTime);

    //////////////////////////////////////////
    //  Clean up subsystems
    gTaskManager.Shutdown();
    System::Shutdown();

    return 0;
}
