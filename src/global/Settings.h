/*********************************************************\
File:           Settings.h
Purpose:        Holds all the "global" game settings that
                can be modified in game via the console
Author:         Kyle Weicht
Created:        4/27/2011
Modified:       5/2/2011 7:36:28 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SETTINGS_H_
#define _SETTINGS_H_
#include "types.h"
#include "config.h"

namespace Riot
{    
    template<class T>
    struct TSetting
    {
    public:
        TSetting( const char* szName, T initialValue );
        inline T GetValue( void ) { return m_Value; }
        inline void SetValue( const T& val ) { m_Value = val; }

        inline operator T&() { return m_Value; }
        inline operator const T&() const { return m_Value; }
        inline const T& operator=( const T& r ) { m_Value = r; return m_Value; }
        
        inline uint GetNameHash( void ) { return m_nNameHash; }
    private:
        T       m_Value;
        uint    m_nNameHash;
    };
    
#define DECLARE_SETTING( Type, Name ) \
    extern TSetting<Type> Name;

    DECLARE_SETTING( uint, gnShowBoundingVolumes );
    DECLARE_SETTING( uint, gnRenderWireframe );
    DECLARE_SETTING( uint, gnRenderOn );
    DECLARE_SETTING( uint, gnConsoleActive );
    DECLARE_SETTING( uint, gnShowFPS );

    void SetSetting( const char* szVariableName, float fValue );
    void SetSetting( const char* szVariableName, int nValue );

    
    inline uint StringHash32( const char* szString )
    {
        uint nHash = 0;
        uint nCharIndex = 0;

        while( szString[nCharIndex] != 0 )
        {
            uint nThisChar = szString[nCharIndex] << (nCharIndex % 4 ) * 8;
            nHash |= nThisChar;

            nCharIndex++;
        }

        return nHash;
    }
    
    inline uint64 StringHash64( const char* szString )
    {
        uint64 nHash = 0;
        uint nCharIndex = 0;

        while( szString[nCharIndex] != 0 )
        {
            uint nThisChar = szString[nCharIndex] << (nCharIndex % 8 ) * 8;
            nHash |= nThisChar;

            nCharIndex++;
        }

        return nHash;
    }

    inline uint StringHash32CaseInsensitive( const char* szString )
    {
        uint nHash = 0;
        uint nCharIndex = 0;

        while( szString[nCharIndex] != 0 )
        {
            uint nChar = szString[nCharIndex];
            if( nChar >= 'a' && nChar <= 'z' )
            {
                nChar -= ('a' - 'A');
            }
            uint nThisChar = nChar << (nCharIndex % 4 ) * 8;
            nHash |= nThisChar;

            nCharIndex++;
        }

        return nHash;
    }

} // namespace Riot

#endif // #ifndef _SETTINGS_H_
