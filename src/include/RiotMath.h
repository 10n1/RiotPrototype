/*********************************************************\
File:       RiotMath.h
Purpose:    Vector and matrices implementations
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
    RiotVector3<T> r1;
    RiotVector3<T> r2;
    RiotVector3<T> r3;
};

template< class T >
class RiotMatrix4x4
{
public:
    RiotVector4<T> r1;
    RiotVector4<T> r2;
    RiotVector4<T> r3;
    RiotVector4<T> r4;
};

#endif _RIOTMATH_H_