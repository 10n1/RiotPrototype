/*********************************************************\
File:      RiotMath.cpp
Purpose:   Math function declarations
TODO:      Should some of (most) be split into a .inl
             file for inlining?
\*********************************************************/
#include "..\include\RiotMath.h"

/**********************************************************\
| class RiotVector2
\**********************************************************/
RiotVector2::RiotVector2(float X, float Y) : x(X), y(Y) 
{ 
}

RiotVector2::RiotVector2(const RiotVector2& V) : x(V.x), y(V.y)
{ 
}

RiotVector2::RiotVector2(const float* F) : x(F[0]), y(F[1])
{ 
}

RiotVector2& RiotVector2::operator=(const RiotVector2& V)
{
    x = V.x, y = V.y;
    return *this;
}

/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RiotVector2 RiotVector2::operator+(const RiotVector2& V) const
{ 
    return RiotVector2( x + V.x, y + V.y);
}

RiotVector2 RiotVector2::operator+(float F) const 
{ 
    return RiotVector2( x + F, y + F); 
}

RiotVector2& RiotVector2::operator+=(const RiotVector2& V) 
{ 
    x += V.x, y += V.y; return *this; 
}

RiotVector2& RiotVector2::operator+=(float F) 
{ 
    x += F, y += F; return *this; 
}


// Subtract
RiotVector2 RiotVector2::operator-(const RiotVector2& V) const 
{ 
    return RiotVector2( x - V.x, y - V.y); 
}

RiotVector2 RiotVector2::operator-(float F) const 
{ 
    return RiotVector2( x - F, y - F); 
}

RiotVector2& RiotVector2::operator-=(const RiotVector2& V) 
{ 
    x -= V.x, y -= V.y; return *this; 
}

RiotVector2& RiotVector2::operator-=(float F) 
{ 
    x -= F, y -= F; return *this; 
}


// Multiply and divide
RiotVector2 RiotVector2::operator*(float F) const 
{ 
    return RiotVector2( x * F, y * F); 
}

RiotVector2& RiotVector2::operator*=(float F) 
{ 
    x *= F, y *= F; return *this; 
}

RiotVector2 RiotVector2::operator/(float F) const 
{ 
    return RiotVector2( x / F, y / F); 
}

RiotVector2& RiotVector2::operator/=(float F) 
{ 
    x /= F, y /= F; return *this; 
}


// Comparison
bool RiotVector2::operator==(const RiotVector2& V) const 
{ 
    return (x == V.x && y == V.y); 
}

bool RiotVector2::operator!=(const RiotVector2& V) const 
{ 
    return !(x == V.x && y == V.y); 
}


// Vector operations

// Vector-Vector operations
float RiotVector2::DotProduct(const RiotVector2& V) const
{
    return x * V.x + y * V.y;
}

// Misc operations
float RiotVector2::Magnitude(void) const
{
    return sqrtf(MagnitudeSquared());
}

float RiotVector2::MagnitudeSquared(void) const
{
    return x*x + y*y;
}

float RiotVector2::Distance(const RiotVector2& V) const
{
    return sqrtf(DistanceSquared(V));
}

float RiotVector2::DistanceSquared(const RiotVector2& V) const
{
    return RiotVector2(x - V.x, y - V.y).MagnitudeSquared();
}

void RiotVector2::Normalize(void)
{
    *this /= Magnitude();
}

void RiotVector2::Zero(void)
{
    x = 0.0f, y = 0.0f;
}

// Non-member functions
float DotProduct(const RiotVector2& V1, const RiotVector2& V2)
{
    return V1.DotProduct(V2);
}

float Distance(const RiotVector2& V1, const RiotVector2& V2)
{
    return V1.Distance(V2);
}

float DistanceSquared(const RiotVector2& V1, const RiotVector2& V2)
{
    return V1.DistanceSquared(V2);
}

RiotVector2 Normalize(const RiotVector2& V)
{
    return RiotVector2(V) / V.Magnitude();
}


/**********************************************************\
|**********************************************************|
| class RiotVector3
|**********************************************************|
\**********************************************************/
RiotVector3::RiotVector3(float X, float Y, float Z) : x(X), y(Y), z(Z)
{ 
}

RiotVector3::RiotVector3(const RiotVector3& V) : x(V.x), y(V.y), z(V.z)
{ 
}

RiotVector3::RiotVector3(const float* F) : x(F[0]), y(F[1]), z(F[2])
{ 
}

RiotVector3& RiotVector3::operator=(const RiotVector3& V)
{
    x = V.x, y = V.y, z= V.z;
    return *this;
}

/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RiotVector3 RiotVector3::operator+(const RiotVector3& V) const
{ 
    return RiotVector3( x + V.x, y + V.y, z + V.z); 
}

RiotVector3 RiotVector3::operator+(float F) const 
{ 
    return RiotVector3( x + F, y + F, z + F); 
}

RiotVector3& RiotVector3::operator+=(const RiotVector3& V) 
{ 
    x += V.x, y += V.y, z += V.z; return *this; 
}

RiotVector3& RiotVector3::operator+=(float F) 
{ 
    x += F, y += F, z += F; return *this; 
}


// Subtract
RiotVector3 RiotVector3::operator-(const RiotVector3& V) const
{ 
    return RiotVector3( x - V.x, y - V.y, z - V.z); 
}

RiotVector3 RiotVector3::operator-(float F) const 
{ 
    return RiotVector3( x - F, y - F, z - F); 
}

RiotVector3& RiotVector3::operator-=(const RiotVector3& V) 
{ 
    x -= V.x, y -= V.y, z -= V.z; return *this; 
}

RiotVector3& RiotVector3::operator-=(float F) 
{ 
    x -= F, y -= F, z -= F; return *this; 
}

// Multiply and divide
RiotVector3 RiotVector3::operator*(float F) const 
{ 
    return RiotVector3( x * F, y * F, z * F); 
}

RiotVector3& RiotVector3::operator*=(float F) 
{ 
    x *= F, y *= F, z *= F; return *this; 
}

RiotVector3 RiotVector3::operator/(float F) const 
{
    float recip = 1 / F;
    return RiotVector3( x * recip, y * recip, z * recip);
}

RiotVector3& RiotVector3::operator/=(float F) 
{ 
    float recip = 1 / F;
    x *= recip, y *= recip, z *= recip; return *this; 
}


// Comparison
bool RiotVector3::operator==(const RiotVector3& V) const 
{ 
    return (x == V.x && y == V.y && z == V.z); 
}

bool RiotVector3::operator!=(const RiotVector3& V) const 
{ 
    return !(*this == V); 
}


// Vector operations

// Vector-Vector operations
float RiotVector3::DotProduct(const RiotVector3& V) const
{
    return x * V.x + y * V.y + z * V.z;
}

RiotVector3 RiotVector3::CrossProduct(const RiotVector3& V) const
{
    float X = y * V.z - z * V.y;
    float Y = z * V.x - x * V.z;
    float Z = x * V.y - y * V.x;

    return RiotVector3(X, Y, Z);
}

// Misc operations
float RiotVector3::Magnitude(void) const
{
    return sqrtf(MagnitudeSquared());
}

float RiotVector3::MagnitudeSquared(void) const
{
    return x*x + y*y + z*z;
}

float RiotVector3::Distance(const RiotVector3& V) const
{
    return sqrtf(DistanceSquared(V));
}

float RiotVector3::DistanceSquared(const RiotVector3& V) const
{
    return RiotVector3(x - V.x, y - V.y, z - V.z).MagnitudeSquared();
}

void RiotVector3::Normalize(void)
{
    float recip = 1 / Magnitude();
    *this *= recip;
}

void RiotVector3::Zero(void)
{
    x = 0.0f, y = 0.0f, z = 0.0f;
}

// Non-member functions
float DotProduct(const RiotVector3& V1, const RiotVector3& V2)
{
    return V1.DotProduct(V2);
}

RiotVector3 CrossProduct(const RiotVector3& V1, const RiotVector3& V2)
{
    return V1.CrossProduct(V2);
}

float Distance(const RiotVector3& V1, const RiotVector3& V2)
{
    return V1.Distance(V2);
}

float DistanceSquared(const RiotVector3& V1, const RiotVector3& V2)
{
    return V1.DistanceSquared(V2);
}

RiotVector3 Normalize(const RiotVector3& V)
{
    float recip = 1 / V.Magnitude();
    return RiotVector3(V) * recip;
}

/**********************************************************\
|**********************************************************|
| class RiotVector4
|**********************************************************|
\**********************************************************/
RiotVector4::RiotVector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W)
{ 
}

RiotVector4::RiotVector4(const RiotVector4& V) : x(V.x), y(V.y), z(V.z), w (V.w)
{ 
}

RiotVector4::RiotVector4(float X, float Y, float Z)  : x(X), y(Y), z(Z), w(0.0f)
{
}

RiotVector4::RiotVector4(const float* F) : x(F[0]), y(F[1]), z(F[2]), w(F[3])
{ 
}

RiotVector4& RiotVector4::operator=(const RiotVector4& V)
{
    x = V.x, y = V.y, z= V.z, w = V.w;
    return *this;
}

/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
RiotVector4 RiotVector4::operator+(const RiotVector4& V) const
{ 
    return RiotVector4( x + V.x, y + V.y, z + V.z, w + V.w);
}

RiotVector4 RiotVector4::operator+(float F) const 
{ 
    return RiotVector4( x + F, y + F, z + F, w + F); 
}

RiotVector4& RiotVector4::operator+=(const RiotVector4& V) 
{ 
    x += V.x, y += V.y, z += V.z, w += V.w; 
    return *this;
}

RiotVector4& RiotVector4::operator+=(float F) 
{
    x += F, y += F, z += F, w += F; 
    return *this; 
}


// Subtract
RiotVector4 RiotVector4::operator-(const RiotVector4& V) const
{ 
    return RiotVector4( x - V.x, y - V.y, z - V.z, w - V.w); 
}

RiotVector4 RiotVector4::operator-(float F) const 
{ 
    return RiotVector4( x - F, y - F, z - F, w - F); 
}

RiotVector4& RiotVector4::operator-=(const RiotVector4& V) 
{ 
    x -= V.x, y -= V.y, z -= V.z, w -= V.w;
    return *this;
}

RiotVector4& RiotVector4::operator-=(float F) 
{ 
    x -= F, y -= F, z -= F, w -= F; 
    return *this; 
}


// Multiply and divide
RiotVector4 RiotVector4::operator*(float F) const 
{ 
    return RiotVector4( x * F, y * F, z * F, w * F); 
}

RiotVector4& RiotVector4::operator*=(float F) 
{ 
    x *= F, y *= F, z *= F, w *= F; 
    return *this; 
}

RiotVector4 RiotVector4::operator/(float F) const 
{
    float recip = 1 / F;
    return RiotVector4( x * recip, y * recip, z * recip, w * recip);
}

RiotVector4& RiotVector4::operator/=(float F) 
{
    float recip = 1 / F;
    x *= recip, y *= recip, z *= recip, w *= recip; 
    return *this;
}


// Comparison
bool RiotVector4::operator==(const RiotVector4& V) const 
{ 
    return (x == V.x && y == V.y && z == V.z && w == V.w); 
}

bool RiotVector4::operator!=(const RiotVector4& V) const 
{ 
    return !(*this == V); 
}


// Vector operations

// Vector-Vector operations
float RiotVector4::DotProduct(const RiotVector4& V) const
{
    return x * V.x + y * V.y + z * V.z + w * V.w;
}

RiotVector4 RiotVector4::CrossProduct(const RiotVector4& V) const
{
    float X = y * V.z - z * V.y;
    float Y = z * V.x - x * V.z;
    float Z = x * V.y - y * V.x;

    return RiotVector4(X, Y, Z, 0.0f);
}


// Misc operations
float RiotVector4::Magnitude(void) const
{
    return sqrtf(MagnitudeSquared());
}

float RiotVector4::MagnitudeSquared(void) const
{
    return x*x + y*y + z*z + w*w;
}

float RiotVector4::Distance(const RiotVector4& V) const
{
    return sqrtf(DistanceSquared(V));
}

float RiotVector4::DistanceSquared(const RiotVector4& V) const
{
    return RiotVector4(x - V.x, y - V.y, z - V.z, w - V.w).MagnitudeSquared();
}

void RiotVector4::Normalize(void)
{
    float recip = 1 / Magnitude();
    *this *= recip;
}

void RiotVector4::Zero(void)
{
    x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
}

// Non-member functions
float DotProduct(const RiotVector4& V1, const RiotVector4& V2)
{
    return V1.DotProduct(V2);
}

RiotVector4 CrossProduct(const RiotVector4& V1, const RiotVector4& V2)
{
    return V1.CrossProduct(V2);
}

float Distance(const RiotVector4& V1, const RiotVector4& V2)
{
    return V1.Distance(V2);
}

float DistanceSquared(const RiotVector4& V1, const RiotVector4& V2)
{
    return V1.DistanceSquared(V2);
}

RiotVector4 Normalize(const RiotVector4& V)
{
    float recip = 1 / V.Magnitude();
    return RiotVector4(V) * recip;
}

RiotVector4 RiotVector4Zero() 
{ 
    return RiotVector4(0.0f, 0.0f, 0.0f, 0.0f); 
}
