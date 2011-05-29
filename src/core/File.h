/*********************************************************\
File:           File.h
Purpose:        
Author:         Kyle Weicht
Created:        5/29/2011
Modified:       5/29/2011 1:30:31 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _FILE_H_
#define _FILE_H_
#include <fstream>
#include "common.h"

namespace Riot
{

    class CFile
    {
    public:
        struct TFileData
        {
            CFile*  pFile;
            void*   pData;
            uint    nBytes;
        };

    public:
        // CFile constructor
        CFile();

        // CFile destructor
        ~CFile();
        /***************************************\
        | class methods                         |
        \***************************************/

        void LoadFile( const char* szFilename, const char* szFileMode );

        void ReadBytes( void* szBuffer, uint nNumBytes );

        void WriteBytes( void* pData, uint nNumBytes );

        static void AsyncReadBytes( void* pData, uint nThreadId, uint nStart, uint nCount );
        static void AsyncWriteBytes( void* pData, uint nThreadId, uint nStart, uint nCount );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        char    m_szFilename[ 256 ];
        char    m_szFileMode[8];
        uint    m_nPos;
    };

} // namespace Riot

#endif // #ifndef _FILE_H_
