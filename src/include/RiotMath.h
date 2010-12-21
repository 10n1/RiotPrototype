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
    RiotVector2() { }
    RiotVector2(float X, float Y);
    RiotVector2(const RiotVector2& V);
    RiotVector2(const float* F);
    RiotVector2& operator=(const RiotVector2& V);

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RiotVector2 operator+(const RiotVector2& V) const;
    RiotVector2 operator+(float F) const;
    RiotVector2& operator+=(const RiotVector2& V);
    RiotVector2& operator+=(float F);

    // Subtract
    RiotVector2 operator-(const RiotVector2& V) const;
    RiotVector2 operator-(float F) const;
    RiotVector2& operator-=(const RiotVector2& V);
    RiotVector2& operator-=(float F);

    // Multiply and divide
    RiotVector2 operator*(float F) const;
    RiotVector2& operator*=(float F);
    RiotVector2 operator/(float F) const;
    RiotVector2& operator/=(float F);

    // Comparison
    bool operator==(const RiotVector2& V) const;
    bool operator!=(const RiotVector2& V) const;

    // Vector operations

    // Vector-Vector operations
    float DotProduct(const RiotVector2& V) const;

    // Misc operations
    float Magnitude(void) const;
    float MagnitudeSquared(void) const;
    float Distance(const RiotVector2& V) const;
    float DistanceSquared(const RiotVector2& V) const;
    void Normalize(void);
    void Zero(void);
};

float DotProduct(const RiotVector2& V1, const RiotVector2& V2);
float Distance(const RiotVector2& V1, const RiotVector2& V2);
float DistanceSquared(const RiotVector2& V1, const RiotVector2& V2);
RiotVector2 Normalize(const RiotVector2& V);

template< class T >
class RiotVector3
{
public:
    T x;
    T y;
    T z;
};

template< class T >
class RiotVector4
{
public:
    T x;
    T y;
    T z;
    T w;
};

template< class T >
class RiotMatrix3x3
{
public:
    RiotVector3<T> r1;
    RiotVector3<T> r2;
    RiotVector3<T> r3;
};

template< class T >
class RiotMatrix4x4
{
public:
    RiotVector4<T> r1;
    RiotVector4<T> r2;
    RiotVector4<T> r3;
    RiotVector4<T> r4;
};

#endif _RIOTMATH_H_
