/*********************************************************\
File:           View.h
Purpose:        Interface for anything in the engine that
                can "view", aka, a camera
Author:         Kyle Weicht
Created:        4/14/2011
Modified:       4/14/2011 10:47:38 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _VIEW_H_
#define _VIEW_H_
#include "common.h"
#include "IRefCounted.h"

namespace Riot
{

    class CView : public IRefCounted
    {
    public:
        // CView constructor
        CView();

        // CView destructor
        ~CView();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  Translate
        //  Moves the camera
        //-----------------------------------------------------------------------------
        void TranslateX( float fTrans );
        void TranslateY( float fTrans );
        void TranslateZ( float fTrans );
        // TODO: Should the view be attached to an object
        //       Maybe it should have the option? So some views
        //       are independent, while others are attached
        //       to their actors (eg: the player)

        //-----------------------------------------------------------------------------
        //  Rotate
        //  Rotates the camera
        //-----------------------------------------------------------------------------
        void RotateX( float fRad );
        void RotateY( float fRad );

        //-----------------------------------------------------------------------------
        //  Update
        //  Updates the object
        //  TODO: Pre- and Post- updates?
        //-----------------------------------------------------------------------------
        void Update( float fDeltaTime );

        //-----------------------------------------------------------------------------
        //  SetPerspective
        //  Sets the projection matrix
        //-----------------------------------------------------------------------------
        void SetPerspective( float fFoV, float fAspectRatio, float fNear, float fFar );

        //-----------------------------------------------------------------------------
        //  GetView/ProjMatrix
        //  Returns the view/proj matrix
        //-----------------------------------------------------------------------------
        const RMatrix4& GetViewMatrix( void );
        const RMatrix4& GetProjMatrix( void );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        RMatrix4    m_mNewView;
        RMatrix4    m_mNewProj;

        RVector4    m_vNewPosition;
        RVector4    m_vNewUp;
        RVector4    m_vNewLook;
        RVector4    m_vNewRight;
    };

} // namespace Riot


#endif // #ifndef _VIEW_H_
