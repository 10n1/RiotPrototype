/*********************************************************\
File:       Math.h
Purpose:    Vector and matrix implementations
\*********************************************************/
#ifndef _RIOTMATH_H_
#define _RIOTMATH_H_

#include <math.h>
#include "Types.h"

//#define RIOT_USE_INTRINSICS

#ifdef RIOT_USE_INTRINSICS
#include <xmmintrin.h>
#include <emmintrin.h>
#define _MM_QUAD_ADD_PS(m1, m2, m3, m4)  ( _mm_add_ps( _mm_add_ps( m1, m2 ), _mm_add_ps( m3, m4 ) ) )
#endif

#define Swap(a,b) { t = a; a = b; b = t; }

static const float gs_fPi = 3.14159265358979f;
static const float gs_fPiRecip = (1.0f/gs_fPi);
static const float gs_fDegToRad = (gs_fPi/180.0f);
static const float gs_fRadToDeg = (180.0f/gs_fPi);
static const float gs_fEpsilon = (0.0001f);
inline float DegToRad( float fDeg ) { return fDeg * gs_fDegToRad; }
inline float RadToDeg( float fRad ) { return fRad * gs_fRadToDeg; }

#pragma warning(disable:4201)

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


class RMatrix2
{
public:
    /***************************************\
    | class members
    \***************************************/
    union
    {
        struct { float m11, m12, m21, m22; };
        float f[4];
    };

public:
    // RMatrix2 constructor
    RMatrix2() {}
    RMatrix2( float M11, float M12, float M21, float M22 );
    RMatrix2(const RMatrix2& M);
    RMatrix2(const float* F);
    RMatrix2& operator=(const RMatrix2& M);

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RMatrix2 operator+(const RMatrix2& M) const;
    RMatrix2 operator+(float F) const;
    RMatrix2& operator+=(const RMatrix2& M);
    RMatrix2& operator+=(float F);

    // Subtract
    RMatrix2 operator-(const RMatrix2& M) const;
    RMatrix2 operator-(float F) const;
    RMatrix2& operator-=(const RMatrix2& M);
    RMatrix2& operator-=(float F);

    // Multiply and divide
    RMatrix2 operator*(float F) const;
    RMatrix2& operator*=(float F);
    RMatrix2 operator/(float F) const;
    RMatrix2& operator/=(float F);

    // Comparison
    bool operator==(const RMatrix2& M) const;
    bool operator!=(const RMatrix2& M) const;

    // Matrix operations

    RMatrix2 operator*(const RMatrix2& M) const;
    RMatrix2 &operator*=(const RMatrix2& M);
    RVector2 operator*(const RVector2& V) const;

    // Misc matrix operations
    void Identity();
    void Transpose();
    float Determinant();
    void Inverse();
};

class RMatrix3
{
public:
    /***************************************\
    | class members
    \***************************************/
    union
    {
        struct{    float    m11, m12, m13,
            m21, m22, m23,
            m31, m32, m33; };
        float f[9];
    };
public:
    // RMatrix3 constructor
    RMatrix3() {}
    RMatrix3( float M11, float M12, float M13, 
                           float M21, float M22, float M23,
                           float M31, float M32, float M33);
    RMatrix3(const RMatrix3& M);
    RMatrix3(const float* F);
    RMatrix3& operator=(const RMatrix3& M);

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RMatrix3 operator+(const RMatrix3& M) const;
    RMatrix3 operator+(float F) const;
    RMatrix3& operator+=(const RMatrix3& M);
    RMatrix3& operator+=(float F);

    // Subtract
    RMatrix3 operator-(const RMatrix3& M) const;
    RMatrix3 operator-(float F) const;
    RMatrix3& operator-=(const RMatrix3& M);
    RMatrix3& operator-=(float F);

    // Multiply and divide
    RMatrix3 operator*(float F) const;
    RMatrix3& operator*=(float F);
    RMatrix3 operator/(float F) const;
    RMatrix3& operator/=(float F);

    // Comparison
    bool operator==(const RMatrix3& M) const;
    bool operator!=(const RMatrix3& M) const;

    // Matrix operations

    RMatrix3 operator*(const RMatrix3& M) const;
    RMatrix3 &operator*=(const RMatrix3& M);
    RVector3 operator*(const RVector3& V) const;

    // Misc matrix operations
    void Identity();
    void Transpose();
    float Determinant();
    void Inverse();
};


class RMatrix4
{
public:
    /***************************************\
    | class members
    \***************************************/
    union
    {
        struct 
        { float    
            m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34,
            m41, m42, m43, m44; 
        };
        float f[16];

        struct
        {
            RVector4 r0;
            RVector4 r1;
            RVector4 r2;
            RVector4 r3;
        };
#ifdef RIOT_USE_INTRINSICS
        struct
        {
            __m128 r1;
            __m128 r2; 
            __m128 r3;
            __m128 r4;
        };
#endif
    };
public:
    // RMatrix4 constructor
    RMatrix4() {}
    RMatrix4( float M11, float M12, float M13, float M14,
                           float M21, float M22, float M23, float M24,
                           float M31, float M32, float M33, float M34,
                           float M41, float M42, float M43, float M44 );
    RMatrix4(const RMatrix4& M);
    RMatrix4(const float* F);
    RMatrix4& operator=(const RMatrix4& M);

    /***************************************\
    | class methods
    \***************************************/
    // Scalar math operations

    // Add
    RMatrix4 operator+(const RMatrix4& M) const;
    RMatrix4 operator+(float F) const;
    RMatrix4& operator+=(const RMatrix4& M);
    RMatrix4& operator+=(float F);

    // Subtract
    RMatrix4 operator-(const RMatrix4& M) const;
    RMatrix4 operator-(float F) const;
    RMatrix4& operator-=(const RMatrix4& M);
    RMatrix4& operator-=(float F);

    // Multiply and divide
    RMatrix4 operator*(float F) const;
    RMatrix4& operator*=(float F);
    RMatrix4 operator/(float F) const;
    RMatrix4& operator/=(float F);

    // Comparison
    bool operator==(const RMatrix4& M) const;
    bool operator!=(const RMatrix4& M) const;

    // Matrix operations

    RMatrix4 operator*(const RMatrix4& M) const;
    RMatrix4 &operator*=(const RMatrix4& M);
    RVector4 operator*(const RVector4& V) const;

    // Misc matrix operations
    void Identity();
    void Transpose();
    float Determinant();
    void Inverse();

    // 3D Matrix operations
    void SetPosition(const RVector4 &v);
    void SetPosition(const float *F);
    void SetPosition(float x, float y, float z);
    RVector4 GetPosition(void) const;
};

RMatrix4 TranslationMatrix(float x, float y, float z, RMatrix4* pMatrix = 0);
RMatrix4 RotationXMatrix(float rad, RMatrix4* pMatrix = 0);
RMatrix4 RotationYMatrix(float rad, RMatrix4* pMatrix = 0);
RMatrix4 RotationZMatrix(float rad, RMatrix4* pMatrix = 0);
RMatrix4 ScalingMatrix(float x, float y, float z, RMatrix4* pMatrix = 0);
RMatrix4 RotationAxisMatrix(float rad, RVector4& axis, RMatrix4* pMatrix = 0);
RMatrix4 PerspectiveLHMatrix(float fFOV, float fAspect, float fZNear, float fZFar);
RMatrix4 LookAt(RVector4& pos, RVector4& target, RVector4& up);

RMatrix4 RMatrix4Identity();

#pragma warning(default:4201)

#endif // #ifndef _RIOTMATH_H_
