/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/8/2011 8:38:15 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include "timer.h"
#include <stdlib.h> // For atexit on UNIX systems

#include <stdio.h>

#include <Windows.h>
#include <xnamath.h>

void FillRand( float* f, uint nCount )
{
    for( uint i = 0; i < nCount; ++i )
    {
        f[i] = rand()/(float)(RAND_MAX);
        f[i] *= 10.0f;
    }
}

uint Compare( float* a, float* b, uint nCount )
{
    for( uint i = 0; i < nCount; ++i )
    {
        if( abs( a[i] - b[i] ) > gs_EPSILON )
        {
            return 0;
        }
    }
    
    return 1;
}

void TestMath( void );

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( ReleasePool );
    srand( GetTickCount() );
    
    CTimer t;
    t.Reset();
    double fStartTime = 0.0f;
    double fEndTime = 0.0f;


    byte  p[1024*256] = { 0 };
    
    fStartTime = t.GetElapsedTime();
    
    RQuaternion q0, q1;
    FillRand( q0, 4 );
    FillRand( q1, 4 );
    Normalize( q0 );
    Normalize( q1 );
    RMatrix4 mqm;

    for( uint i = 0; i < 32 * 1024 * 1024; ++i )
    {
        mqm = RMatrix4( RQuatGetMatrix( q0 ) );
        q0.x = mqm.m11;
        q0 = Normalize( q0 );
    }
    
    fEndTime = t.GetElapsedTime();
    
    if( mqm.m11 != 'a' )
    {
        printf( "%f seconds, %f start\n", fEndTime, fStartTime );
    }

    return 0;
}

#define PRINT_RESULT( x ) printf( "%d\t\t%s\n", x, #x )

void TestMath( void )
{
    RMatrix4 ma, mb, mc;
    XMMATRIX xa, xb, xc;

    FillRand( ma, 16 );
    FillRand( mb, 16 );

    Memcpy( &xa, &ma, sizeof( XMMATRIX ) );
    Memcpy( &xb, &mb, sizeof( XMMATRIX ) );


    PRINT_RESULT( Compare( ma, &xa.m[0][0], 16 ) );

    //mc = ma * mb;
    mc = ma * mb;
    xc = XMMatrixMultiply( xa, xb );
    
    PRINT_RESULT( Compare( mc, &xc.m[0][0], 16 ) );

    RVector4 mv1, mv2;
    XMVECTOR xv1, xv2;

    FillRand( mv1, 4 );
    Memcpy( &xv1, mv1, sizeof( RVector4 ) );

    mv2 = mv1 * mc;
    xv2 = XMVector4Transform( xv1, xc );

    XMVECTOR t;
    t.m128_f32[0] = xv2.m128_f32[3];
    t.m128_f32[1] = xv2.m128_f32[2];
    t.m128_f32[2] = xv2.m128_f32[1];
    t.m128_f32[3] = xv2.m128_f32[0];
    PRINT_RESULT( Compare( mv2, xv2.m128_f32, 4 ) );

    RVector3 vec;
    FillRand( vec, 3 );
    float f = rand()/(float)(RAND_MAX);
    f *= 360.0f;
    
    XMVECTOR xq = XMQuaternionRotationAxis( XMLoadFloat3( (XMFLOAT3*)&vec ), f );
    RQuaternion mq = RQuatFromAxisAngle( vec, f );

    PRINT_RESULT( Compare( xq.m128_f32, mq, 4 ) );
    
    XMMATRIX xqm = XMMatrixRotationQuaternion( xq );
    RMatrix4 mqm( RQuatGetMatrix( mq ) );

    PRINT_RESULT( Compare( (float*)&xqm, mqm, 4 ) );
}
