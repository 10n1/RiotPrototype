/*********************************************************\
File:           todo.h
Purpose:        
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/8/2011 9:56:01 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TODO_H_
#define _TODO_H_

    Deferred renderer
    asset pipeline
    defining materials by scripts
    TCP/IP abstraction
    Server
    client
    audio
    rigid body physics
    OS X port of client and server
    Linux port of client and server
    UI button
    UI label
    OpenGL implementation
    3D button
    3D text label
    File IO abstraction
        Directory crawling
        loading all files in a directory
        asyncronous loading/saving
    frustrum culling
    function timing/profiling
    CPU utilization chart
    the Machine
    inventory
    loot
    pets
    diffs
    building system
    enemy teams
    team leader AI
    individulal unit AI
    multiplayer
    picking
    key binding
    game state machine
        paused
        editor
    particle system
    multiple camera support
    logging
    logging to a file
    tutorial
    console

    We need to come up with a way to handle array positions changing (when we remove an object from the middle of an array)
    determine right way to flip the frame buffer when drawing a full screen buffer

    Determine how to get various "global" data
        Window width/height

    Determine where the window Origin is (top left {Win32} or bottom left {UNIX}? )
        Come up with way to convert everything to this system

    Need to come up with a calling convention
        For creation functions, do we pass in objects like DirectX does and return a status?

#endif // #ifndef _TODO_H_
