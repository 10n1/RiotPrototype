/*********************************************************\
File:           Settings.cpp
Author:         Kyle Weicht
Created:        4/27/2011
Modified:       5/2/2011 7:42:01 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "settings.h"

namespace Riot
{
    
#define DEFINE_SETTING( Type, Name, InitialValue ) \
    TSetting<Type> Name( #Name, InitialValue )

    
    DEFINE_SETTING( uint, gnShowBoundingVolumes, 0 );
    DEFINE_SETTING( uint, gnRenderWireframe, 0 );
    DEFINE_SETTING( uint, gnRenderOn, 0 );
    DEFINE_SETTING( uint, gnConsoleActive, 0 );
    DEFINE_SETTING( uint, gnShowFPS, 1 );

    template<class T>
    TSetting<T>::TSetting( const char* szName, T initialValue )
        : m_nNameHash( StringHash32CaseInsensitive( szName ) )
        , m_Value( initialValue )
    {
    }

    TSetting<uint>* m_nSettings[] = 
    {
        &gnShowBoundingVolumes,
        &gnRenderWireframe,
        &gnRenderOn,
        &gnConsoleActive,
        &gnShowFPS,
    };
    
    DEFINE_SETTING( float, fNothing, 0.0f );
    TSetting<float>* m_fSettings[1] = 
    {
        &fNothing,
    };

    static uint gs_nNumIntSettings = ARRAY_LENGTH( m_nSettings );
    static uint gs_nNumFloatSettings = 0;

    void SetSetting( const char* szVariableName, float fValue )
    {
        uint nHash = StringHash32CaseInsensitive( szVariableName );

        bool bFound = false;
        for( uint i = 0; i < gs_nNumFloatSettings; ++i )
        {
            if( nHash == m_fSettings[i]->GetNameHash() )
            {
                *m_fSettings[i] = fValue;
                break;
            }
        }
    }
    void SetSetting( const char* szVariableName, int nValue )
    {
        uint nHash = StringHash32CaseInsensitive( szVariableName );

        bool bFound = false;
        for( uint i = 0; i < gs_nNumIntSettings; ++i )
        {
            if( nHash == m_nSettings[i]->GetNameHash() )
            {
                *m_nSettings[i] = nValue;
                break;
            }
        }
    }

} // namespace Riot
