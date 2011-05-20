#ifndef _CHARACTER_H_
#define _CHARACTER_H_
#include "common.h"
#include "IListener.h"

namespace Riot
{
    class CMesh;
    class CView;
    
    class CCharacter : public IListener
    {
    public:
        CCharacter( void );
        ~CCharacter( void );

        CView* GetView( void );
        RTransform& GetTransform( void );

    private:
        CMesh* m_pMesh;
        CView* m_pView;
        RTransform m_tTransform;
        uint m_nCharacterID;
    };
};

#endif // _CHARACTER_H_
