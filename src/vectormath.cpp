/*********************************************************\
File:           vectormath.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 9:59:46 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "vectormath.h"

namespace Riot
{

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

    a.x = m.m11 * v.x + m.m12 * v.y + m.m13 * v.z;
    a.y = m.m21 * v.x + m.m22 * v.y + m.m23 * v.z;
    a.z = m.m31 * v.x + m.m32 * v.y + m.m33 * v.z;

    return a;
}

RMatrix3 operator*( const RMatrix3& l, const RMatrix3& r )
{
    RMatrix3 m;

    m.m11 = l.m11 * r.m11 + l.m12 * r.m21 + l.m13 * r.m31;
    m.m12 = l.m11 * r.m12 + l.m12 * r.m22 + l.m13 * r.m32;
    m.m13 = l.m11 * r.m13 + l.m12 * r.m23 + l.m13 * r.m33;

    m.m21 = l.m21 * r.m11 + l.m22 * r.m21 + l.m23 * r.m31;
    m.m22 = l.m21 * r.m12 + l.m22 * r.m22 + l.m23 * r.m32;
    m.m23 = l.m21 * r.m13 + l.m22 * r.m23 + l.m23 * r.m33;

    m.m31 = l.m31 * r.m11 + l.m32 * r.m21 + l.m33 * r.m31;
    m.m32 = l.m31 * r.m12 + l.m32 * r.m22 + l.m33 * r.m32;
    m.m33 = l.m31 * r.m13 + l.m32 * r.m23 + l.m33 * r.m33;

    return m;
}


// Matrix functions
float       Determinant( const RMatrix3& a )
{
    float det1 = ( (a.m11*a.m22*a.m33) + (a.m12*a.m23*a.m31) + (a.m13*a.m21*a.m32) );
    float det2 = ( (a.m13*a.m22*a.m31) + (a.m12*a.m21*a.m33) + (a.m11*a.m23*a.m32) );
    return det1 - det2;
}

RMatrix3    Inverse( const RMatrix3& a )
{
    RMatrix3 inv;

    inv.m11 =   (a.m22*a.m33) - (a.m23*a.m32);
    inv.m12 = -((a.m21*a.m33) - (a.m23*a.m31));
    inv.m13 =   (a.m21*a.m32) - (a.m22*a.m31);

    inv.m21 = -((a.m12*a.m33) - (a.m13*a.m32));
    inv.m22 =   (a.m11*a.m33) - (a.m13*a.m31);
    inv.m23 = -((a.m11*a.m32) - (a.m12*a.m31));

    inv.m31 = (a.m12*a.m23) - (a.m13*a.m22);
    inv.m32 = -((a.m11*a.m23) - (a.m13*a.m21));
    inv.m33 = (a.m11*a.m22) - (a.m12*a.m21);

    inv = Transpose( inv );
    float fRecip = 1.0f/Determinant( a );
    inv *= fRecip;

    return inv;
}

RMatrix3    Transpose( const RMatrix3& a )
{
    RMatrix3 m(a);

    Swap(m.m12, m.m21);
    Swap(m.m13, m.m31);    
    Swap(m.m23, m.m32);

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
    fDet += a.m11 * Determinant( RMatrix3( a.m22,a.m23,a.m24, 
                           a.m32,a.m33,a.m34, 
                           a.m42,a.m43,a.m44) );

    fDet -=a .m12 * Determinant( RMatrix3( a.m21,a.m23,a.m24, 
                              a.m31,a.m33,a.m34, 
                              a.m41,a.m43,a.m44 ) );

    fDet += a.m13 * Determinant( RMatrix3( a.m21,a.m22,a.m24, 
                           a.m31,a.m32,a.m34, 
                           a.m41,a.m42,a.m44) );

    fDet -= a.m14 * Determinant( RMatrix3( a.m21,a.m22,a.m23, 
                             a.m31,a.m32,a.m33, 
                             a.m41,a.m42,a.m43) );

    return fDet;
}

RMatrix4    Inverse( const RMatrix4& a )
{
    RMatrix4 m;

    // Row 1
   m.m11 = Determinant( RMatrix3(    
       a.m22,a.m23,a.m24, 
       a.m32,a.m33,a.m34, 
       a.m42,a.m43,a.m44) );

   m.m12 = -Determinant( RMatrix3(    
       a.m21,a.m23,a.m24, 
       a.m31,a.m33,a.m34, 
       a.m41,a.m43,a.m44) );

   m.m13 = Determinant( RMatrix3(    
       a.m21,a.m22,a.m24, 
       a.m31,a.m32,a.m34, 
       a.m41,a.m42,a.m44) );

   m.m14 = -Determinant( RMatrix3(    
       a.m21,a.m22,a.m23, 
       a.m31,a.m32,a.m33, 
       a.m41,a.m42,a.m43) );

    // Row 2
   m.m21 = -Determinant( RMatrix3(    
       a.m12,a.m13,a.m14, 
       a.m32,a.m33,a.m34, 
       a.m42,a.m43,a.m44) );

   m.m22 = Determinant( RMatrix3(    
       a.m11,a.m13,a.m14, 
       a.m31,a.m33,a.m34, 
       a.m41,a.m43,a.m44) );

   m.m23 = -Determinant( RMatrix3(    
       a.m11,a.m12,a.m14, 
       a.m31,a.m32,a.m34, 
       a.m41,a.m42,a.m44) );

   m.m24 = Determinant( RMatrix3(    
       a.m11,a.m12,a.m13, 
       a.m31,a.m32,a.m33, 
       a.m41,a.m42,a.m43) );

    // Row 3
   m.m31 = Determinant( RMatrix3(    
       a.m12,a.m13,a.m14, 
       a.m22,a.m23,a.m24, 
       a.m42,a.m43,a.m44) );

   m.m32 = -Determinant( RMatrix3(    
       a.m11,a.m13,a.m14, 
       a.m21,a.m23,a.m24, 
       a.m41,a.m43,a.m44) );

   m.m33 = Determinant( RMatrix3(    
       a.m11,a.m12,a.m14, 
       a.m21,a.m22,a.m24, 
       a.m41,a.m42,a.m44) );

   m.m34 = -Determinant( RMatrix3(    
       a.m11,a.m12,a.m13, 
       a.m21,a.m22,a.m23, 
       a.m41,a.m42,a.m43) );

    // Row 4
   m.m41 = -Determinant( RMatrix3(    
       a.m12,a.m13,a.m14, 
       a.m22,a.m23,a.m24, 
       a.m32,a.m33,a.m34) );

   m.m42 = Determinant( RMatrix3(    
       a.m11,a.m13,a.m14, 
       a.m21,a.m23,a.m24, 
       a.m31,a.m33,a.m34) );

   m.m43 = -Determinant( RMatrix3(    
       a.m11,a.m12,a.m14, 
       a.m21,a.m22,a.m24, 
       a.m31,a.m32,a.m34) );

   m.m44 = Determinant( RMatrix3(    
       a.m11,a.m12,a.m13, 
       a.m21,a.m22,a.m23, 
       a.m31,a.m32,a.m33) );

    m = Transpose( m );
    float fRecip = 1.0f/Determinant( a );
    m *= fRecip;

    return m;
}

RMatrix4    Transpose( const RMatrix4& a )
{
    RMatrix4 m(a);
        
    Swap(m.m12,m.m21);
    Swap(m.m13,m.m31);
    Swap(m.m14,m.m41);
    Swap(m.m23,m.m32);
    Swap(m.m24,m.m42);
    Swap(m.m34,m.m43);

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

}
