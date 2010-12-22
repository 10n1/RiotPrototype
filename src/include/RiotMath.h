/*********************************************************\
File:       RiotMath.h
Purpose:    Vector and matrices implementations
\*********************************************************/
#ifndef _RIOTMATH_H_
#define _RIOTMATH_H_

#include <math.h>
#include "RiotTypes.h"

static const float gs_fPi = 3.14159265358979f;
static const float gs_fPiRecip = (1.0f/gs_fPi);
static const float gs_fDegToRad = (gs_fPi/180.0f);
static const float gs_fRadToDeg = (180.0f/gs_fPi);
static const float gs_fEpsilon = (0.0001f);
_inline float DegToRad( float fDeg ) { return fDeg * gs_fDegToRad; }
_inline float RadToDeg( float fRad ) { return fRad * gs_fRadToDeg; }

class RiotVector2
{
public:
    /***************************************\
    | class members
    \***************************************/
    union
    {
        struct { float x, y; };
        float f[2];
    };

    // RiotVector2 constructors
    RiotVector2(  ) { }
    RiotVector2( float X, float Y );
    RiotVector2( const RiotVector2& V );
    RiotVector2( const float* F );
    RiotVector2& operator=( const RiotVector2& V );

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RiotVector2 operator+( const RiotVector2& V ) const;
    RiotVector2 operator+( float F ) const;
    RiotVector2& operator+=( const RiotVector2& V );
    RiotVector2& operator+=( float F );

    // Subtract
    RiotVector2 operator-( const RiotVector2& V ) const;
    RiotVector2 operator-( float F ) const;
    RiotVector2& operator-=( const RiotVector2& V );
    RiotVector2& operator-=( float F );

    // Multiply and divide
    RiotVector2 operator*( float F ) const;
    RiotVector2& operator*=( float F );
    RiotVector2 operator/( float F ) const;
    RiotVector2& operator/=( float F );

    // Comparison
    bool operator==( const RiotVector2& V ) const;
    bool operator!=( const RiotVector2& V ) const;

    // Vector operations

    // Vector-Vector operations
    float DotProduct( const RiotVector2& V ) const;

    // Misc operations
    float Magnitude( void ) const;
    float MagnitudeSquared( void ) const;
    float Distance( const RiotVector2& V ) const;
    float DistanceSquared( const RiotVector2& V ) const;
    void Normalize( void );
    void Zero( void );
};

float DotProduct( const RiotVector2& V1, const RiotVector2& V2 );
float Distance( const RiotVector2& V1, const RiotVector2& V2 );
float DistanceSquared( const RiotVector2& V1, const RiotVector2& V2 );
RiotVector2 Normalize( const RiotVector2& V );

class RiotVector3
{
public:
    /***************************************\
    | class members
    \***************************************/
    union
    {
        struct { float x, y, z; };
        float f[3];
    };

public:
    // RiotVector3 constructors
    RiotVector3(  ) { }
    RiotVector3( float X, float Y, float Z );
    RiotVector3( const RiotVector3& V );
    RiotVector3( const float* F );
    RiotVector3& operator=( const RiotVector3& V );

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RiotVector3 operator+( const RiotVector3& V ) const;
    RiotVector3 operator+( float F ) const;
    RiotVector3& operator+=( const RiotVector3& V );
    RiotVector3& operator+=( float F );

    // Subtract
    RiotVector3 operator-( const RiotVector3& V ) const;
    RiotVector3 operator-( float F ) const;
    RiotVector3& operator-=( const RiotVector3& V );
    RiotVector3& operator-=( float F );

    // Multiply and divide
    RiotVector3 operator*( float F ) const;
    RiotVector3& operator*=( float F );
    RiotVector3 operator/( float F ) const;
    RiotVector3& operator/=( float F );

    // Comparison
    bool operator==( const RiotVector3& V ) const;
    bool operator!=( const RiotVector3& V ) const;

    // Vector operations

    // Vector-Vector operations
    float DotProduct( const RiotVector3& V ) const;
    RiotVector3 CrossProduct( const RiotVector3& V ) const;

    // Misc operations
    float Magnitude( void ) const;
    float MagnitudeSquared( void ) const;
    float Distance( const RiotVector3& V ) const;
    float DistanceSquared( const RiotVector3& V ) const;
    void Normalize( void );
    void Zero( void );
};

float DotProduct( const RiotVector3& V1, const RiotVector3& V2 );
RiotVector3 CrossProduct( const RiotVector3& V1, const RiotVector3& V2 );
float Distance( const RiotVector3& V1, const RiotVector3& V2 );
float DistanceSquared( const RiotVector3& V1, const RiotVector3& V2 );
RiotVector3 Normalize( const RiotVector3& V );
RiotVector3 RiotVector3Zero( void );

class RiotVector4
{
public:
    /***************************************\
    | class members
    \***************************************/
    union
    {
        struct { float x, y, z, w; };
        float f[4];
    };

public:
    // RiotVector4 constructors
    RiotVector4(  ) { }
    RiotVector4( float X, float Y, float Z, float W );
    RiotVector4( float X, float Y, float Z );
    RiotVector4( const RiotVector4& V );
    RiotVector4( const float* F );
    RiotVector4& operator=( const RiotVector4& V );

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RiotVector4 operator+( const RiotVector4& V ) const;
    RiotVector4 operator+( float F ) const;
    RiotVector4& operator+=( const RiotVector4& V );
    RiotVector4& operator+=( float F );

    // Subtract
    RiotVector4 operator-( const RiotVector4& V ) const;
    RiotVector4 operator-( float F ) const;
    RiotVector4& operator-=( const RiotVector4& V );
    RiotVector4& operator-=( float F );

    // Multiply and divide
    RiotVector4 operator*( float F ) const;
    RiotVector4& operator*=( float F );
    RiotVector4 operator/( float F ) const;
    RiotVector4& operator/=( float F );

    // Comparison
    bool operator==( const RiotVector4& V ) const;
    bool operator!=( const RiotVector4& V ) const;

    // Vector operations

    // Vector-Vector operations
    float DotProduct( const RiotVector4& V ) const;
    RiotVector4 CrossProduct( const RiotVector4& V ) const;

    // Misc operations
    float Magnitude( void ) const;
    float MagnitudeSquared( void ) const;
    float Distance( const RiotVector4& V ) const;
    float DistanceSquared( const RiotVector4& V ) const;
    void Normalize( void );
    void Zero( void );
};

float DotProduct( const RiotVector4& V1, const RiotVector4& V2 );
RiotVector4 CrossProduct( const RiotVector4& V1, const RiotVector4& V2 );
float Distance( const RiotVector4& V1, const RiotVector4& V2 );
float DistanceSquared( const RiotVector4& V1, const RiotVector4& V2 );
RiotVector4 Normalize( const RiotVector4& V );
RiotVector4 RiotVector4Zero(  );

//template< class T >
//class RiotMatrix3x3
//{
//public:
//    RiotRiotVector3<T> r1;
//    RiotRiotVector3<T> r2;
//    RiotRiotVector3<T> r3;
//};
//
//template< class T >
//class RiotMatrix4x4
//{
//public:
//    RiotRiotVector4<T> r1;
//    RiotRiotVector4<T> r2;
//    RiotRiotVector4<T> r3;
//    RiotRiotVector4<T> r4;
//};

#endif _RIOTMATH_H_
