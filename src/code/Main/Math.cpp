/*********************************************************\
File:      RiotMath.cpp
Purpose:   Math function declarations
TODO:      Should some of (most) be split into a .inl
             file for inlining?
\*********************************************************/
#include "Common.h"
#include "RiotMath.h"
#include <memory> // For memcpy

/**********************************************************\
| class RVector2
\**********************************************************/
RVector2::RVector2(float X, float Y) : x(X), y(Y) 
{ 
}

RVector2::RVector2(const RVector2& V) : x(V.x), y(V.y)
{ 
}

RVector2::RVector2(const float* F) : x(F[0]), y(F[1])
{ 
}

RVector2& RVector2::operator=(const RVector2& V)
{
    x = V.x, y = V.y;
    return *this;
}

/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RVector2 RVector2::operator+(const RVector2& V) const
{ 
    return RVector2( x + V.x, y + V.y);
}

RVector2 RVector2::operator+(float F) const 
{ 
    return RVector2( x + F, y + F); 
}

RVector2& RVector2::operator+=(const RVector2& V) 
{ 
    x += V.x, y += V.y; return *this; 
}

RVector2& RVector2::operator+=(float F) 
{ 
    x += F, y += F; return *this; 
}


// Subtract
RVector2 RVector2::operator-(const RVector2& V) const 
{ 
    return RVector2( x - V.x, y - V.y); 
}

RVector2 RVector2::operator-(float F) const 
{ 
    return RVector2( x - F, y - F); 
}

RVector2& RVector2::operator-=(const RVector2& V) 
{ 
    x -= V.x, y -= V.y; return *this; 
}

RVector2& RVector2::operator-=(float F) 
{ 
    x -= F, y -= F; return *this; 
}


// Multiply and divide
RVector2 RVector2::operator*(float F) const 
{ 
    return RVector2( x * F, y * F); 
}

RVector2& RVector2::operator*=(float F) 
{ 
    x *= F, y *= F; return *this; 
}

RVector2 RVector2::operator/(float F) const 
{ 
    return RVector2( x / F, y / F); 
}

RVector2& RVector2::operator/=(float F) 
{ 
    x /= F, y /= F; return *this; 
}


// Comparison
bool RVector2::operator==(const RVector2& V) const 
{ 
    return (x == V.x && y == V.y); 
}

bool RVector2::operator!=(const RVector2& V) const 
{ 
    return !(x == V.x && y == V.y); 
}


// Vector operations

// Vector-Vector operations
float RVector2::DotProduct(const RVector2& V) const
{
    return x * V.x + y * V.y;
}

// Misc operations
float RVector2::Magnitude(void) const
{
    return sqrtf(MagnitudeSquared());
}

float RVector2::MagnitudeSquared(void) const
{
    return x*x + y*y;
}

float RVector2::Distance(const RVector2& V) const
{
    return sqrtf(DistanceSquared(V));
}

float RVector2::DistanceSquared(const RVector2& V) const
{
    return RVector2(x - V.x, y - V.y).MagnitudeSquared();
}

void RVector2::Normalize(void)
{
    *this /= Magnitude();
}

void RVector2::Zero(void)
{
    x = 0.0f, y = 0.0f;
}

// Non-member functions
float DotProduct(const RVector2& V1, const RVector2& V2)
{
    return V1.DotProduct(V2);
}

float Distance(const RVector2& V1, const RVector2& V2)
{
    return V1.Distance(V2);
}

float DistanceSquared(const RVector2& V1, const RVector2& V2)
{
    return V1.DistanceSquared(V2);
}

RVector2 Normalize(const RVector2& V)
{
    return RVector2(V) / V.Magnitude();
}


/**********************************************************\
|**********************************************************|
| class RVector3
|**********************************************************|
\**********************************************************/
RVector3::RVector3(float X, float Y, float Z) : x(X), y(Y), z(Z)
{ 
}

RVector3::RVector3(const RVector3& V) : x(V.x), y(V.y), z(V.z)
{ 
}

RVector3::RVector3(const float* F) : x(F[0]), y(F[1]), z(F[2])
{ 
}

RVector3& RVector3::operator=(const RVector3& V)
{
    x = V.x, y = V.y, z= V.z;
    return *this;
}

/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RVector3 RVector3::operator+(const RVector3& V) const
{ 
    return RVector3( x + V.x, y + V.y, z + V.z); 
}

RVector3 RVector3::operator+(float F) const 
{ 
    return RVector3( x + F, y + F, z + F); 
}

RVector3& RVector3::operator+=(const RVector3& V) 
{ 
    x += V.x, y += V.y, z += V.z; return *this; 
}

RVector3& RVector3::operator+=(float F) 
{ 
    x += F, y += F, z += F; return *this; 
}


// Subtract
RVector3 RVector3::operator-(const RVector3& V) const
{ 
    return RVector3( x - V.x, y - V.y, z - V.z); 
}

RVector3 RVector3::operator-(float F) const 
{ 
    return RVector3( x - F, y - F, z - F); 
}

RVector3& RVector3::operator-=(const RVector3& V) 
{ 
    x -= V.x, y -= V.y, z -= V.z; return *this; 
}

RVector3& RVector3::operator-=(float F) 
{ 
    x -= F, y -= F, z -= F; return *this; 
}

// Multiply and divide
RVector3 RVector3::operator*(float F) const 
{ 
    return RVector3( x * F, y * F, z * F); 
}

RVector3& RVector3::operator*=(float F) 
{ 
    x *= F, y *= F, z *= F; return *this; 
}

RVector3 RVector3::operator/(float F) const 
{
    float recip = 1 / F;
    return RVector3( x * recip, y * recip, z * recip);
}

RVector3& RVector3::operator/=(float F) 
{ 
    float recip = 1 / F;
    x *= recip, y *= recip, z *= recip; return *this; 
}


// Comparison
bool RVector3::operator==(const RVector3& V) const 
{ 
    return (x == V.x && y == V.y && z == V.z); 
}

bool RVector3::operator!=(const RVector3& V) const 
{ 
    return !(*this == V); 
}


// Vector operations

// Vector-Vector operations
float RVector3::DotProduct(const RVector3& V) const
{
    return x * V.x + y * V.y + z * V.z;
}

RVector3 RVector3::CrossProduct(const RVector3& V) const
{
    float X = y * V.z - z * V.y;
    float Y = z * V.x - x * V.z;
    float Z = x * V.y - y * V.x;

    return RVector3(X, Y, Z);
}

// Misc operations
float RVector3::Magnitude(void) const
{
    return sqrtf(MagnitudeSquared());
}

float RVector3::MagnitudeSquared(void) const
{
    return x*x + y*y + z*z;
}

float RVector3::Distance(const RVector3& V) const
{
    return sqrtf(DistanceSquared(V));
}

float RVector3::DistanceSquared(const RVector3& V) const
{
    return RVector3(x - V.x, y - V.y, z - V.z).MagnitudeSquared();
}

void RVector3::Normalize(void)
{
    float recip = 1 / Magnitude();
    *this *= recip;
}

void RVector3::Zero(void)
{
    x = 0.0f, y = 0.0f, z = 0.0f;
}

// Non-member functions
float DotProduct(const RVector3& V1, const RVector3& V2)
{
    return V1.DotProduct(V2);
}

RVector3 CrossProduct(const RVector3& V1, const RVector3& V2)
{
    return V1.CrossProduct(V2);
}

float Distance(const RVector3& V1, const RVector3& V2)
{
    return V1.Distance(V2);
}

float DistanceSquared(const RVector3& V1, const RVector3& V2)
{
    return V1.DistanceSquared(V2);
}

RVector3 Normalize(const RVector3& V)
{
    float recip = 1 / V.Magnitude();
    return RVector3(V) * recip;
}

/**********************************************************\
|**********************************************************|
| class RVector4
|**********************************************************|
\**********************************************************/
RVector4::RVector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W)
{ 
}

RVector4::RVector4(const RVector4& V) : x(V.x), y(V.y), z(V.z), w (V.w)
{ 
}

RVector4::RVector4(float X, float Y, float Z)  : x(X), y(Y), z(Z), w(0.0f)
{
}

RVector4::RVector4(const float* F) : x(F[0]), y(F[1]), z(F[2]), w(F[3])
{ 
}

RVector4& RVector4::operator=(const RVector4& V)
{
    x = V.x, y = V.y, z= V.z, w = V.w;
    return *this;
}

/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RVector4 RVector4::operator+(const RVector4& V) const
{ 
    return RVector4( x + V.x, y + V.y, z + V.z, w + V.w);
}

RVector4 RVector4::operator+(float F) const 
{ 
    return RVector4( x + F, y + F, z + F, w + F); 
}

RVector4& RVector4::operator+=(const RVector4& V) 
{ 
    x += V.x, y += V.y, z += V.z, w += V.w; 
    return *this;
}

RVector4& RVector4::operator+=(float F) 
{
    x += F, y += F, z += F, w += F; 
    return *this; 
}


// Subtract
RVector4 RVector4::operator-(const RVector4& V) const
{ 
    return RVector4( x - V.x, y - V.y, z - V.z, w - V.w); 
}

RVector4 RVector4::operator-(float F) const 
{ 
    return RVector4( x - F, y - F, z - F, w - F); 
}

RVector4& RVector4::operator-=(const RVector4& V) 
{ 
    x -= V.x, y -= V.y, z -= V.z, w -= V.w;
    return *this;
}

RVector4& RVector4::operator-=(float F) 
{ 
    x -= F, y -= F, z -= F, w -= F; 
    return *this; 
}


// Multiply and divide
RVector4 RVector4::operator*(float F) const 
{ 
    return RVector4( x * F, y * F, z * F, w * F); 
}

RVector4& RVector4::operator*=(float F) 
{ 
    x *= F, y *= F, z *= F, w *= F; 
    return *this; 
}

RVector4 RVector4::operator/(float F) const 
{
    float recip = 1 / F;
    return RVector4( x * recip, y * recip, z * recip, w * recip);
}

RVector4& RVector4::operator/=(float F) 
{
    float recip = 1 / F;
    x *= recip, y *= recip, z *= recip, w *= recip; 
    return *this;
}


// Comparison
bool RVector4::operator==(const RVector4& V) const 
{ 
    return (x == V.x && y == V.y && z == V.z && w == V.w); 
}

bool RVector4::operator!=(const RVector4& V) const 
{ 
    return !(*this == V); 
}


// Vector operations

// Vector-Vector operations
float RVector4::DotProduct(const RVector4& V) const
{
    return x * V.x + y * V.y + z * V.z + w * V.w;
}

RVector4 RVector4::CrossProduct(const RVector4& V) const
{
    float X = y * V.z - z * V.y;
    float Y = z * V.x - x * V.z;
    float Z = x * V.y - y * V.x;

    return RVector4(X, Y, Z, 0.0f);
}


// Misc operations
float RVector4::Magnitude(void) const
{
    return sqrtf(MagnitudeSquared());
}

float RVector4::MagnitudeSquared(void) const
{
    return x*x + y*y + z*z + w*w;
}

float RVector4::Distance(const RVector4& V) const
{
    return sqrtf(DistanceSquared(V));
}

float RVector4::DistanceSquared(const RVector4& V) const
{
    return RVector4(x - V.x, y - V.y, z - V.z, w - V.w).MagnitudeSquared();
}

void RVector4::Normalize(void)
{
    float recip = 1 / Magnitude();
    *this *= recip;
}

void RVector4::Zero(void)
{
    x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
}

// Non-member functions
float DotProduct(const RVector4& V1, const RVector4& V2)
{
    return V1.DotProduct(V2);
}

RVector4 CrossProduct(const RVector4& V1, const RVector4& V2)
{
    return V1.CrossProduct(V2);
}

float Distance(const RVector4& V1, const RVector4& V2)
{
    return V1.Distance(V2);
}

float DistanceSquared(const RVector4& V1, const RVector4& V2)
{
    return V1.DistanceSquared(V2);
}

RVector4 Normalize(const RVector4& V)
{
    float recip = 1 / V.Magnitude();
    return RVector4(V) * recip;
}

RVector4 RVector4Zero() 
{ 
    return RVector4(0.0f, 0.0f, 0.0f, 0.0f); 
}

/**********************************************************\
|**********************************************************|
| class RMatrix2
|**********************************************************|
\**********************************************************/
RMatrix2::RMatrix2( float M11, float M12, float M21, float M22 ) : m11(M11), m12(M12), m21(M21), m22(M22)
{
}

RMatrix2::RMatrix2(const RMatrix2& M) : m11(M.m11), m12(M.m12), m21(M.m21), m22(M.m22)
{
}

RMatrix2::RMatrix2(const float* F)
{
    memcpy(f, F, sizeof(RMatrix2));
}

RMatrix2& RMatrix2::operator=(const RMatrix2& M)
{
    memcpy(f, M.f, sizeof(RMatrix2));
    return *this;
}


/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RMatrix2 RMatrix2::operator+(const RMatrix2& M) const
{
    return RMatrix2( m11 + M.m11, m12 + M.m12, m21 + M.m21, m22 + M.m22); 
}

RMatrix2 RMatrix2::operator+(float F) const
{
    return RMatrix2( m11 + F, m12 + F, m21 + F, m22 + F); 
}

RMatrix2& RMatrix2::operator+=(const RMatrix2& M)
{
    m11 += M.m11, m12 += M.m12, m21 += M.m21, m22 += M.m22;
    return *this;
}

RMatrix2& RMatrix2::operator+=(float F)
{
    m11 += F, m12 += F, m21 += F, m22 += F;
    return *this;
}


// Subtract
RMatrix2 RMatrix2::operator-(const RMatrix2& M) const
{
    return RMatrix2( m11 - M.m11, m12 - M.m12, m21 - M.m21, m22 - M.m22); 
}

RMatrix2 RMatrix2::operator-(float F) const
{
    return RMatrix2( m11 - F, m12 - F, m21 - F, m22 - F); 
}

RMatrix2& RMatrix2::operator-=(const RMatrix2& M)
{
    m11 -= M.m11, m12 -= M.m12, m21 -= M.m21, m22 -= M.m22;
    return *this;
}

RMatrix2& RMatrix2::operator-=(float F)
{
    m11 -= F, m12 -= F, m21 -= F, m22 -= F;
    return *this;
}


// Multiply and divide
RMatrix2 RMatrix2::operator*(float F) const
{
    return RMatrix2( m11 * F, m12 * F, m21 * F, m22 * F); 
}

RMatrix2& RMatrix2::operator*=(float F)
{
    m11 *= F, m12 *= F, m21 *= F, m22 *= F;
    return *this;
}

RMatrix2 RMatrix2::operator/(float F) const
{
    return RMatrix2( m11 / F, m12 / F, m21 / F, m22 / F); 
}

RMatrix2& RMatrix2::operator/=(float F)
{
    m11 /= F, m12 /= F, m21 /= F, m22 /= F;
    return *this;
}


// Comparison
bool RMatrix2::operator==(const RMatrix2& M) const
{
    for(int i = 1; i < 4; ++i)
    {
        if(f[i] != M.f[i])
            return false;
    }

    return true;
}

bool RMatrix2::operator!=(const RMatrix2& M) const
{
    return !(*this == M);
}


// Matrix operations

RMatrix2 RMatrix2::operator*(const RMatrix2& M) const
{
    RMatrix2 m;
#ifndef RIOT_USE_INTRINSICS
    m.m11 = m11 * M.m11 + m12 * M.m21;
    m.m12 = m11 * M.m12 + m12 * M.m22;
    m.m21 = m21 * M.m11 + m22 * M.m21;
    m.m22 = m21 * M.m12 + m22 * M.m22;
#else
    __m128 a = _mm_set_ps(m11, m11, m21, m21);
    __m128 b = _mm_set_ps(M.m11, M.m12, M.m11, M.m12);
    __m128 c = _mm_set_ps(m12, m12, m22, m22);
    __m128 d = _mm_set_ps(M.m21, M.m22, M.m21, M.m22);

    _mm_store_ps(m.f, _mm_add_ps( _mm_mul_ps( a, b), _mm_mul_ps(c, d) ) );
#endif

    return m;
}

RMatrix2& RMatrix2::operator*=(const RMatrix2& M)
{
#ifndef RIOT_USE_INTRINSICS
    m11 = m11 * M.m11 + m12 * M.m21;
    m12 = m11 * M.m12 + m12 * M.m22;
    m21 = m21 * M.m11 + m22 * M.m21;
    m22 = m21 * M.m12 + m22 * M.m22;
#else
    __m128 a = _mm_set_ps(m11, m11, m21, m21);
    __m128 b = _mm_set_ps(M.m11, M.m12, M.m11, M.m12);
    __m128 c = _mm_set_ps(m12, m12, m22, m22);
    __m128 d = _mm_set_ps(M.m21, M.m22, M.m21, M.m22);

    _mm_store_ps(f, _mm_add_ps( _mm_mul_ps( a, b), _mm_mul_ps(c, d) ) );
#endif
    return *this;
}

RVector2 RMatrix2::operator*(const RVector2& V) const
{
    RVector2 v;
    v.x = m11 * V.x + m12 * V.y;
    v.y = m21 * V.x + m22 * V.y;

    return v;
}


// Misc matrix operations
void RMatrix2::Identity()
{
    m12 = m21 = 0.0f;
    m11 = m22 = 1.0f;
}

void RMatrix2::Transpose()
{
    float t;
    Swap(m12, m21);
}

float RMatrix2::Determinant()
{
    return (m11 * m22) - (m12 * m21);
}

void RMatrix2::Inverse()
{
    float t;
    Swap(m11, m22);
    m12 = -m12;
    m21 = -m21;
    *this *= 1/Determinant();
}

/**********************************************************\
|**********************************************************|
| class RMatrix3
|**********************************************************|
\**********************************************************/
RMatrix3::RMatrix3(    float M11, float M12, float M13, 
                    float M21, float M22, float M23,
                    float M31, float M32, float M33) 
                    : m11(M11), m12(M12), m13(M13),
                      m21(M21), m22(M22), m23(M23),
                      m31(M31), m32(M32), m33(M33)
{
}

RMatrix3::RMatrix3(const RMatrix3& M)
{
    memcpy(f, M.f, sizeof(RMatrix3));
}

RMatrix3::RMatrix3(const float* F)
{
    memcpy(f, F, sizeof(RMatrix3));
}

RMatrix3& RMatrix3::operator=(const RMatrix3& M)
{
    memcpy(f, M.f, sizeof(RMatrix3));
    return *this;
}


/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RMatrix3 RMatrix3::operator+(const RMatrix3& M) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix3( m11 + M.m11, m12 + M.m12, m13 + M.m13,
                    m21 + M.m21, m22 + M.m22, m23 + M.m23, 
                    m31 + M.m31, m32 + M.m32, m33 + M.m33 );
#else
    RMatrix3 m;

    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_load_ps(M.f);
    _mm_store_ps(m.f, _mm_add_ps(a, b));

    a = _mm_load_ps(f + 4);
    b = _mm_load_ps(M.f + 4);
    _mm_store_ps(m.f+4, _mm_add_ps(a, b));

    m.m33 = m33 + M.m33;

    return m;
#endif
}

RMatrix3 RMatrix3::operator+(float F) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix3( m11 + F, m12 + F, m13 + F,
                    m21 + F, m22 + F, m23 + F,
                    m31 + F, m32 + F, m33 + F ); 
#else
    RMatrix3 m;

    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_set1_ps(F);
    _mm_store_ps(m.f, _mm_add_ps(a, b));

    a = _mm_load_ps(f + 4);
    _mm_store_ps(m.f+4, _mm_add_ps(a, b));

    m.m33 = m33 + F;

    return m;
#endif
}

RMatrix3& RMatrix3::operator+=(const RMatrix3& M)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 += M.m11, m12 += M.m12, m13 += M.m13;
    m21 += M.m21, m22 += M.m22, m23 += M.m23;
    m31 += M.m31, m32 += M.m32, m33 += M.m33;
    return *this;
#else
    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_load_ps(M.f);
    _mm_store_ps(f, _mm_add_ps(a, b));

    a = _mm_load_ps(f + 4);
    b = _mm_load_ps(M.f + 4);
    _mm_store_ps(f+4, _mm_add_ps(a, b));

    m33 += M.m33;

    return *this;
#endif
}

RMatrix3& RMatrix3::operator+=(float F)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 += F, m12 += F, m13 += F;
    m21 += F, m22 += F, m23 += F;
    m31 += F, m32 += F, m33 += F;
    return *this;
#else
    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_set1_ps(F);
    _mm_store_ps(f, _mm_add_ps(a, b));

    a = _mm_load_ps(f+4);
    _mm_store_ps(f+4, _mm_add_ps(a, b));

    m33 += F;

    return *this;
#endif
}


// sub
RMatrix3 RMatrix3::operator-(const RMatrix3& M) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix3( m11 - M.m11, m12 - M.m12, m13 - M.m13,
                    m21 - M.m21, m22 - M.m22, m23 - M.m23, 
                    m31 - M.m31, m32 - M.m32, m33 - M.m33 );
#else
    RMatrix3 m;

    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_load_ps(M.f);
    _mm_store_ps(m.f, _mm_sub_ps(a, b));

    a = _mm_load_ps(f+4);
    b = _mm_load_ps(M.f+4);
    _mm_store_ps(m.f+4, _mm_sub_ps(a, b));

    m.m33 = m33 - M.m33;

    return m;
#endif
}

RMatrix3 RMatrix3::operator-(float F) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix3( m11 - F, m12 - F, m13 - F,
                    m21 - F, m22 - F, m23 - F,
                    m31 - F, m32 - F, m33 - F ); 
#else
    RMatrix3 m;

    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_set1_ps(F);
    _mm_store_ps(m.f, _mm_sub_ps(a, b));

    a = _mm_load_ps(f+4);
    _mm_store_ps(m.f+4, _mm_sub_ps(a, b));

    m.m33 = m33 - F;

    return m;
#endif
}

RMatrix3& RMatrix3::operator-=(const RMatrix3& M)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 -= M.m11, m12 -= M.m12, m13 -= M.m13;
    m21 -= M.m21, m22 -= M.m22, m23 -= M.m23;
    m31 -= M.m31, m32 -= M.m32, m33 -= M.m33;
    return *this;
#else
    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_load_ps(M.f);
    _mm_store_ps(f, _mm_sub_ps(a, b));

    a = _mm_load_ps(f+4);
    b = _mm_load_ps(M.f+4);
    _mm_store_ps(f+4, _mm_sub_ps(a, b));

    m33 -= M.m33;

    return *this;
#endif
}

RMatrix3& RMatrix3::operator-=(float F)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 -= F, m12 -= F, m13 -= F;
    m21 -= F, m22 -= F, m23 -= F;
    m31 -= F, m32 -= F, m33 -= F;
    return *this;
#else
    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_set1_ps(F);
    _mm_store_ps(f, _mm_sub_ps(a, b));

    a = _mm_load_ps(f+4);
    _mm_store_ps(f+4, _mm_sub_ps(a, b));

    m33 -= F;

    return *this;
#endif
}


// Multiply and divide
RMatrix3 RMatrix3::operator*(float F) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix3( m11 * F, m12 * F, m13 * F,
                    m21 * F, m22 * F, m23 * F,
                    m31 * F, m32 * F, m33 * F ); 
#else
    RMatrix3 m;

    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_set1_ps(F);
    _mm_store_ps(m.f, _mm_mul_ps(a, b));

    a = _mm_load_ps(f+4);
    _mm_store_ps(m.f+4, _mm_mul_ps(a, b));

    m.m33 = m33 * F;

    return m;
#endif
}

RMatrix3& RMatrix3::operator*=(float F)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 *= F, m12 *= F, m13 *= F;
    m21 *= F, m22 *= F, m23 *= F;
    m31 *= F, m32 *= F, m33 *= F;
    return *this;
#else
    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_set1_ps(F);
    _mm_store_ps(f, _mm_mul_ps(a, b));

    a = _mm_load_ps(f - 4);
    _mm_store_ps(f-4, _mm_mul_ps(a, b));

    m33 *= F;

    return *this;
#endif
}

RMatrix3 RMatrix3::operator/(float F) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix3( m11 / F, m12 / F, m13 / F,
                    m21 / F, m22 / F, m23 / F,
                    m31 / F, m32 / F, m33 / F ); 
#else
    RMatrix3 m;

    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_set1_ps(F);
    _mm_store_ps(m.f, _mm_div_ps(a, b));

    a = _mm_load_ps(f - 4);
    _mm_store_ps(m.f-4, _mm_div_ps(a, b));

    m.m33 = m33 / F;

    return m;
#endif
}

RMatrix3& RMatrix3::operator/=(float F)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 /= F, m12 /= F, m13 /= F;
    m21 /= F, m22 /= F, m23 /= F;
    m31 /= F, m32 /= F, m33 /= F;
    return *this;
#else
    __m128 a = _mm_load_ps(f);
    __m128 b = _mm_set1_ps(F);
    _mm_store_ps(f, _mm_div_ps(a, b));

    a = _mm_load_ps(f - 4);
    _mm_store_ps(f-4, _mm_div_ps(a, b));

    m33 /= F;

    return *this;
#endif
}

// Comparison
bool RMatrix3::operator==(const RMatrix3& M) const
{
    for(int i = 1; i < 9; ++i)
    {
        if(f[i] != M.f[i])
            return false;
    }

    return true;
}

bool RMatrix3::operator!=(const RMatrix3& M) const
{
    return !(*this == M);
}


// Matrix operations

RMatrix3 RMatrix3::operator*(const RMatrix3& M) const
{
    RMatrix3 m;
#ifndef RIOT_USE_INTRINSICS
    m.m11 = m11 * M.m11 + m12 * M.m21 + m13 * M.m31;
    m.m12 = m11 * M.m12 + m12 * M.m22 + m13 * M.m32;
    m.m13 = m11 * M.m13 + m12 * M.m23 + m13 * M.m33;

    m.m21 = m21 * M.m11 + m22 * M.m21 + m23 * M.m31;
    m.m22 = m21 * M.m12 + m22 * M.m22 + m23 * M.m32;
    m.m23 = m21 * M.m13 + m22 * M.m23 + m23 * M.m33;

    m.m31 = m31 * M.m11 + m32 * M.m21 + m33 * M.m31;
    m.m32 = m31 * M.m12 + m32 * M.m22 + m33 * M.m32;
    m.m33 = m31 * M.m13 + m32 * M.m23 + m33 * M.m33;
#else
    // Try 3 calculations
    // Need to use set because a 3x3 matrix isn't properly aligned
    __m128 Mrow1 = _mm_set_ps(M.m11, M.m12, M.m13, 0.0f);
    __m128 Mrow2 = _mm_set_ps(M.m21, M.m22, M.m23, 0.0f);
    __m128 Mrow3 = _mm_set_ps(M.m31, M.m32, M.m33, 0.0f);

    __m128 col1 = _mm_load1_ps(&m11);
    __m128 col2 = _mm_load1_ps(&m12);
    __m128 col3 = _mm_load1_ps(&m13);

    col1 = _mm_mul_ps(col1, Mrow1);
    col2 = _mm_mul_ps(col2, Mrow2);
    col3 = _mm_mul_ps(col3, Mrow3);

    __m128 res = _mm_add_ps( _mm_add_ps( col1, col2), col3);
    m.m11 = res.m128_f32[3];
    m.m12 = res.m128_f32[2];
    m.m13 = res.m128_f32[1];

    col1 = _mm_load1_ps(&m21);
    col2 = _mm_load1_ps(&m22);
    col3 = _mm_load1_ps(&m23);

    col1 = _mm_mul_ps(col1, Mrow1);
    col2 = _mm_mul_ps(col2, Mrow2);
    col3 = _mm_mul_ps(col3, Mrow3);

    res = _mm_add_ps( _mm_add_ps( col1, col2), col3);
    m.m21 = res.m128_f32[3];
    m.m22 = res.m128_f32[2];
    m.m23 = res.m128_f32[1];

    col1 = _mm_load1_ps(&m31);
    col2 = _mm_load1_ps(&m32);
    col3 = _mm_load1_ps(&m33);

    col1 = _mm_mul_ps(col1, Mrow1);
    col2 = _mm_mul_ps(col2, Mrow2);
    col3 = _mm_mul_ps(col3, Mrow3);

    res = _mm_add_ps( _mm_add_ps( col1, col2), col3);
    m.m31 = res.m128_f32[3];
    m.m32 = res.m128_f32[2];
    m.m33 = res.m128_f32[1];
#endif

    return m;
}

RMatrix3& RMatrix3::operator*=(const RMatrix3& M)
{
#ifndef RIOT_USE_INTRINSICS
    m11 = m11 * M.m11 + m12 * M.m21 + m13 * M.m31;
    m12 = m11 * M.m12 + m12 * M.m22 + m13 * M.m32;
    m13 = m11 * M.m13 + m12 * M.m23 + m13 * M.m33;

    m21 = m21 * M.m11 + m22 * M.m21 + m23 * M.m31;
    m22 = m21 * M.m12 + m22 * M.m22 + m23 * M.m32;
    m23 = m21 * M.m13 + m22 * M.m23 + m23 * M.m33;

    m31 = m31 * M.m11 + m32 * M.m31 + m33 * M.m31;
    m32 = m31 * M.m12 + m32 * M.m32 + m33 * M.m32;
    m33 = m31 * M.m13 + m32 * M.m33 + m33 * M.m33;
#else
    // Try 3 calculations
    // Need to use set because a 3x3 matrix isn't properly aligned
    __m128 Mrow1 = _mm_set_ps(M.m11, M.m12, M.m13, 0.0f);
    __m128 Mrow2 = _mm_set_ps(M.m21, M.m22, M.m23, 0.0f);
    __m128 Mrow3 = _mm_set_ps(M.m31, M.m32, M.m33, 0.0f);

    __m128 col1 = _mm_load1_ps(&m11);
    __m128 col2 = _mm_load1_ps(&m12);
    __m128 col3 = _mm_load1_ps(&m13);

    col1 = _mm_mul_ps(col1, Mrow1);
    col2 = _mm_mul_ps(col2, Mrow2);
    col3 = _mm_mul_ps(col3, Mrow3);

    __m128 res = _mm_add_ps( _mm_add_ps( col1, col2), col3);
    m11 = res.m128_f32[3];
    m12 = res.m128_f32[2];
    m13 = res.m128_f32[1];

    col1 = _mm_load1_ps(&m21);
    col2 = _mm_load1_ps(&m22);
    col3 = _mm_load1_ps(&m23);

    col1 = _mm_mul_ps(col1, Mrow1);
    col2 = _mm_mul_ps(col2, Mrow2);
    col3 = _mm_mul_ps(col3, Mrow3);

    res = _mm_add_ps( _mm_add_ps( col1, col2), col3);
    m21 = res.m128_f32[3];
    m22 = res.m128_f32[2];
    m23 = res.m128_f32[1];

    col1 = _mm_load1_ps(&m31);
    col2 = _mm_load1_ps(&m32);
    col3 = _mm_load1_ps(&m33);

    col1 = _mm_mul_ps(col1, Mrow1);
    col2 = _mm_mul_ps(col2, Mrow2);
    col3 = _mm_mul_ps(col3, Mrow3);

    res = _mm_add_ps( _mm_add_ps( col1, col2), col3);
    m31 = res.m128_f32[3];
    m32 = res.m128_f32[2];
    m33 = res.m128_f32[1];
#endif
    return *this;
}

RVector3 RMatrix3::operator*(const RVector3& V) const
{
    RVector3 v;
#ifndef RIOT_USE_INTRINSICS
    v.x = m11 * V.x + m12 * V.y + m13 * V.z;
    v.y = m21 * V.x + m22 * V.y + m23 * V.z;
    v.z = m31 * V.x + m32 * V.y + m33 * V.z;
#else
    v.x = m11 * V.x + m12 * V.y + m13 * V.z;
    v.y = m21 * V.x + m22 * V.y + m23 * V.z;
    v.z = m31 * V.x + m32 * V.y + m33 * V.z;
#endif
    return v;
}


// Misc matrix operations
void RMatrix3::Identity()
{
    memset(f, 0, sizeof(RMatrix3));
    m11 = m22 = m33 = 1.0f;
}

void RMatrix3::Transpose()
{
    float t;
    Swap(m12, m21);
    Swap(m13, m31);
    
    Swap(m23, m32);
}

float RMatrix3::Determinant()
{
#ifndef RIOT_USE_INTRINSICS 
    float det1 = ( (m11*m22*m33) + (m12*m23*m31) + (m13*m21*m32) );
    float det2 = ( (m13*m22*m31) + (m12*m21*m33) + (m11*m23*m32) );
    return det1 - det2;
#else
    float det1 = ( (m11*m22*m33) + (m12*m23*m31) + (m13*m21*m32) );
    float det2 = ( (m13*m22*m31) + (m12*m21*m33) + (m11*m23*m32) );
    return det1 - det2;
#endif
}

void RMatrix3::Inverse()
{
    RMatrix3 m = *this;
#ifndef RIOT_USE_INTRINSICS 
    m.m11 = (m22*m33) - (m23*m32);
    m.m12 = -((m21*m33) - (m23*m31));
    m.m13 = (m21*m32) - (m22*m31);

    m.m21 = -((m12*m33) - (m13*m32));
    m.m22 = (m11*m33) - (m13*m31);
    m.m23 = -((m11*m32) - (m12*m31));

    m.m31 = (m12*m23) - (m13*m22);
    m.m32 = -((m11*m23) - (m13*m21));
    m.m33 = (m11*m22) - (m12*m21);
#else
    __m128 a, b, c, d;
    a = _mm_set_ps(m22, m21, m21, m12);
    b = _mm_set_ps(m33, m33, m32, m33);
    c = _mm_set_ps(m23, m23, m22, m13);
    d = _mm_set_ps(m32, m31, m31, m32);

    a = _mm_mul_ps(a, b);
    c = _mm_mul_ps(c, d);

    __m128 res = _mm_sub_ps(a, c);

    m.m11 = res.m128_f32[3];
    m.m12 = -res.m128_f32[2];
    m.m13 = res.m128_f32[1];
    m.m21 = -res.m128_f32[0];

    a = _mm_set_ps(m11, m11, m12, m11);
    b = _mm_set_ps(m33, m32, m23, m23);
    c = _mm_set_ps(m13, m12, m13, m13);
    d = _mm_set_ps(m31, m31, m22, m21);

    a = _mm_mul_ps(a, b);
    c = _mm_mul_ps(c, d);

    res = _mm_sub_ps(a, c);

    m.m22 = res.m128_f32[3];
    m.m23 = -res.m128_f32[2];
    m.m31 = res.m128_f32[1];
    m.m32 = -res.m128_f32[0];

    m.m33 = (m11*m22) - (m12*m21);
#endif
    m.Transpose();
    float recip = 1/Determinant();

    *this = m * recip;
}


/**********************************************************\
|**********************************************************|
| class RMatrix4
|**********************************************************|
\**********************************************************/
RMatrix4::RMatrix4(   float M11, float M12, float M13, float M14, 
                    float M21, float M22, float M23, float M24,
                    float M31, float M32, float M33, float M34, 
                    float M41, float M42, float M43, float M44)
                    : m11(M11), m12(M12), m13(M13), m14(M14)
                    , m21(M21), m22(M22), m23(M23), m24(M24)
                    , m31(M31), m32(M32), m33(M33), m34(M34)
                    , m41(M41), m42(M42), m43(M43), m44(M44)
{
}

RMatrix4::RMatrix4(const RMatrix4& M)
{
    memcpy(f, M.f, sizeof(RMatrix4));
}

RMatrix4::RMatrix4(const float* F)
{
    memcpy(f, F, sizeof(RMatrix4));
}

RMatrix4& RMatrix4::operator=(const RMatrix4& M)
{
    memcpy(f, M.f, sizeof(RMatrix4));
    return *this;
}


/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RMatrix4 RMatrix4::operator+(const RMatrix4& M) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix4( m11 + M.m11, m12 + M.m12, m13 + M.m13, m14 + M.m14,
                    m21 + M.m21, m22 + M.m22, m23 + M.m23, m24 + M.m24, 
                    m31 + M.m31, m32 + M.m32, m33 + M.m33, m34 + M.m34,
                    m41 + M.m41, m42 + M.m42, m43 + M.m43, m44 + M.m44 );
#else
    RMatrix4 m;

    m.r1 = _mm_add_ps(r1, M.r1);
    m.r2 = _mm_add_ps(r2, M.r2);
    m.r3 = _mm_add_ps(r3, M.r3);
    m.r4 = _mm_add_ps(r4, M.r4);

    return m;
#endif
}

RMatrix4 RMatrix4::operator+(float F) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix4( m11 + F, m12 + F, m13 + F, m14 + F,
                    m21 + F, m22 + F, m23 + F, m24 + F,
                    m31 + F, m32 + F, m33 + F, m34 + F,
                    m41 + F, m42 + F, m43 + F, m44 + F); 
#else
    RMatrix4 m;

    __m128 f = _mm_load1_ps(&F);
    m.r1 = _mm_add_ps(r1, f);
    m.r2 = _mm_add_ps(r2, f);
    m.r3 = _mm_add_ps(r3, f);
    m.r4 = _mm_add_ps(r4, f);

    return m;
#endif
}

RMatrix4& RMatrix4::operator+=(const RMatrix4& M)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 += M.m11, m12 += M.m12, m13 += M.m13, m14 + M.m14;
    m21 += M.m21, m22 += M.m22, m23 += M.m23, m24 + M.m24;
    m31 += M.m31, m32 += M.m32, m33 += M.m33, m34 + M.m34;
    m31 += M.m31, m32 += M.m32, m33 += M.m33, m34 + M.m34;
#else
    r1 = _mm_add_ps(r1, M.r1);
    r2 = _mm_add_ps(r2, M.r2);
    r3 = _mm_add_ps(r3, M.r3);
    r4 = _mm_add_ps(r4, M.r4);
#endif
    return *this;
}

RMatrix4& RMatrix4::operator+=(float F)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 += F, m12 += F, m13 += F, m14 + F;
    m21 += F, m22 += F, m23 += F, m24 + F;
    m31 += F, m32 += F, m33 += F, m34 + F;
    m41 += F, m42 += F, m43 += F, m44 + F;
    return *this;
#else
    __m128 b = _mm_set1_ps(F);

    r1 = _mm_add_ps(r1, b);
    r2 = _mm_add_ps(r2, b);
    r3 = _mm_add_ps(r3, b);
    r4 = _mm_add_ps(r4, b);

    return *this;
#endif
}


// sub
RMatrix4 RMatrix4::operator-(const RMatrix4& M) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix4( m11 - M.m11, m12 - M.m12, m13 - M.m13, m14 - M.m14,
                    m21 - M.m21, m22 - M.m22, m23 - M.m23, m24 - M.m24, 
                    m31 - M.m31, m32 - M.m32, m33 - M.m33, m34 - M.m34,
                    m41 - M.m41, m42 - M.m42, m43 - M.m43, m44 - M.m44 );
#else
    RMatrix4 m;

    m.r1 = _mm_sub_ps(r1, M.r1);
    m.r2 = _mm_sub_ps(r2, M.r2);
    m.r3 = _mm_sub_ps(r3, M.r3);
    m.r4 = _mm_sub_ps(r4, M.r4);

    return m;
#endif
}

RMatrix4 RMatrix4::operator-(float F) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix4( m11 - F, m12 - F, m13 - F, m14 - F,
                    m21 - F, m22 - F, m23 - F, m24 - F,
                    m31 - F, m32 - F, m33 - F, m34 - F,
                    m41 - F, m42 - F, m43 - F, m44 - F); 
#else
    RMatrix4 m;

    __m128 f = _mm_load1_ps(&F);
    m.r1 = _mm_sub_ps(r1, f);
    m.r2 = _mm_sub_ps(r2, f);
    m.r3 = _mm_sub_ps(r3, f);
    m.r4 = _mm_sub_ps(r4, f);

    return m;
#endif
}

RMatrix4& RMatrix4::operator-=(const RMatrix4& M)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 -= M.m11, m12 -= M.m12, m13 -= M.m13, m14 - M.m14;
    m21 -= M.m21, m22 -= M.m22, m23 -= M.m23, m24 - M.m24;
    m31 -= M.m31, m32 -= M.m32, m33 -= M.m33, m34 - M.m34;
    m31 -= M.m31, m32 -= M.m32, m33 -= M.m33, m34 - M.m34;
    return *this;
#else
    r1 = _mm_sub_ps(r1, M.r1);
    r2 = _mm_sub_ps(r2, M.r2);
    r3 = _mm_sub_ps(r3, M.r3);
    r4 = _mm_sub_ps(r4, M.r4);

    return *this;
#endif
}

RMatrix4& RMatrix4::operator-=(float F)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 -= F, m12 -= F, m13 -= F, m14 - F;
    m21 -= F, m22 -= F, m23 -= F, m24 - F;
    m31 -= F, m32 -= F, m33 -= F, m34 - F;
    m41 -= F, m42 -= F, m43 -= F, m44 - F;
    return *this;
#else
    __m128 f = _mm_load1_ps(&F);
    r1 = _mm_sub_ps(r1, f);
    r2 = _mm_sub_ps(r2, f);
    r3 = _mm_sub_ps(r3, f);
    r4 = _mm_sub_ps(r4, f);

    return *this;
#endif
}


// Multiply and divide
RMatrix4 RMatrix4::operator*(float F) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix4( m11 * F, m12 * F, m13 * F, m14 * F,
                    m21 * F, m22 * F, m23 * F, m24 * F,
                    m31 * F, m32 * F, m33 * F, m34 * F,
                    m41 * F, m42 * F, m43 * F, m44 * F); 
#else
    RMatrix4 m;

    __m128 f = _mm_load1_ps(&F);
    m.r1 = _mm_mul_ps(r1, f);
    m.r2 = _mm_mul_ps(r2, f);
    m.r3 = _mm_mul_ps(r3, f);
    m.r4 = _mm_mul_ps(r4, f);

    return m;
#endif
}

RMatrix4& RMatrix4::operator*=(float F)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 *= F, m12 *= F, m13 *= F, m14 * F;
    m21 *= F, m22 *= F, m23 *= F, m24 * F;
    m31 *= F, m32 *= F, m33 *= F, m34 * F;
    m41 *= F, m42 *= F, m43 *= F, m44 * F;
    return *this;
#else
    __m128 f = _mm_load1_ps(&F);
    r1 = _mm_mul_ps(r1, f);
    r2 = _mm_mul_ps(r2, f);
    r3 = _mm_mul_ps(r3, f);
    r4 = _mm_mul_ps(r4, f);

    return *this;
#endif
}

RMatrix4 RMatrix4::operator/(float F) const
{
#ifndef RIOT_USE_INTRINSICS 
    return RMatrix4( m11 / F, m12 / F, m13 / F, m14 / F,
                    m21 / F, m22 / F, m23 / F, m24 / F,
                    m31 / F, m32 / F, m33 / F, m34 / F,
                    m41 / F, m42 / F, m43 / F, m44 / F); 
#else
    RMatrix4 m;

    __m128 f = _mm_load1_ps(&F);
    m.r1 = _mm_div_ps(r1, f);
    m.r2 = _mm_div_ps(r2, f);
    m.r3 = _mm_div_ps(r3, f);
    m.r4 = _mm_div_ps(r4, f);

    return m;
#endif
}

RMatrix4& RMatrix4::operator/=(float F)
{
#ifndef RIOT_USE_INTRINSICS 
    m11 /= F, m12 /= F, m13 /= F, m14 / F;
    m21 /= F, m22 /= F, m23 /= F, m24 / F;
    m31 /= F, m32 /= F, m33 /= F, m34 / F;
    m41 /= F, m42 /= F, m43 /= F, m44 / F;
    return *this;
#else
    __m128 f = _mm_load1_ps(&F);
    r1 = _mm_div_ps(r1, f);
    r2 = _mm_div_ps(r2, f);
    r3 = _mm_div_ps(r3, f);
    r4 = _mm_div_ps(r4, f);

    return *this;
#endif
}

// Comparison
bool RMatrix4::operator==(const RMatrix4& M) const
{
    for(int i = 1; i < 16; ++i)
    {
        if(f[i] != M.f[i])
            return false;
    }

    return true;
}

bool RMatrix4::operator!=(const RMatrix4& M) const
{
    return !(*this == M);
}


// Matrix operations
RMatrix4 RMatrix4::operator*(const RMatrix4& M) const
{
    RMatrix4 m;
#ifndef RIOT_USE_INTRINSICS
    m.m11 = (m11 * M.m11) + (m12 * M.m21) + (m13 * M.m31) + (m14 * M.m41);
    m.m12 = (m11 * M.m12) + (m12 * M.m22) + (m13 * M.m32) + (m14 * M.m42);
    m.m13 = (m11 * M.m13) + (m12 * M.m23) + (m13 * M.m33) + (m14 * M.m43);
    m.m14 = (m11 * M.m14) + (m12 * M.m24) + (m13 * M.m34) + (m14 * M.m44);
    
    m.m21 = (m21 * M.m11) + (m22 * M.m21) + (m23 * M.m31) + (m24 * M.m41);
    m.m22 = (m21 * M.m12) + (m22 * M.m22) + (m23 * M.m32) + (m24 * M.m42);
    m.m23 = (m21 * M.m13) + (m22 * M.m23) + (m23 * M.m33) + (m24 * M.m43);
    m.m24 = (m21 * M.m14) + (m22 * M.m24) + (m23 * M.m34) + (m24 * M.m44);
    
    m.m31 = (m31 * M.m11) + (m32 * M.m21) + (m33 * M.m31) + (m34 * M.m41);
    m.m32 = (m31 * M.m12) + (m32 * M.m22) + (m33 * M.m32) + (m34 * M.m42);
    m.m33 = (m31 * M.m13) + (m32 * M.m23) + (m33 * M.m33) + (m34 * M.m43);
    m.m34 = (m31 * M.m14) + (m32 * M.m24) + (m33 * M.m34) + (m34 * M.m44);
    
    m.m41 = (m41 * M.m11) + (m42 * M.m21) + (m43 * M.m31) + (m44 * M.m41);
    m.m42 = (m41 * M.m12) + (m42 * M.m22) + (m43 * M.m32) + (m44 * M.m42);
    m.m43 = (m41 * M.m13) + (m42 * M.m23) + (m43 * M.m33) + (m44 * M.m43);
    m.m44 = (m41 * M.m14) + (m42 * M.m24) + (m43 * M.m34) + (m44 * M.m44);

#else
    m.r1 = _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&m11), M.r1), _mm_mul_ps( _mm_load1_ps(&m12), M.r2 ), _mm_mul_ps( _mm_load1_ps(&m13), M.r3), _mm_mul_ps( _mm_load1_ps(&m14), M.r4 ) );
    m.r2 = _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&m21), M.r1), _mm_mul_ps( _mm_load1_ps(&m22), M.r2 ), _mm_mul_ps( _mm_load1_ps(&m23), M.r3), _mm_mul_ps( _mm_load1_ps(&m24), M.r4 ) );
    m.r3 = _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&m31), M.r1), _mm_mul_ps( _mm_load1_ps(&m32), M.r2 ), _mm_mul_ps( _mm_load1_ps(&m33), M.r3), _mm_mul_ps( _mm_load1_ps(&m34), M.r4 ) );
    m.r4 = _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&m41), M.r1), _mm_mul_ps( _mm_load1_ps(&m42), M.r2 ), _mm_mul_ps( _mm_load1_ps(&m43), M.r3), _mm_mul_ps( _mm_load1_ps(&m44), M.r4 ) );
#endif

    return m;
}

RMatrix4& RMatrix4::operator*=(const RMatrix4& M)
{
#ifndef RIOT_USE_INTRINSICS
    RMatrix4 m;
    m.m11 = (m11 * M.m11) + (m12 * M.m21) + (m13 * M.m31) + (m14 * M.m41);
    m.m12 = (m11 * M.m12) + (m12 * M.m22) + (m13 * M.m32) + (m14 * M.m42);
    m.m13 = (m11 * M.m13) + (m12 * M.m23) + (m13 * M.m33) + (m14 * M.m43);
    m.m14 = (m11 * M.m14) + (m12 * M.m24) + (m13 * M.m34) + (m14 * M.m44);
    
    m.m21 = (m21 * M.m11) + (m22 * M.m21) + (m23 * M.m31) + (m24 * M.m41);
    m.m22 = (m21 * M.m12) + (m22 * M.m22) + (m23 * M.m32) + (m24 * M.m42);
    m.m23 = (m21 * M.m13) + (m22 * M.m23) + (m23 * M.m33) + (m24 * M.m43);
    m.m24 = (m21 * M.m14) + (m22 * M.m24) + (m23 * M.m34) + (m24 * M.m44);
    
    m.m31 = (m31 * M.m11) + (m32 * M.m21) + (m33 * M.m31) + (m34 * M.m41);
    m.m32 = (m31 * M.m12) + (m32 * M.m22) + (m33 * M.m32) + (m34 * M.m42);
    m.m33 = (m31 * M.m13) + (m32 * M.m23) + (m33 * M.m33) + (m34 * M.m43);
    m.m34 = (m31 * M.m14) + (m32 * M.m24) + (m33 * M.m34) + (m34 * M.m44);
    
    m.m41 = (m41 * M.m11) + (m42 * M.m21) + (m43 * M.m31) + (m44 * M.m41);
    m.m42 = (m41 * M.m12) + (m42 * M.m22) + (m43 * M.m32) + (m44 * M.m42);
    m.m43 = (m41 * M.m13) + (m42 * M.m23) + (m43 * M.m33) + (m44 * M.m43);
    m.m44 = (m41 * M.m14) + (m42 * M.m24) + (m43 * M.m34) + (m44 * M.m44);
    *this = m;
#else
    r1 = _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&m11), M.r1), _mm_mul_ps( _mm_load1_ps(&m12), M.r2 ), _mm_mul_ps( _mm_load1_ps(&m13), M.r3), _mm_mul_ps( _mm_load1_ps(&m14), M.r4 ) );
    r2 = _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&m21), M.r1), _mm_mul_ps( _mm_load1_ps(&m22), M.r2 ), _mm_mul_ps( _mm_load1_ps(&m23), M.r3), _mm_mul_ps( _mm_load1_ps(&m24), M.r4 ) );
    r3 = _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&m31), M.r1), _mm_mul_ps( _mm_load1_ps(&m32), M.r2 ), _mm_mul_ps( _mm_load1_ps(&m33), M.r3), _mm_mul_ps( _mm_load1_ps(&m34), M.r4 ) );
    r4 = _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&m41), M.r1), _mm_mul_ps( _mm_load1_ps(&m42), M.r2 ), _mm_mul_ps( _mm_load1_ps(&m43), M.r3), _mm_mul_ps( _mm_load1_ps(&m44), M.r4 ) );
#endif
    return *this;
}

RVector4 RMatrix4::operator*(const RVector4& V) const
{
    RVector4 v;
#ifndef RIOT_USE_INTRINSICS
    v.x = m11*V.x + m21*V.y + m31*V.z + m41*V.w;
    v.y = m12*V.x + m22*V.y + m32*V.z + m42*V.w;
    v.z = m13*V.x + m23*V.y + m33*V.z + m43*V.w;
    v.w = m14*V.x + m24*V.y + m34*V.z + m44*V.w;

    RVector4 x = r0 * V.x;
    RVector4 y = r1 * V.y;
    RVector4 z = r2 * V.z;
    RVector4 w = r3 * V.w;

    v = x + y + z + w;
#else
    _mm_store_ps(v.f, _MM_QUAD_ADD_PS(_mm_mul_ps( _mm_load1_ps(&V.x), r1), _mm_mul_ps( _mm_load1_ps(&V.y), r2 ), _mm_mul_ps( _mm_load1_ps(&V.z), r3), _mm_mul_ps( _mm_load1_ps(&V.w), r4 ) ) );    
#endif
    return v;
}


// Misc matrix operations
void RMatrix4::Identity()
{
    memset(f, 0, sizeof(RMatrix4));
    m11 = m22 = m33 = m44 = 1.0f;
}

void RMatrix4::Transpose()
{
#ifndef RIOT_USE_INTRINSICS
    float t;
    
    Swap(m12,m21);
    Swap(m13,m31);
    Swap(m14,m41);

    Swap(m23,m32);
    Swap(m24,m42);

    Swap(m34,m43);
#else
    _MM_TRANSPOSE4_PS(r1, r2, r3, r4);
#endif
}

float RMatrix4::Determinant()
{
    float det = 0.0f;
#ifndef RIOT_USE_INTRINSICS 
    // Row 1
    det += m11 * RMatrix3(    m22, m23, m24, 
                            m32, m33, m34, 
                            m42, m43, m44).Determinant();

    det -= m12 * RMatrix3(    m21, m23, m24, 
                            m31, m33, m34, 
                            m41, m43, m44).Determinant();

    det += m13 * RMatrix3(    m21, m22, m24, 
                            m31, m32, m34, 
                            m41, m42, m44).Determinant();

    det -= m14 * RMatrix3(    m21, m22, m23, 
                            m31, m32, m33, 
                            m41, m42, m43).Determinant();
#else
    // Row 1
    det += m11 * RMatrix3(    m22, m23, m24, 
                            m32, m33, m34, 
                            m42, m43, m44).Determinant();

    det -= m12 * RMatrix3(    m21, m23, m24, 
                            m31, m33, m34, 
                            m41, m43, m44).Determinant();

    det += m13 * RMatrix3(    m21, m22, m24, 
                            m31, m32, m34, 
                            m41, m42, m44).Determinant();

    det -= m14 * RMatrix3(    m21, m22, m23, 
                            m31, m32, m33, 
                            m41, m42, m43).Determinant();

#endif
    return det;
}

void RMatrix4::Inverse()
{
    RMatrix4 m = *this;
#ifndef RIOT_USE_INTRINSICS 
    // Row 1
    m.m11 = RMatrix3(    m22, m23, m24, 
        m32, m33, m34, 
        m42, m43, m44).Determinant();

    m.m12 = -RMatrix3(    m21, m23, m24, 
        m31, m33, m34, 
        m41, m43, m44).Determinant();

    m.m13 = RMatrix3(    m21, m22, m24, 
        m31, m32, m34, 
        m41, m42, m44).Determinant();

    m.m14 = -RMatrix3(    m21, m22, m23, 
        m31, m32, m33, 
        m41, m42, m43).Determinant();

    // Row 2
    m.m21 = -RMatrix3(    m12, m13, m14, 
        m32, m33, m34, 
        m42, m43, m44).Determinant();

    m.m22 = RMatrix3(    m11, m13, m14, 
        m31, m33, m34, 
        m41, m43, m44).Determinant();

    m.m23 = -RMatrix3(    m11, m12, m14, 
        m31, m32, m34, 
        m41, m42, m44).Determinant();

    m.m24 = RMatrix3(    m11, m12, m13, 
        m31, m32, m33, 
        m41, m42, m43).Determinant();

    // Row 3
    m.m31 = RMatrix3(    m12, m13, m14, 
        m22, m23, m24, 
        m42, m43, m44).Determinant();

    m.m32 = -RMatrix3(    m11, m13, m14, 
        m21, m23, m24, 
        m41, m43, m44).Determinant();

    m.m33 = RMatrix3(    m11, m12, m14, 
        m21, m22, m24, 
        m41, m42, m44).Determinant();

    m.m34 = -RMatrix3(    m11, m12, m13, 
        m21, m22, m23, 
        m41, m42, m43).Determinant();

    // Row 4
    m.m41 = -RMatrix3(    m12, m13, m14, 
        m22, m23, m24, 
        m32, m33, m34).Determinant();

    m.m42 = RMatrix3(    m11, m13, m14, 
        m21, m23, m24, 
        m31, m33, m34).Determinant();

    m.m43 = -RMatrix3(    m11, m12, m14, 
        m21, m22, m24, 
        m31, m32, m34).Determinant();

    m.m44 = RMatrix3(    m11, m12, m13, 
        m21, m22, m23, 
        m31, m32, m33).Determinant();
#else
    // Row 1
    m.m11 = RMatrix3(    m22, m23, m24, 
        m32, m33, m34, 
        m42, m43, m44).Determinant();

    m.m12 = -RMatrix3(    m21, m23, m24, 
        m31, m33, m34, 
        m41, m43, m44).Determinant();

    m.m13 = RMatrix3(    m21, m22, m24, 
        m31, m32, m34, 
        m41, m42, m44).Determinant();

    m.m14 = -RMatrix3(    m21, m22, m23, 
        m31, m32, m33, 
        m41, m42, m43).Determinant();

    // Row 2
    m.m21 = -RMatrix3(    m12, m13, m14, 
        m32, m33, m34, 
        m42, m43, m44).Determinant();

    m.m22 = RMatrix3(    m11, m13, m14, 
        m31, m33, m34, 
        m41, m43, m44).Determinant();

    m.m23 = -RMatrix3(    m11, m12, m14, 
        m31, m32, m34, 
        m41, m42, m44).Determinant();

    m.m24 = RMatrix3(    m11, m12, m13, 
        m31, m32, m33, 
        m41, m42, m43).Determinant();

    // Row 3
    m.m31 = RMatrix3(    m12, m13, m14, 
        m22, m23, m24, 
        m42, m43, m44).Determinant();

    m.m32 = -RMatrix3(    m11, m13, m14, 
        m21, m23, m24, 
        m41, m43, m44).Determinant();

    m.m33 = RMatrix3(    m11, m12, m14, 
        m21, m22, m24, 
        m41, m42, m44).Determinant();

    m.m34 = -RMatrix3(    m11, m12, m13, 
        m21, m22, m23, 
        m41, m42, m43).Determinant();

    // Row 4
    m.m41 = -RMatrix3(    m12, m13, m14, 
        m22, m23, m24, 
        m32, m33, m34).Determinant();

    m.m42 = RMatrix3(    m11, m13, m14, 
        m21, m23, m24, 
        m31, m33, m34).Determinant();

    m.m43 = -RMatrix3(    m11, m12, m14, 
        m21, m22, m24, 
        m31, m32, m34).Determinant();

    m.m44 = RMatrix3(    m11, m12, m13, 
        m21, m22, m23, 
        m31, m32, m33).Determinant();
#endif
    m.Transpose();
    float recip = 1/Determinant();

    *this = m * recip;
}

void RMatrix4::SetPosition(const RVector4 &v)
{
    SetPosition(v.f);
}

void RMatrix4::SetPosition(const float *F)
{
    memcpy(f+12, F, sizeof(float) * 3);
}

void RMatrix4::SetPosition(float x, float y, float z)
{
    m41 = x, m42 = y, m43 = z;
}

RVector4 RMatrix4::GetPosition(void) const
{
    return RVector4(&m41);
}

RMatrix4 TranslationMatrix(float x, float y, float z, RMatrix4* pMatrix)
{
    RMatrix4 m;
    m.Identity();
    m.m41 = x, m.m42 = y, m.m43 = z;
    if(pMatrix)
        *pMatrix = m;

    return m;
}

RMatrix4 RotationXMatrix(float rad, RMatrix4* pMatrix)
{
    RMatrix4 m;
    m.Identity();

    float c = cosf(rad);
    float s = sinf(rad);

    m.m22 = m.m33 = c;
    m.m23 = s;
    m.m32 = -s;

    if(pMatrix)

        *pMatrix = m;

    return m;
}

RMatrix4 RotationYMatrix(float rad, RMatrix4* pMatrix)
{
    RMatrix4 m;
    m.Identity();

    float c = cosf(rad);
    float s = sinf(rad);

    m.m11 = m.m33 = c;
    m.m13 = -s;
    m.m31 = s;

    if(pMatrix)

        *pMatrix = m;

    return m;
}

RMatrix4 RotationZMatrix(float rad, RMatrix4* pMatrix)
{
    RMatrix4 m;
    m.Identity();

    float c = cos(rad);
    float s = sinf(rad);

    m.m11 = m.m22 = c;
    m.m12 = s;
    m.m21 = -s;

    if(pMatrix)

        *pMatrix = m;

    return m;
}

RMatrix4 RotationAxisMatrix(float rad, RVector4& axis, RMatrix4* pMatrix )
{
    float c = cosf(rad);
    float s = sinf(rad);
    float t = 1 - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    RMatrix4 m;
    m.Identity();
    m.m11 = t * x * x + c;
    m.m12 = (t * x * y) - s * z;
    m.m13 = (t * x * z) + s * y;

    m.m21 = (t * x * y) + (s * z);
    m.m22 = (t * y * y) + c;
    m.m23 = (t * y * z) - (s * x);
    
    m.m31 = (t * x * z) - (s * y);
    m.m32 = (t * y * z) + (s * x);
    m.m33 = (t * z * z) + c;

    return m;
}

RMatrix4 ScalingMatrix(float x, float y, float z, RMatrix4* pMatrix)
{
    RMatrix4 m;
    m.Identity();
    m.m11 = x, m.m22 = y, m.m33 = z;
    if(pMatrix)
        *pMatrix = m;

    return m;
}

RMatrix4 PerspectiveLHMatrix(float fFOV, float fAspect, float fZNear, float fZFar)
{
    RMatrix4 m;
    m.Identity();
    float y = 1/(tanf(fFOV/2));
    float diff = fZFar-fZNear;
    float div = fZFar / diff;
    m.m11 =  y / fAspect;
    m.m22 = y;
    m.m33 = div;
    m.m34 = 1;
    m.m43 = -fZNear * div;
    m.m44 = 0;
    return m;
}

RMatrix4 LookAt(RVector4& pos, RVector4& target, RVector4& up)
{
    RVector4    xAxis,
        yAxis,
        zAxis;

    RMatrix4 mView;
    mView.Identity();

    zAxis = target - pos;
    zAxis.Normalize();
    
    xAxis = CrossProduct(up, zAxis);
    xAxis.Normalize();

    yAxis = CrossProduct(zAxis, xAxis);

    mView.m11 = xAxis.x;
    mView.m21 = xAxis.y;
    mView.m31 = xAxis.z;
    mView.m41 = -(DotProduct(xAxis, pos));

    mView.m12 = yAxis.x;
    mView.m22 = yAxis.y;
    mView.m32 = yAxis.z;
    mView.m42 = -(DotProduct(yAxis, pos));

    mView.m13 = zAxis.x;
    mView.m23 = zAxis.y;
    mView.m33 = zAxis.z;
    mView.m43 = -(DotProduct(zAxis, pos));
    
    return mView;
}

RMatrix4 RMatrix4Identity() 
{ 
    return RMatrix4( 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f ); 
}
