/*********************************************************\
File:       Math.h
Purpose:    Vector and matrix implementations
\*********************************************************/
#ifndef _RIOTMATH_H_
#define _RIOTMATH_H_

#include <math.h>
#include "Types.h"

static const float gs_fPi = 3.14159265358979f;
static const float gs_fPiRecip = (1.0f/gs_fPi);
static const float gs_fDegToRad = (gs_fPi/180.0f);
static const float gs_fRadToDeg = (180.0f/gs_fPi);
static const float gs_fEpsilon = (0.0001f);
_inline float DegToRad( float fDeg ) { return fDeg * gs_fDegToRad; }
_inline float RadToDeg( float fRad ) { return fRad * gs_fRadToDeg; }

class RVector2
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

    // RVector2 constructors
    RVector2(  ) { }
    RVector2( float X, float Y );
    RVector2( const RVector2& V );
    RVector2( const float* F );
    RVector2& operator=( const RVector2& V );

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RVector2 operator+( const RVector2& V ) const;
    RVector2 operator+( float F ) const;
    RVector2& operator+=( const RVector2& V );
    RVector2& operator+=( float F );

    // Subtract
    RVector2 operator-( const RVector2& V ) const;
    RVector2 operator-( float F ) const;
    RVector2& operator-=( const RVector2& V );
    RVector2& operator-=( float F );

    // Multiply and divide
    RVector2 operator*( float F ) const;
    RVector2& operator*=( float F );
    RVector2 operator/( float F ) const;
    RVector2& operator/=( float F );

    // Comparison
    bool operator==( const RVector2& V ) const;
    bool operator!=( const RVector2& V ) const;

    // Vector operations

    // Vector-Vector operations
    float DotProduct( const RVector2& V ) const;

    // Misc operations
    float Magnitude( void ) const;
    float MagnitudeSquared( void ) const;
    float Distance( const RVector2& V ) const;
    float DistanceSquared( const RVector2& V ) const;
    void Normalize( void );
    void Zero( void );
};

float DotProduct( const RVector2& V1, const RVector2& V2 );
float Distance( const RVector2& V1, const RVector2& V2 );
float DistanceSquared( const RVector2& V1, const RVector2& V2 );
RVector2 Normalize( const RVector2& V );

class RVector3
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
    // RVector3 constructors
    RVector3(  ) { }
    RVector3( float X, float Y, float Z );
    RVector3( const RVector3& V );
    RVector3( const float* F );
    RVector3& operator=( const RVector3& V );

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RVector3 operator+( const RVector3& V ) const;
    RVector3 operator+( float F ) const;
    RVector3& operator+=( const RVector3& V );
    RVector3& operator+=( float F );

    // Subtract
    RVector3 operator-( const RVector3& V ) const;
    RVector3 operator-( float F ) const;
    RVector3& operator-=( const RVector3& V );
    RVector3& operator-=( float F );

    // Multiply and divide
    RVector3 operator*( float F ) const;
    RVector3& operator*=( float F );
    RVector3 operator/( float F ) const;
    RVector3& operator/=( float F );

    // Comparison
    bool operator==( const RVector3& V ) const;
    bool operator!=( const RVector3& V ) const;

    // Vector operations

    // Vector-Vector operations
    float DotProduct( const RVector3& V ) const;
    RVector3 CrossProduct( const RVector3& V ) const;

    // Misc operations
    float Magnitude( void ) const;
    float MagnitudeSquared( void ) const;
    float Distance( const RVector3& V ) const;
    float DistanceSquared( const RVector3& V ) const;
    void Normalize( void );
    void Zero( void );
};

float DotProduct( const RVector3& V1, const RVector3& V2 );
RVector3 CrossProduct( const RVector3& V1, const RVector3& V2 );
float Distance( const RVector3& V1, const RVector3& V2 );
float DistanceSquared( const RVector3& V1, const RVector3& V2 );
RVector3 Normalize( const RVector3& V );
RVector3 RVector3Zero( void );

class RVector4
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
    // RVector4 constructors
    RVector4(  ) { }
    RVector4( float X, float Y, float Z, float W );
    RVector4( float X, float Y, float Z );
    RVector4( const RVector4& V );
    RVector4( const float* F );
    RVector4& operator=( const RVector4& V );

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RVector4 operator+( const RVector4& V ) const;
    RVector4 operator+( float F ) const;
    RVector4& operator+=( const RVector4& V );
    RVector4& operator+=( float F );

    // Subtract
    RVector4 operator-( const RVector4& V ) const;
    RVector4 operator-( float F ) const;
    RVector4& operator-=( const RVector4& V );
    RVector4& operator-=( float F );

    // Multiply and divide
    RVector4 operator*( float F ) const;
    RVector4& operator*=( float F );
    RVector4 operator/( float F ) const;
    RVector4& operator/=( float F );

    // Comparison
    bool operator==( const RVector4& V ) const;
    bool operator!=( const RVector4& V ) const;

    // Vector operations

    // Vector-Vector operations
    float DotProduct( const RVector4& V ) const;
    RVector4 CrossProduct( const RVector4& V ) const;

    // Misc operations
    float Magnitude( void ) const;
    float MagnitudeSquared( void ) const;
    float Distance( const RVector4& V ) const;
    float DistanceSquared( const RVector4& V ) const;
    void Normalize( void );
    void Zero( void );
};

float DotProduct( const RVector4& V1, const RVector4& V2 );
RVector4 CrossProduct( const RVector4& V1, const RVector4& V2 );
float Distance( const RVector4& V1, const RVector4& V2 );
float DistanceSquared( const RVector4& V1, const RVector4& V2 );
RVector4 Normalize( const RVector4& V );
RVector4 RVector4Zero(  );

//template< class T >
//class RMatrix3x3
//{
//public:
//    RVector3<T> r1;
//    RVector3<T> r2;
//    RVector3<T> r3;
//};
//
//template< class T >
//class RMatrix4x4
//{
//public:
//    RVector4<T> r1;
//    RVector4<T> r2;
//    RVector4<T> r3;
//    RVector4<T> r4;
//};

#endif // #ifndef _RIOTMATH_H_
