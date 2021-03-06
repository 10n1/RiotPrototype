/*********************************************************\
File:           vectormath.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/17/2011 7:33:12 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "vectormath.h"

//-----------------------------------------------------------------------------
//  Matrix3
//-----------------------------------------------------------------------------
// Basic math
RMatrix3 operator+( const RMatrix3& l, const RMatrix3& r )
{
    return RMatrix3( l.r0 + r.r0, l.r1 + r.r1, l.r2 + r.r2 );
}

RMatrix3 operator-( const RMatrix3& l, const RMatrix3& r )
{
    return RMatrix3( l.r0 - r.r0, l.r1 - r.r1, l.r2 - r.r2 );
}

RMatrix3 operator*( const RMatrix3&a, float f )
{
    return RMatrix3( a.r0 * f, a.r1 * f, a.r2 * f );
}

RMatrix3 operator/( const RMatrix3&a, float f )
{
    f = 1.0f/f;
    return RMatrix3( a.r0 * f, a.r1 * f, a.r2 * f );
}


RMatrix3& operator+=( RMatrix3& l, const RMatrix3& r )
{
    l.r0 += r.r0;
    l.r1 += r.r1;
    l.r2 += r.r2;
    return l;
}

RMatrix3& operator-=( RMatrix3& l, const RMatrix3& r )
{
    l.r0 -= r.r0;
    l.r1 -= r.r1;
    l.r2 -= r.r2;
    return l;
}

RMatrix3& operator*=( RMatrix3&a, float f )
{
    a.r0 *= f;
    a.r1 *= f;
    a.r2 *= f;
    return a;
}



// Multiplication
RVector3 operator*( const RVector3& v, const RMatrix3& m )
{
    RVector3 a;

    a.x = Hadd( mul(m.r0, v) );
    a.y = Hadd( mul(m.r1, v) );    
    a.z = Hadd( mul(m.r2, v) );

    return a;
}

RMatrix3 operator*( const RMatrix3& l, const RMatrix3& r )
{
    RMatrix3 m;

    RVector3 c0( r.r0.x, r.r1.x, r.r2.x );
    RVector3 c1( r.r0.y, r.r1.y, r.r2.y );
    RVector3 c2( r.r0.z, r.r1.z, r.r2.z );

    m.r0.x = Hadd( mul( l.r0, c0 ) );
    m.r0.y = Hadd( mul( l.r0, c1 ) );
    m.r0.z = Hadd( mul( l.r0, c2 ) );    

    m.r1.x = Hadd( mul( l.r1, c0 ) );
    m.r1.y = Hadd( mul( l.r1, c1 ) );
    m.r1.z = Hadd( mul( l.r1, c2 ) );    

    m.r2.x = Hadd( mul( l.r2, c0 ) );
    m.r2.y = Hadd( mul( l.r2, c1 ) );
    m.r2.z = Hadd( mul( l.r2, c2 ) );

    return m;
}


// Matrix functions
float       Determinant( const RMatrix3& a )
{
    float det1 = ( (a.r0.x*a.r1.y*a.r2.z) + (a.r0.y*a.r1.z*a.r2.x) + (a.r0.z*a.r1.x*a.r2.y) );
    float det2 = ( (a.r0.z*a.r1.y*a.r2.x) + (a.r0.y*a.r1.x*a.r2.z) + (a.r0.x*a.r1.z*a.r2.y) );
    return det1 - det2;
}

RMatrix3    Inverse( const RMatrix3& a )
{
    RMatrix3 inv;

    inv.r0.x =   (a.r1.y*a.r2.z) - (a.r1.z*a.r2.y);
    inv.r0.y = -((a.r1.x*a.r2.z) - (a.r1.z*a.r2.x));
    inv.r0.z =   (a.r1.x*a.r2.y) - (a.r1.y*a.r2.x);

    inv.r1.x = -((a.r0.y*a.r2.z) - (a.r0.z*a.r2.y));
    inv.r1.y =   (a.r0.x*a.r2.z) - (a.r0.z*a.r2.x);
    inv.r1.z = -((a.r0.x*a.r2.y) - (a.r0.y*a.r2.x));

    inv.r2.x = (a.r0.y*a.r1.z) - (a.r0.z*a.r1.y);
    inv.r2.y = -((a.r0.x*a.r1.z) - (a.r0.z*a.r1.x));
    inv.r2.z = (a.r0.x*a.r1.y) - (a.r0.y*a.r1.x);

    inv = Transpose( inv );
    float fRecip = 1.0f/Determinant( a );
    inv *= fRecip;

    return inv;
}

RMatrix3    Transpose( const RMatrix3& a )
{
    RMatrix3 m(a);

    Swap(m.r0.y, m.r1.x);
    Swap(m.r0.z, m.r2.x);    
    Swap(m.r1.z, m.r2.y);

    return m;
}

RMatrix3 RMatrix3RotationX( float fRad )
{
    RMatrix3    m;
    float       c;
    float       s;

    c = cosf( fRad );
    s = sinf( fRad );

    m = RMatrix3( 1.0f, 0.0f, 0.0f,
        0.0f,    c,    s,
        0.0f,   -s,    c );
    return m;
}

RMatrix3 RMatrix3RotationY( float fRad )
{
    RMatrix3    m;
    float       c;
    float       s;

    c = cosf( fRad );
    s = sinf( fRad );

    m = RMatrix3(    c, 0.0f,   -s,
        0.0f, 1.0f, 0.0f,
        s, 0.0f,    c );

    return m;
}

RMatrix3 RMatrix3RotationZ( float fRad )
{
    RMatrix3    m;
    float       c;
    float       s;

    c = cosf( fRad );
    s = sinf( fRad );

    m = RMatrix3(    c,    s, 0.0f,
        -s,    c, 0.0f,
        0.0f, 0.0f, 1.0f );
    return m;
}

RMatrix3 RMatrix3Scale( float fScale )
{
    return RMatrix3( fScale,   0.0f,   0.0f,
        0.0f,   fScale,   0.0f,
        0.0f,     0.0f, fScale ); 
}


//-----------------------------------------------------------------------------
//  Matrix4
//-----------------------------------------------------------------------------
// Basic math
RMatrix4 operator+( const RMatrix4& l, const RMatrix4& r )
{
    return RMatrix4( l.r0 + r.r0, l.r1 + r.r1, l.r2 + r.r2, l.r3 + r.r3 );
}

RMatrix4 operator-( const RMatrix4& l, const RMatrix4& r )
{
    return RMatrix4( l.r0 - r.r0, l.r1 - r.r1, l.r2 - r.r2, l.r3 - r.r3 );
}

RMatrix4 operator*( const RMatrix4&a, float f )
{
    return RMatrix4( a.r0 * f, a.r1 * f, a.r2 * f, a.r3 * f );
}

RMatrix4 operator/( const RMatrix4&a, float f )
{
    f = 1.0f/f;
    return RMatrix4( a.r0 * f, a.r1 * f, a.r2 * f, a.r3 * f );
}


RMatrix4& operator+=( RMatrix4& l, const RMatrix4& r )
{
    l.r0 += r.r0;
    l.r1 += r.r1;
    l.r2 += r.r2;
    l.r3 += r.r3;
    return l;
}

RMatrix4& operator-=( RMatrix4& l, const RMatrix4& r )
{
    l.r0 -= r.r0;
    l.r1 -= r.r1;
    l.r2 -= r.r2;
    l.r3 -= r.r3;
    return l;
}

RMatrix4& operator*=( RMatrix4&a, float f )
{
    a.r0 *= f;
    a.r1 *= f;
    a.r2 *= f;
    a.r3 *= f;
    return a;
}



// Multiplication
RVector4 operator*( const RVector4& v, const RMatrix4& m )
{
    RVector4 a;

    RVector4 x = m.r0 * v.x;
    RVector4 y = m.r1 * v.y;
    RVector4 z = m.r2 * v.z;
    RVector4 w = m.r3 * v.w;

    a = x + y + z + w;

    return a;
}

RMatrix4 operator*( const RMatrix4& l, const RMatrix4& r )
{
    return RMatrix4( l.r0 * r, l.r1 * r, l.r2 * r, l.r3 * r );
}


// Matrix functions
float       Determinant( const RMatrix4& a )
{
    float fDet = 0.0f;

    // Row 1
    fDet += a.r0.x * Determinant( RMatrix3( a.r1.y,a.r1.z,a.r1.w, 
        a.r2.y,a.r2.z,a.r2.w, 
        a.r2.y,a.r2.z,a.r2.w) );

    fDet -=a .r0.y * Determinant( RMatrix3( a.r1.x,a.r1.z,a.r1.w, 
        a.r2.x,a.r2.z,a.r2.w, 
        a.r2.x,a.r2.z,a.r2.w ) );

    fDet += a.r0.z * Determinant( RMatrix3( a.r1.x,a.r1.y,a.r1.w, 
        a.r2.x,a.r2.y,a.r2.w, 
        a.r2.x,a.r2.y,a.r2.w) );

    fDet -= a.r0.w * Determinant( RMatrix3( a.r1.x,a.r1.y,a.r1.z, 
        a.r2.x,a.r2.y,a.r2.z, 
        a.r2.x,a.r2.y,a.r2.z) );

    return fDet;
}

RMatrix4    Inverse( const RMatrix4& a )
{
    RMatrix4 m;

    // Row 1
    m.r0.x = Determinant( RMatrix3(    
        a.r1.y,a.r1.z,a.r1.w, 
        a.r2.y,a.r2.z,a.r2.w, 
        a.r2.y,a.r2.z,a.r2.w) );

    m.r0.y = -Determinant( RMatrix3(    
        a.r1.x,a.r1.z,a.r1.w, 
        a.r2.x,a.r2.z,a.r2.w, 
        a.r2.x,a.r2.z,a.r2.w) );

    m.r0.z = Determinant( RMatrix3(    
        a.r1.x,a.r1.y,a.r1.w, 
        a.r2.x,a.r2.y,a.r2.w, 
        a.r2.x,a.r2.y,a.r2.w) );

    m.r0.w = -Determinant( RMatrix3(    
        a.r1.x,a.r1.y,a.r1.z, 
        a.r2.x,a.r2.y,a.r2.z, 
        a.r2.x,a.r2.y,a.r2.z) );

    // Row 2
    m.r1.x = -Determinant( RMatrix3(    
        a.r0.y,a.r0.z,a.r0.w, 
        a.r2.y,a.r2.z,a.r2.w, 
        a.r2.y,a.r2.z,a.r2.w) );

    m.r1.y = Determinant( RMatrix3(    
        a.r0.x,a.r0.z,a.r0.w, 
        a.r2.x,a.r2.z,a.r2.w, 
        a.r2.x,a.r2.z,a.r2.w) );

    m.r1.z = -Determinant( RMatrix3(    
        a.r0.x,a.r0.y,a.r0.w, 
        a.r2.x,a.r2.y,a.r2.w, 
        a.r2.x,a.r2.y,a.r2.w) );

    m.r1.w = Determinant( RMatrix3(    
        a.r0.x,a.r0.y,a.r0.z, 
        a.r2.x,a.r2.y,a.r2.z, 
        a.r2.x,a.r2.y,a.r2.z) );

    // Row 3
    m.r2.x = Determinant( RMatrix3(    
        a.r0.y,a.r0.z,a.r0.w, 
        a.r1.y,a.r1.z,a.r1.w, 
        a.r2.y,a.r2.z,a.r2.w) );

    m.r2.y = -Determinant( RMatrix3(    
        a.r0.x,a.r0.z,a.r0.w, 
        a.r1.x,a.r1.z,a.r1.w, 
        a.r2.x,a.r2.z,a.r2.w) );

    m.r2.z = Determinant( RMatrix3(    
        a.r0.x,a.r0.y,a.r0.w, 
        a.r1.x,a.r1.y,a.r1.w, 
        a.r2.x,a.r2.y,a.r2.w) );

    m.r2.w = -Determinant( RMatrix3(    
        a.r0.x,a.r0.y,a.r0.z, 
        a.r1.x,a.r1.y,a.r1.z, 
        a.r2.x,a.r2.y,a.r2.z) );

    // Row 4
    m.r2.x = -Determinant( RMatrix3(    
        a.r0.y,a.r0.z,a.r0.w, 
        a.r1.y,a.r1.z,a.r1.w, 
        a.r2.y,a.r2.z,a.r2.w) );

    m.r2.y = Determinant( RMatrix3(    
        a.r0.x,a.r0.z,a.r0.w, 
        a.r1.x,a.r1.z,a.r1.w, 
        a.r2.x,a.r2.z,a.r2.w) );

    m.r2.z = -Determinant( RMatrix3(    
        a.r0.x,a.r0.y,a.r0.w, 
        a.r1.x,a.r1.y,a.r1.w, 
        a.r2.x,a.r2.y,a.r2.w) );

    m.r2.w = Determinant( RMatrix3(    
        a.r0.x,a.r0.y,a.r0.z, 
        a.r1.x,a.r1.y,a.r1.z, 
        a.r2.x,a.r2.y,a.r2.z) );

    m = Transpose( m );
    float fRecip = 1.0f/Determinant( a );
    m *= fRecip;

    return m;
}

RMatrix4    Transpose( const RMatrix4& a )
{
    RMatrix4 m(a);

    Swap(m.r0.y,m.r1.x);
    Swap(m.r0.z,m.r2.x);
    Swap(m.r0.w,m.r3.x);

    Swap(m.r1.z,m.r2.y);
    Swap(m.r1.w,m.r3.y);

    Swap(m.r2.w,m.r3.z);

    return m;
}


RMatrix4 RMatrix4RotationX( float fRad )
{
    RMatrix4    m;
    float       c;
    float       s;

    c = cosf( fRad );
    s = sinf( fRad );

    m = RMatrix4( 1.0f, 0.0f, 0.0f, 0.0f, 
        0.0f,    c,    s, 0.0f, 
        0.0f,   -s,    c, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f );
    return m;
}

RMatrix4 RMatrix4RotationY( float fRad )
{
    RMatrix4    m;
    float       c;
    float       s;

    c = cosf( fRad );
    s = sinf( fRad );

    m = RMatrix4(    c, 0.0f,   -s, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        s, 0.0f,    c, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f );

    return m;
}

RMatrix4 RMatrix4RotationZ( float fRad )
{
    RMatrix4    m;
    float       c;
    float       s;

    c = cosf( fRad );
    s = sinf( fRad );

    m = RMatrix4(    c,    s, 0.0f, 0.0f, 
        -s,    c, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f );
    return m;
}

RMatrix4 RMatrix4RotationAxis( const RVector3& axis, float fRad )
{
    float c = cosf(fRad);
    float s = sinf(fRad);
    float t = 1 - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    RMatrix4 m = RMatrix4Identity();

    m(0,0) = t * x * x + c;
    m(0,1) = (t * x * y) - s * z;
    m(0,2) = (t * x * z) + s * y;

    m(1,0) = (t * x * y) + (s * z);
    m(1,1) = (t * y * y) + c;
    m(1,2) = (t * y * z) - (s * x);

    m(2,0) = (t * x * z) - (s * y);
    m(2,1) = (t * y * z) + (s * x);
    m(2,2) = (t * z * z) + c;

    return m;
}

RMatrix4 RMatrix4Scale( float fScale )
{
    return RMatrix4( fScale,   0.0f,   0.0f, 0.0f, 
        0.0f,   fScale,   0.0f, 0.0f, 
        0.0f,     0.0f, fScale, 0.0f, 
        0.0f,     0.0f,   0.0f, 1.0f );
}

RMatrix4 RMatrix4Translation( const RVector3& t )
{
    return RMatrix4( 1.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 
        t.x,  t.y,  t.z, 1.0f );
}

RMatrix4 RMatrix4Translation( const RVector4& t )
{
    return RMatrix4( 1.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 
        t.x,  t.y,  t.z, 1.0f );
}

RMatrix4 RMatrix4Translation( float x, float y, float z )
{
    return RMatrix4( 1.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 
        x,    y,    z, 1.0f );
}


RMatrix4 RMatrix4PerspectiveLH( float fFOV, float fAspect, float fZNear, float fZFar)
{
    RMatrix4 m = RMatrix4Identity();

    float y = 1/(tanf(fFOV/2));
    float diff = fZFar-fZNear;
    float div = fZFar / diff;
    m(0,0) =  y / fAspect;
    m(1,1) = y;
    m(2,2) = div;
    m(2,3) = 1;
    m(3,2) = -fZNear * div;
    m(3,3) = 0;
    return m;
}

RVector3 RMatrix4GetXAxis( const RMatrix4& m  )
{
    return m.r0.xyz();
}

RVector3 RMatrix4GetYAxis( const RMatrix4& m  )
{
    return m.r1.xyz();
}

RVector3 RMatrix4GetZAxis( const RMatrix4& m  )
{
    return m.r2.xyz();
}

RVector3 RMatrix4GetTranslation( const RMatrix4& m  )
{
    return m.r3.xyz();
}


//-----------------------------------------------------------------------------
//  Quaternion
//-----------------------------------------------------------------------------
RVector3 Rotate( const RQuaternion& q, const RVector3& v )
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

    return RVector3( (1-2*(ysq+zsq))*v.x + (    2*(xy+zw))*v.y + (    2*(xz-yw))*v.z,
        (    2*(xy-zw))*v.x + (1-2*(xsq-zsq))*v.y + (    2*(yz+xw))*v.z,
        (    2*(xz+yw))*v.x + (    2*(yz-xw))*v.y + (1-2*(xsq+ysq))*v.z );
}

RQuaternion operator*( const RQuaternion& l, const RQuaternion& r )
{
    return RQuaternion(
        l.w*r.x + l.x*r.w + l.y*r.z - l.z*r.y,
        l.w*r.y - l.x*r.z + l.y*r.w + l.z*r.x,
        l.w*r.z + l.x*r.y - l.y*r.x + l.z*r.w,
        l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z );
}

RQuaternion Lerp( const RQuaternion& a, const RQuaternion& b, float t )
{
    float inv = 1.0f - t;

    return RQuaternion(
        inv*a.x + t*b.x,
        inv*a.y + t*b.y,
        inv*a.z + t*b.z,
        inv*a.w + t*b.w );
}

RQuaternion Slerp( const RQuaternion& a, const RQuaternion& b, float t )
{
    return Lerp( a, b, t );
}
