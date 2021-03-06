/*********************************************************\
File:           File.cpp
Author:         Kyle Weicht
Created:        5/29/2011
Modified:       5/29/2011 1:31:00 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include <fstream>
#include "file.h"
#include <string.h>

namespace Riot
{

    // CFile constructor
    CFile::CFile()
        : m_nPos( 0 )
    {
    }

    // CFile destructor
    CFile::~CFile()
    {
    }
    /***************************************\
    | class methods                         |
    \***************************************/

    void CFile::LoadFile( const char* szFilename, const char* szFileMode )
    {
        strcpy( m_szFilename, szFilename );
        strcpy( m_szFileMode, szFileMode );

        FILE* pf = fopen( szFilename, szFileMode );
        fclose( pf );
    }

    uint CFile::ReadBytes( void* szBuffer, uint nNumBytes )
    {
        uint nTotalRead;
        uint nResult;
        fpos_t pos;
        FILE* pf = fopen( m_szFilename, m_szFileMode );

        pos = m_nPos;
        fsetpos( pf, &pos );

        nTotalRead = fread( szBuffer, 1, nNumBytes, pf );
        ASSERT( nTotalRead != 0 );

        nResult = fgetpos( pf, &pos );

        ASSERT( nResult == 0 );

        m_nPos = pos;

        fclose( pf );
        
        return nTotalRead;
    }

    uint CFile::WriteBytes( void* pData, uint nNumBytes )
    {
        uint nTotalWritten;
        uint nResult;
        fpos_t pos;
        FILE* pf = fopen( m_szFilename, m_szFileMode );

        pos = m_nPos;
        fsetpos( pf, &pos );

        nTotalWritten = fwrite( pData, 1, nNumBytes, pf );
        ASSERT( nTotalWritten != 0 );

        nResult = fgetpos( pf, &pos );

        ASSERT( nResult == 0 );

        m_nPos = pos;

        fclose( pf );
        
        return nTotalWritten;
    }

    void CFile::AsyncReadBytes( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        TFileData* data = (TFileData*)pData;

        data->pFile->ReadBytes( data->pData, data->nBytes );
    }

    void CFile::AsyncWriteBytes( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        TFileData* data = (TFileData*)pData;

        data->pFile->WriteBytes( data->pData, data->nBytes );
    }

} // namespace Riot
