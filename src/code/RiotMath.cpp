/*********************************************************\
File:       RiotMath.cpp
Purpose:    Math definitions
\*********************************************************/
#include "..\include\RiotMath.h"

/**********************************************************\
| class Vector2
\**********************************************************/
Vector2::Vector2(float X, float Y) : x(X), y(Y) 
{ 
}

Vector2::Vector2(const Vector2& V) : x(V.x), y(V.y)
{ 
}

Vector2::Vector2(const float* F) : x(F[0]), y(F[1])
{ 
}

Vector2& Vector2::operator=(const Vector2& V)
{
    x = V.x, y = V.y;
    return *this;
}

/***************************************\
| class methods
\***************************************/
// Scalar math operations

// Add
Vector2 Vector2::operator+(const Vector2& V) const
{ 
    return Vector2( x + V.x, y + V.y);
}

Vector2 Vector2::operator+(float F) const 
{ 
    return Vector2( x + F, y + F); 
}

Vector2& Vector2::operator+=(const Vector2& V) 
{ 
    x += V.x, y += V.y; return *this; 
}

Vector2& Vector2::operator+=(float F) 
{ 
    x += F, y += F; return *this; 
}


// Subtract
Vector2 Vector2::operator-(const Vector2& V) const 
{ 
    return Vector2( x - V.x, y - V.y); 
}

Vector2 Vector2::operator-(float F) const 
{ 
    return Vector2( x - F, y - F); 
}

Vector2& Vector2::operator-=(const Vector2& V) 
{ 
    x -= V.x, y -= V.y; return *this; 
}

Vector2& Vector2::operator-=(float F) 
{ 
    x -= F, y -= F; return *this; 
}


// Multiply and divide
Vector2 Vector2::operator*(float F) const 
{ 
    return Vector2( x * F, y * F); 
}

Vector2& Vector2::operator*=(float F) 
{ 
    x *= F, y *= F; return *this; 
}

Vector2 Vector2::operator/(float F) const 
{ 
    return Vector2( x / F, y / F); 
}

Vector2& Vector2::operator/=(float F) 
{ 
    x /= F, y /= F; return *this; 
}


// Comparison
bool Vector2::operator==(const Vector2& V) const 
{ 
    return (x == V.x && y == V.y); 
}

/*********************************************************\
File:      RiotMath.cpp
Purpose:   Math function declarations
TODO:      Should some of (most) be split into a .inl
             file for inlining?
\*********************************************************/

bool Vector2::operator!=(const Vector2& V) const 
{ 
    return !(x == V.x && y == V.y); 
}


// Vector operations

// Vector-Vector operations
float Vector2::DotProduct(const Vector2& V) const
{
    return x * V.x + y * V.y;
}

// Misc operations
float Vector2::Magnitude(void) const
{
    return sqrtf(MagnitudeSquared());
}

float Vector2::MagnitudeSquared(void) const
{
    return x*x + y*y;
}

float Vector2::Distance(const Vector2& V) const
{
    return sqrtf(DistanceSquared(V));
}

float Vector2::DistanceSquared(const Vector2& V) const
{
    return Vector2(x - V.x, y - V.y).MagnitudeSquared();
}

void Vector2::Normalize(void)
{
    *this /= Magnitude();
}

void Vector2::Zero(void)
{
    x = 0.0f, y = 0.0f;
}

// Non-member functions
float DotProduct(const Vector2& V1, const Vector2& V2)
{
    return V1.DotProduct(V2);
}

float Distance(const Vector2& V1, const Vector2& V2)
{
    return V1.Distance(V2);
}

float DistanceSquared(const Vector2& V1, const Vector2& V2)
{
    return V1.DistanceSquared(V2);
}

Vector2 Normalize(const Vector2& V)
{
    return Vector2(V) / V.Magnitude();
}
