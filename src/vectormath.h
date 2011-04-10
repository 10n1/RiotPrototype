/*********************************************************\
File:           vectormath.h
Purpose:        3D math library
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 10:21:56 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _VECTORMATH_H_
#define _VECTORMATH_H_
#include <math.h>
#include "types.h"

#pragma warning(disable:4201)

//////////////////////////////////////////
// Math constants
const float gs_EPSILON  = 0.0001f;
const float gs_PI       = (float)(4.0 * atan(1.0));
const float gs_2PI      = (float)(8.0 * atan(1.0));
const float gs_HALFPI   = (float)(2.0 * atan(1.0));
const float gs_HINVPI   = (float)(1.0/(4.0 * atan(1.0)));
const float gs_DEGTORAD = (gs_PI/180.0f);
const float gs_RADTODEG = (180.0f/gs_PI);

template<class T> inline T Max( const T& a, const T& b ) { return (a>b) ? a : b; }
template<class T> inline T Min( const T& a, const T& b ) { return (a<b) ? a : b; }
template<class T> inline void Swap( T& a, T& b ) { T tmp = a; a = b; b = tmp; }

inline float Square( float f ) { return f*f; }

//-----------------------------------------------------------------------------
//  Vector2
//-----------------------------------------------------------------------------
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
    
    /***************************************\
    | class methods
    \***************************************/
    // RVector2 constructors
    inline RVector2(  ) { }
    inline RVector2( float X, float Y ) : x(X), y(Y) { }
    inline RVector2( const float* F ) : x(F[0]), y(F[1]) { }
    inline RVector2& operator=( const RVector2& a ) { x = a.x, y = a.y; return *this; }

    // Misc
    inline operator float*() { return f; }
    inline operator const float*() const { return f; }
    inline float& operator[](int i) { return f[i]; }
    inline const float& operator[](int i) const { return f[i]; }
};

// small basic math
inline RVector2 operator+( const RVector2& l, const RVector2& r ) { return RVector2( l.x + r.x, l.y + r.y ); }
inline RVector2 operator-( const RVector2& l, const RVector2& r ) { return RVector2( l.x - r.x, l.y - r.y ); }
inline uint operator==( const RVector2& l, const RVector2& r ) { return l.x == r.x && l.y == r.y; }
inline RVector2 RVector2Zero( void ) { return RVector2( 0.0f, 0.0f ); }
inline RVector2 mul( const RVector2& l, const RVector2& r ) { return RVector2( l.x*r.x, l.y*r.y ); }
inline float Hadd( const RVector2& v ) { return v.x + v.y; }

//-----------------------------------------------------------------------------
//  Vector3
//-----------------------------------------------------------------------------
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
    
    /***************************************\
    | class methods
    \***************************************/
    // RVector3 constructors
    inline RVector3(  ) { }
    inline RVector3( float X, float Y, float Z ) : x(X), y(Y), z(Z) { }
    inline RVector3( const float* F ) : x(F[0]), y(F[1]), z(F[2]) { }
    inline RVector3& operator=( const RVector3& a ) { x = a.x, y = a.y, z = a.z; return *this; }

    // Misc
    inline operator float*() { return f; }
    inline operator const float*() const { return f; }
    inline float& operator[](int i) { return f[i]; }
    inline const float& operator[](int i) const { return f[i]; }
};

// Basic math operations
inline RVector3 operator+( const RVector3& l, const RVector3& r ) { return RVector3( l.x + r.x, l.y + r.y, l.z + r.z ); }
inline RVector3 operator-( const RVector3& l, const RVector3& r ) { return RVector3( l.x - r.x, l.y - r.y, l.z - r.z ); }
inline RVector3 operator-( const RVector3& a ) { return RVector3( -a.x, -a.y, -a.z ); }

inline RVector3 operator/( const RVector3& a, float f ) { f = 1.0f/f; return RVector3( a.x * f, a.y * f, a.z * f ); }
inline RVector3 operator*( const RVector3& a, float f ) { return RVector3( a.x * f, a.y * f, a.z * f ); }
inline RVector3 operator*( float f, const RVector3& a ) { return a * f; }

inline RVector3& operator+=( RVector3& l, const RVector3& r ) { l.x += r.x, l.y += r.y, l.z += r.z; return l; }
inline RVector3& operator-=( RVector3& l, const RVector3& r ) { l.x -= r.x, l.y -= r.y, l.z -= r.z; return l; }
inline RVector3& operator/=( RVector3& a, float f ) { f = 1.0f/f; a.x *= f, a.y *= f, a.z *= f; return a; }
inline RVector3& operator*=( RVector3& a, float f ) { a.x *= f, a.y *= f, a.z *= f; return a; }

inline RVector3 mul( const RVector3& l, const RVector3& r ) { return RVector3( l.x*r.x, l.y*r.y, l.z*r.z ); }
inline float Hadd( const RVector3& v ) { return v.x + v.y + v.z; }

// Comparisons
inline uint operator==( const RVector3& l, const RVector3& r ) { return l.x == r.x && l.y == r.y && l.z == r.z; }
inline uint operator!=( const RVector3& l, const RVector3& r ) { return !(l == r); }

// 3D math functions
inline float DotProduct( const RVector3& l, const RVector3& r ) { return l.x * r.x + l.y * r.y + l.z * r.z; } 
inline RVector3 CrossProduct( const RVector3& l, const RVector3& r ) { return RVector3(l.y*r.z-l.z*r.y,l.z*r.x-l.x*r.z,l.x*r.y-l.y*r.x); }
inline float MagnitudeSq( const RVector3& a ) { return DotProduct(a,a); }
inline float Magnitude( const RVector3& a ) { return sqrtf(DotProduct(a,a)); }
inline RVector3 Normalize( const RVector3&a ) { return a/Magnitude(a); }
inline RVector3 RVector3Zero( void ) { return RVector3( 0.0f, 0.0f, 0.0f ); }

//-----------------------------------------------------------------------------
//  Vector4
//-----------------------------------------------------------------------------
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
    
    /***************************************\
    | class methods
    \***************************************/
    // RVector4 constructors
    inline RVector4(  ) { }
    inline RVector4( float X, float Y, float Z, float W ) : x(X), y(Y), z(Z), w(W) { }
    inline RVector4( const RVector3& V, float W ): x(V.x), y(V.y), z(V.z), w(W) { }
    inline RVector4( const float* F ) : x(F[0]), y(F[1]), z(F[2]), w(F[3]) { }
    inline RVector4& operator=( const RVector4& a ) { x = a.x, y = a.y, z = a.z, w = a.w; return *this; }

    // Misc
    inline operator float*() { return f; }
    inline operator const float*() const { return f; }
    inline float& operator[](int i) { return f[i]; }
    inline const float& operator[](int i) const { return f[i]; }

    inline RVector3& xyz( void ) { return *((RVector3*)this); }
    inline const RVector3& xyz( void ) const { return *((RVector3*)this); }
};

// Basic math operations
inline RVector4 operator+( const RVector4& l, const RVector4& r ) { return RVector4( l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w ); }
inline RVector4 operator-( const RVector4& l, const RVector4& r ) { return RVector4( l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w); }
inline RVector4 operator-( const RVector4& a ) { return RVector4( -a.x, -a.y, -a.z, -a.w ); }

inline RVector4 operator/( const RVector4& a, float f ) { f = 1.0f/f; return RVector4( a.x * f, a.y * f, a.z * f, a.w * f ); }
inline RVector4 operator*( const RVector4& a, float f ) { return RVector4( a.x * f, a.y * f, a.z * f, a.w * f ); }
inline RVector4 operator*( float f, const RVector4& a ) { return a * f; }

inline RVector4& operator+=( RVector4& l, const RVector4& r ) { l.x += r.x, l.y += r.y, l.z += r.z, l.w += r.w; return l; }
inline RVector4& operator-=( RVector4& l, const RVector4& r ) { l.x -= r.x, l.y -= r.y, l.z -= r.z, l.w -= r.w; return l; }
inline RVector4& operator/=( RVector4& a, float f ) { f = 1.0f/f; a.x *= f, a.y *= f, a.z *= f, a.w *= f; return a; }
inline RVector4& operator*=( RVector4& a, float f ) { a.x *= f, a.y *= f, a.z *= f, a.w *= f; return a; }

inline RVector4 mul( const RVector4& l, const RVector4& r ) { return RVector4( l.x*r.x, l.y*r.y, l.z*r.z, l.w*r.w ); }
inline float Hadd( const RVector4& v ) { return v.x + v.y + v.z + v.w; }

// Comparisons
inline uint operator==( const RVector4& l, const RVector4& r ) { return l.x == r.x && l.y == r.y && l.z == r.z && l.w == r.w; }
inline uint operator!=( const RVector4& l, const RVector4& r ) { return !(l == r); }

// 3D math functions
inline float DotProduct( const RVector4& l, const RVector4& r ) { return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w; } 
inline float MagnitudeSq( const RVector4& a ) { return DotProduct(a,a); }
inline float Magnitude( const RVector4& a ) { return sqrtf(DotProduct(a,a)); }
inline RVector4 Normalize( const RVector4&a ) { float recip = 1.0f/Magnitude(a); return a * recip; }
inline RVector4 Vector4Zero( void ) { return RVector4( 0.0f, 0.0f, 0.0f, 0.0f ); }


//-----------------------------------------------------------------------------
//  Matrix3
//-----------------------------------------------------------------------------
class RMatrix3
{ 
public:
    /***************************************\
    | class members
    \***************************************/
    RVector3    r0;
    RVector3    r1;
    RVector3    r2;

    /***************************************\
    | class methods
    \***************************************/
    inline RMatrix3() {}
    inline RMatrix3( float M11, float M12, float M13, 
                     float M21, float M22, float M23,
                     float M31, float M32, float M33)
                     : r0( M11, M12, M13 )
                     , r1( M21, M22, M23 )
                     , r2( M31, M32, M33 )
    {
    }
    inline RMatrix3( const RVector3& R0, const RVector3& R1, const RVector3& R2 ) : r0(R0), r1(R1), r2(R2) { }
    inline RMatrix3(const float* F) : r0( F ), r1( F + 3 ), r2( F + 6 ) { }
    inline RMatrix3& operator=( const RMatrix3& a ) 
    { 
        r0 = a.r0;
        r1 = a.r1;
        r2 = a.r2;
        return *this; 
    }

    
    // Misc
    inline operator float*() { return &r0.x; }
    inline operator const float*() const { return &r0.x; }
    inline RVector3& operator[](int i) { return (&r0)[i]; }
    inline const RVector3& operator[](int i) const { return (&r0)[i]; }

    inline float& operator()(int r, int c ) { return ((&r0)[r])[c]; }
    inline const float& operator()(int r, int c ) const { return ((&r0)[r])[c]; }
    
    inline float& m11() { return r0.x; }
};

// Basic math
RMatrix3 operator+( const RMatrix3& l, const RMatrix3& r );
RMatrix3 operator-( const RMatrix3& l, const RMatrix3& r );
RMatrix3 operator*( const RMatrix3&a, float f );
RMatrix3 operator/( const RMatrix3&a, float f );

RMatrix3& operator+=( RMatrix3& l, const RMatrix3& r );
RMatrix3& operator-=( RMatrix3& l, const RMatrix3& r );
RMatrix3& operator*=( RMatrix3&a, float f );

// Comparisons
inline uint operator==( const RMatrix3& l, const RMatrix3& r ) { return l.r0 == r.r0 && l.r1 == r.r1 && l.r2 == r.r2; }
inline uint operator!=( const RMatrix3& l, const RMatrix3& r ) { return !(l == r); }

// Multiplication
RVector3 operator*( const RVector3& v, const RMatrix3& m );
RMatrix3 operator*( const RMatrix3& l, const RMatrix3& r );

// Matrix functions
float       Determinant( const RMatrix3& a );
RMatrix3    Inverse( const RMatrix3& a );
RMatrix3    Transpose( const RMatrix3& a );

RMatrix3 RMatrix3RotationX( float fRad );
RMatrix3 RMatrix3RotationY( float fRad );
RMatrix3 RMatrix3RotationZ( float fRad );
RMatrix3 RMatrix3Scale( float fScale );

inline RMatrix3 RMatrix3Identity( void ) 
{ 
    return RMatrix3( 1.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 1.0f ); 
}


//-----------------------------------------------------------------------------
//  Matrix4
//-----------------------------------------------------------------------------
class RMatrix4
{ 
public:
    /***************************************\
    | class members
    \***************************************/
    RVector4 r0;
    RVector4 r1;
    RVector4 r2;
    RVector4 r3;

    /***************************************\
    | class methods
    \***************************************/
    inline RMatrix4() {}
    inline RMatrix4( float M11, float M12, float M13, float M14,
                     float M21, float M22, float M23, float M24,
                     float M31, float M32, float M33, float M34,
                     float M41, float M42, float M43, float M44 )
                    : r0(M11, M12, M13, M14)
                    , r1(M21, M22, M23, M24)
                    , r2(M31, M32, M33, M34)
                    , r3(M41, M42, M43, M44) { }
    inline RMatrix4( const RVector4& R0, const RVector4& R1, const RVector4& R2, const RVector4& R3 ) : r0(R0), r1(R1), r2(R2), r3(R3) { }
    inline RMatrix4( const float* F) : r0( F ), r1( F + 4 ), r2( F + 8 ), r3( F + 12 ) { }
    inline RMatrix4( const RMatrix3& m ) : r0( m.r0, 0.0f ), r1( m.r1, 0.0f ), r2( m.r2, 0.0f ), r3( 0.0f, 0.0f, 0.0f, 1.0f ) { }
    inline RMatrix4& operator=( const RMatrix4& a ) 
    { 
        r0 = a.r0;
        r1 = a.r1;
        r2 = a.r2;
        r3 = a.r3;
        return *this; 
    }
    
    // Misc
    inline operator float*() { return &r0.x; }
    inline operator const float*() const { return &r0.x; }
    inline RVector4& operator[](int i) { return (&r0)[i]; }
    inline const RVector4& operator[](int i) const { return (&r0)[i]; }

    inline float& operator()(int r, int c ) { return ((&r0)[r])[c]; }
    inline const float& operator()(int r, int c ) const { return ((&r0)[r])[c]; }
};

// Basic math
RMatrix4 operator+( const RMatrix4& l, const RMatrix4& r );
RMatrix4 operator-( const RMatrix4& l, const RMatrix4& r );
RMatrix4 operator*( const RMatrix4&a, float f );
RMatrix4 operator/( const RMatrix4&a, float f );

RMatrix4& operator+=( RMatrix4& l, const RMatrix4& r );
RMatrix4& operator-=( RMatrix4& l, const RMatrix4& r );
RMatrix4& operator*=( RMatrix4&a, float f );

// Comparisons
inline uint operator==( const RMatrix4& l, const RMatrix4& r ) { return l.r0 == r.r0 && l.r1 == r.r1 && l.r2 == r.r2 && l.r3 == r.r3; }
inline uint operator!=( const RMatrix4& l, const RMatrix4& r ) { return !(l == r); }

// Multiplication
RVector4 operator*( const RVector4& v, const RMatrix4& m );
RMatrix4 operator*( const RMatrix4& l, const RMatrix4& r );

// Matrix functions
float       Determinant( const RMatrix4& a );
RMatrix4    Inverse( const RMatrix4& a );
RMatrix4    Transpose( const RMatrix4& a );


RMatrix4 RMatrix4RotationX( float fRad );
RMatrix4 RMatrix4RotationY( float fRad );
RMatrix4 RMatrix4RotationZ( float fRad );
RMatrix4 RMatrix4Scale( float fScale );
RMatrix4 RMatrix4Translation( const RVector3& t );
RMatrix4 RMatrix4Translation( const RVector4& t );
RMatrix4 RMatrix4Translation( float x, float y, float z );

RVector3 RMatrix4GetXAxis( const RMatrix4& m );
RVector3 RMatrix4GetYAxis( const RMatrix4& m );
RVector3 RMatrix4GetZAxis( const RMatrix4& m );
RVector3 RMatrix4GetTranslation( const RMatrix4& m  );

inline RMatrix4 RMatrix4Identity( void ) 
{ 
    return RMatrix4( 1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f, 
                     0.0f, 0.0f, 0.0f, 1.0f ); 
}


//-----------------------------------------------------------------------------
//  Quaternion
//-----------------------------------------------------------------------------
class RQuaternion : public RVector4
{
public:
    /***************************************\
    | class members
    \***************************************/

    
    /***************************************\
    | class methods
    \***************************************/
    // RQuaternion constructors
    inline RQuaternion(  ) { }
    inline RQuaternion( float X, float Y, float Z, float W ) : RVector4( X, Y, Z, W ) { }
    inline RQuaternion( const float* F ) : RVector4( F ) { }

    // Misc
    inline operator float*() { return f; }
    inline operator const float*() const { return f; }
    inline float& operator[](int i) { return f[i]; }
    inline const float& operator[](int i) const { return f[i]; }
};

// Basic math functions
inline RQuaternion operator+( const RQuaternion& l, const RQuaternion& r ) { return RQuaternion( (RVector4&)l + (RVector4&)r ); }
inline RQuaternion operator-( const RQuaternion& l, const RQuaternion& r ) { return RQuaternion( (RVector4&)l - (RVector4&)r ); }
inline RQuaternion operator-( const RQuaternion& a ) { return RQuaternion( -(RVector4&)a ); }

inline RQuaternion operator*( const RQuaternion& a, float f ) { return RQuaternion( (RVector4&)a * f ); }

inline RQuaternion& operator+=( RQuaternion& l, const RQuaternion& r ) { l.x += r.x, l.y += r.y, l.z += r.z, l.w += r.w; return l; }
inline RQuaternion& operator-=( RQuaternion& l, const RQuaternion& r ) { l.x -= r.x, l.y -= r.y, l.z -= r.z, l.w -= r.w; return l; }
inline RQuaternion& operator*=( RQuaternion& a, float f ) { return (RQuaternion&)((RVector4&)a *= f); }

// 3D functions
inline RVector3 RQuatGetXAxis( const RQuaternion& q ) { return RVector3( 1-2*(q.y*q.y+q.z*q.z), 2*(q.x*q.y+q.w*q.z), 2*(q.x*q.z-q.y*q.w) ); }
inline RVector3 RQuatGetYAxis( const RQuaternion& q ) { return RVector3( 2*(q.x*q.y-q.z*q.w), 1-2*(q.x*q.x+q.z*q.z), 2*(q.y*q.z+q.x*q.w) ); }
inline RVector3 RQuatGetZAxis( const RQuaternion& q ) { return RVector3( 2*(q.x*q.z+q.y*q.w), 2*(q.y*q.z+q.x*q.w), 1-2*(q.x*q.x+q.y*q.y) ); }
inline RMatrix3 RQuatGetMatrix( const RQuaternion& q ) 
{ 
    float xsq = q.x * q.x;
    float ysq = q.y * q.y;
    float zsq = q.z * q.z;
    
    float xy = q.x * q.y;
    float zw = q.z * q.w;
    float xz = q.x * q.z;
    float yw = q.y * q.w;
    float yz = q.y * q.z;
    float xw = q.x * q.w;
    
    return RMatrix3( 1-2*(ysq+zsq),     2*(xy+zw),     2*(xz-yw),
                         2*(xy-zw), 1-2*(xsq-zsq),     2*(yz+xw),
                         2*(xz+yw),     2*(yz-xw), 1-2*(xsq+ysq) ); 
}
inline RQuaternion RQuatFromAxisAngle( const RVector3& axis, float angle )
{
    angle /= 2.0f;
    RVector3 v = Normalize( axis ) * sinf( angle );
    return RQuaternion( v.x, v.y, v.z, cosf( angle ) );
}

RQuaternion operator*( const RQuaternion& l, const RQuaternion& r );

inline RQuaternion Normalize( const RQuaternion& q ) { float recip = 1.0f/Magnitude(q); return q * recip; }
inline RQuaternion Conjugate( const RQuaternion& q ) { return RQuaternion( -q.x, -q.y, -q.z, q.w ); }
inline RQuaternion Inverse( const RQuaternion& q ) { return Conjugate( q ); }

inline RQuaternion RQuaternionZero( void ) { return RQuaternion( 0.0f, 0.0f, 0.0f, 1.0f ); }

RVector3 Rotate( const RQuaternion& q, const RVector3& v );
RQuaternion Lerp( const RQuaternion& a, const RQuaternion& b, float t );
RQuaternion Slerp( const RQuaternion& a, const RQuaternion& b, float t );

//-----------------------------------------------------------------------------
//  Quaternion
//-----------------------------------------------------------------------------
class RTransform
{
public:
    /***************************************\
    | class members
    \***************************************/
    RQuaternion orientation;
    RVector3    position;
    float       scale;
    
    /***************************************\
    | class methods
    \***************************************/
    RTransform() : orientation( RQuaternionZero() ), position( RVector3Zero() ), scale( 1.0f ) { }
    RTransform( const RQuaternion& o, const RVector3& p, float s = 1.0f )
        : orientation( 0 ), position( p ), scale( s ) { }

    inline RMatrix4 GetTransformMatrix( void )
    {
        float xsq = orientation.x * orientation.x;
        float ysq = orientation.y * orientation.y;
        float zsq = orientation.z * orientation.z;

        float xy = orientation.x * orientation.y;
        float zw = orientation.z * orientation.w;
        float xz = orientation.x * orientation.z;
        float yw = orientation.y * orientation.w;
        float yz = orientation.y * orientation.z;
        float xw = orientation.x * orientation.w;

        return RMatrix4( 
        1-2*(ysq+zsq) * scale,     2*(xy+zw) * scale,     2*(xz-yw) * scale, 0.0f,
            2*(xy-zw) * scale, 1-2*(xsq-zsq) * scale,     2*(yz+xw) * scale, 0.0f,
            2*(xz+yw) * scale,     2*(yz-xw) * scale, 1-2*(xsq+ysq) * scale, 0.0f,
                   position.x,            position.y,            position.z, 1.0f ); 
    }

    inline void TranslateX( float f ) { position += RQuatGetXAxis( orientation ) * f; }
    inline void TranslateY( float f ) { position += RQuatGetYAxis( orientation ) * f; }
    inline void TranslateZ( float f ) { position += RQuatGetZAxis( orientation ) * f; }

    inline void RotateWorldX( float rad ) { orientation = orientation * RQuatFromAxisAngle( RVector3( 1.0f, 0.0f, 0.0f ), rad ); }
    inline void RotateWorldY( float rad ) { orientation = orientation * RQuatFromAxisAngle( RVector3( 0.0f, 1.0f, 0.0f ), rad ); }
    inline void RotateWorldZ( float rad ) { orientation = orientation * RQuatFromAxisAngle( RVector3( 0.0f, 0.0f, 1.0f ), rad ); } 
    inline void RotateLocalX( float rad ) { orientation = orientation * RQuatFromAxisAngle( RQuatGetXAxis( orientation ), rad ); }
    inline void RotateLocalY( float rad ) { orientation = orientation * RQuatFromAxisAngle( RQuatGetYAxis( orientation ), rad ); }
    inline void RotateLocalZ( float rad ) { orientation = orientation * RQuatFromAxisAngle( RQuatGetZAxis( orientation ), rad ); } 
};

#pragma warning(disable:4201)

#endif // #ifndef _VECTORMATH_H_
