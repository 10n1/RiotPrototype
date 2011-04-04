/*********************************************************\
File:           RenderObject.h
Purpose:        Base interface for any object processed by
                the Render engine
Author:         Kyle Weicht
Created:        4/3/2011
Modified:       4/3/2011 8:28:17 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _RENDEROBJECT_H_
#define _RENDEROBJECT_H_
#include "Common.h"
#include "IRefCounted.h"

class CRenderObject : public IRefCounted
{
public:
    // CRenderObject constructor
    CRenderObject() { }

    // CRenderObject destructor
    virtual ~CRenderObject() { }
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  ProcessObject
    //  Processes the object
    //-----------------------------------------------------------------------------
    virtual void ProcessObject( void ) = 0;

private:
    /***************************************\
    | class members                         |
    \***************************************/

};


#endif // #ifndef _RENDEROBJECT_H_
