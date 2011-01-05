/*********************************************************\
File:      RiotMath.cpp
Purpose:   Math function declarations
TODO:      Should some of (most) be split into a .inl
             file for inlining?
\*********************************************************/
#include "..\include\RiotMath.h"

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
