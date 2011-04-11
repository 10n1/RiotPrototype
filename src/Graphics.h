/*********************************************************\
File:           Graphics.h
Purpose:        Base graphics hardware API
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 11:42:44 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
#include "common.h"
#include "IRefCounted.h"

namespace Riot
{

    class CGraphicsDevice : public IRefCounted
    {
    public:
        // CGraphicsDevice constructor
        CGraphicsDevice();

        // CGraphicsDevice destructor
        virtual ~CGraphicsDevice();
        /***************************************\
        | class methods                         |
        \***************************************/
        
        virtual void SetClearColor( float fRed, float fGreen, float fBlue, float fAlpha ) = 0;
        virtual void SetClearDepth( float fDepth ) = 0;
        virtual void Clear( void ) = 0;
        virtual void Present( void ) = 0;

    private:
        /***************************************\
        | class members                         |
        \***************************************/

    };


} // namespace Riot

#endif // #ifndef _GRAPHICS_H_
