/*********************************************************\
File:           GraphicsObjects.h
Purpose:        Interfaces for all the graphics objects in the engine
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 8:57:51 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _GRAPHICSOBJECTS_H_
#define _GRAPHICSOBJECTS_H_
#include "common.h"
#include "IRefCounted.h"

namespace Riot
{
    class IRenderTarget : public IRefCounted
    {
    public:
        virtual ~IRenderTarget() { }
    };

} // namespace Riot

#endif // #ifndef _GRAPHICSOBJECTS_H_
