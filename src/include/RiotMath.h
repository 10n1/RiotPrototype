/*********************************************************\
File:       vector.h
Purpose:    STL vector replacement
\*********************************************************/
#ifndef _RIOTMATH_H_
#define _RIOTMATH_H_

template< class T >
class RiotVector2
{
public:
    T x;
    T y;
};

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
    RiotVector3<float> r1;
    RiotVector3<float> r2;
    RiotVector3<float> r3;
};

template< class T >
class RiotMatrix4x4
{
public:
    RiotVector4<float> r1;
    RiotVector4<float> r2;
    RiotVector4<float> r3;
    RiotVector4<float> r4;
};

#endif _RIOTMATH_H_