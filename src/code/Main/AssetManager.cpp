/*********************************************************\
File:           AssetManager.cpp
Author:         Kyle Weicht
Created:        3/31/2011
Modified:       3/31/2011 1:39:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "AssetManager.h"

#include <memory>

// CAssetManager constructor
CAssetManager::CAssetManager()
{
    memset( m_nAssetNames, 0, sizeof( m_nAssetNames ) );
    memset( m_pAssets, 0, sizeof( m_pAssets ) );
}

// CAssetManager destructor
CAssetManager::~CAssetManager()
{
}
/***************************************\
| class methods                         |
\***************************************/

//-----------------------------------------------------------------------------
//  GetInstance
//  Singleton protection
//-----------------------------------------------------------------------------
CAssetManager* CAssetManager::GetInstance( void )
{
    static CAssetManager pInstance;
    return &pInstance;
}

//-----------------------------------------------------------------------------
//  GetAsset
//  Loads the asset and returns its bytecode, or returns the already-loaded asset
//-----------------------------------------------------------------------------
void* CAssetManager::GetAsset( const char* szFilename, uint& nExists )
{
    return 0;
}
