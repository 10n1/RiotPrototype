/*********************************************************\
File:           Settings.cpp
Author:         Kyle Weicht
Created:        4/27/2011
Modified:       5/1/2011 6:55:50 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "settings.h"

namespace Riot
{
    
#define DEFINE_SETTING( Type, Name, InitialValue ) \
    TSetting<Type> Name( #Name, InitialValue )

    
    DEFINE_SETTING( bool, gbShowBoundingVolumes, true );
    DEFINE_SETTING( bool, gbRenderWireframe, false );
    DEFINE_SETTING( bool, gbRenderOn, true );
    DEFINE_SETTING( bool, gbConsoleActive, false );
    DEFINE_SETTING( bool, gbShowFPS, false );

    template<class T>
    TSetting<T>::TSetting( const char* szName, T initialValue )
        : m_nNameHash( StringHash32CaseInsensitive( szName ) )
        , m_Value( initialValue )
    {
    }

    TSetting<bool>* m_bSettings[] = 
    {
        &gbShowBoundingVolumes,
        &gbRenderWireframe,
        &gbRenderOn,
        &gbConsoleActive,
        &gbShowFPS,
    };
    static uint gs_nNumBoolSettings = ARRAY_LENGTH( m_bSettings );

    void SetBoolSetting( const char* szVariableName, bool bValue )
    {
        uint nHash = StringHash32CaseInsensitive( szVariableName );

        bool bFound = false;
        for( uint i = 0; i < gs_nNumBoolSettings; ++i )
        {
            if( nHash == m_bSettings[i]->GetNameHash() )
            {
                *m_bSettings[i] = bValue;
                break;
            }
        }
    }

} // namespace Riot
