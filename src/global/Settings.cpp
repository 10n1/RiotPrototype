/*********************************************************\
File:           Settings.cpp
Author:         Kyle Weicht
Created:        4/27/2011
Modified:       5/2/2011 5:10:01 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "settings.h"

namespace Riot
{
    
#define DEFINE_SETTING( Type, Name, InitialValue ) \
    TSetting<Type> Name( #Name, InitialValue )

    
    DEFINE_SETTING( uint, gbShowBoundingVolumes, 0 );
    DEFINE_SETTING( uint, gbRenderWireframe, 0 );
    DEFINE_SETTING( uint, gbRenderOn, 0 );
    DEFINE_SETTING( uint, gbConsoleActive, 0 );
    DEFINE_SETTING( uint, gbShowFPS, 1 );

    template<class T>
    TSetting<T>::TSetting( const char* szName, T initialValue )
        : m_nNameHash( StringHash32CaseInsensitive( szName ) )
        , m_Value( initialValue )
    {
    }

    TSetting<uint>* m_bSettings[] = 
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
