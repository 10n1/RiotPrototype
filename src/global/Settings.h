/*********************************************************\
File:           Settings.h
Purpose:        Holds all the "global" game settings that
                can be modified in game via the console
Author:         Kyle Weicht
Created:        4/27/2011
Modified:       4/28/2011 10:38:04 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SETTINGS_H_
#define _SETTINGS_H_
#include "types.h"
#include "config.h"

namespace Riot
{
    extern bool gs_bShowBoundingVolumes;    // Renders the bounding volumes
    extern bool gs_bRenderWireframe;        // Renders everything wireframe

} // namespace Riot

#endif // #ifndef _SETTINGS_H_
