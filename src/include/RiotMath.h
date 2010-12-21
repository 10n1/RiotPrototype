/*********************************************************\
File:       RiotMath.h
Purpose:    Vector and matrices implementations
\*********************************************************/
#ifndef _RIOTMATH_H_
#define _RIOTMATH_H_

#include <math.h>

static const float gs_fPi = 3.14159265358979f;
static const float gs_fPiRecip = (1.0f/gs_fPi);
static const float gs_fDegToRad = (gs_fPi/180.0f);
static const float gs_fRadToDeg = (180.0f/gs_fPi);
static const float gs_fEpsilon = (0.0001f);
_inline float DegToRad( float fDeg ) { return fDeg * gs_fDegToRad; }
_inline float RadToDeg( float fRad ) { return fRad * gs_fRadToDeg; }

class Vector2
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

    // Vector2 constructors
    Vector2() { }
    Vector2(float X, float Y);
    Vector2(const Vector2& V);
    Vector2(const float* F);
    Vector2& operator=(const Vector2& V);

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    Vector2 operator+(const Vector2& V) const;
    Vector2 operator+(float F) const;
    Vector2& operator+=(const Vector2& V);
    Vector2& operator+=(float F);

    // Subtract
    Vector2 operator-(const Vector2& V) const;
    Vector2 operator-(float F) const;
    Vector2& operator-=(const Vector2& V);
    Vector2& operator-=(float F);

    // Multiply and divide
    Vector2 operator*(float F) const;
    Vector2& operator*=(float F);
    Vector2 operator/(float F) const;
    Vector2& operator/=(float F);

    // Comparison
    bool operator==(const Vector2& V) const;
    bool operator!=(const Vector2& V) const;

    // Vector operations

    // Vector-Vector operations
    float DotProduct(const Vector2& V) const;

    // Misc operations
    float Magnitude(void) const;
    float MagnitudeSquared(void) const;
    float Distance(const Vector2& V) const;
    float DistanceSquared(const Vector2& V) const;
    void Normalize(void);
    void Zero(void);
};

float DotProduct(const Vector2& V1, const Vector2& V2);
float Distance(const Vector2& V1, const Vector2& V2);
float DistanceSquared(const Vector2& V1, const Vector2& V2);
Vector2 Normalize(const Vector2& V);

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
