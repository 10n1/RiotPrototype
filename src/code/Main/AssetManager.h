/*********************************************************\
File:           AssetManager.h
Purpose:        
Author:         Kyle Weicht
Created:        3/31/2011
Modified:       3/31/2011 1:39:26 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_
#include "Common.h"

#define MAX_ASSETS 128

class CAssetManager
{
public:
    // CAssetManager constructor
    CAssetManager();

    // CAssetManager destructor
    ~CAssetManager();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  GetInstance
    //  Singleton protection
    //-----------------------------------------------------------------------------
    static CAssetManager* GetInstance( void );

    
    //-----------------------------------------------------------------------------
    //  GetAsset
    //  Loads the asset and returns its bytecode, or returns the already-loaded asset
    //-----------------------------------------------------------------------------
    void* GetAsset( const char* szFilename, uint& nExists );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    uint64  m_nAssetNames[MAX_ASSETS];
    pvoid   m_pAssets[MAX_ASSETS];
};


#endif // #ifndef _ASSETMANAGER_H_
