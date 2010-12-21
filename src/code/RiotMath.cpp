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
