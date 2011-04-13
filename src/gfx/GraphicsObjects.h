/*********************************************************\
File:           GraphicsObjects.h
Purpose:        Interfaces for all the graphics objects in the engine
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 9:41:47 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _GRAPHICSOBJECTS_H_
#define _GRAPHICSOBJECTS_H_
#include "common.h"
#include "IRefCounted.h"

// All interfaces are method-less
#define DEFINE_BASE_GRAPHICS_INTERFACE( Name )    \
class IGfx##Name : public IRefCounted  \
    {                               \
    public:                         \
        virtual ~IGfx##Name() { }      \
    }

namespace Riot
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    DEFINE_BASE_GRAPHICS_INTERFACE( RenderTarget );
    DEFINE_BASE_GRAPHICS_INTERFACE( Buffer );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace Riot

#undef CREATE_GRAPHICS_INTERFACE


#endif // #ifndef _GRAPHICSOBJECTS_H_
