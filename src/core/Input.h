/*********************************************************\
File:           Input.h
Purpose:        
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       5/1/2011 6:47:22 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_

namespace Riot
{    
    enum Mouse
    {
        MOUSE_L_BUTTON = 0,
        MOUSE_R_BUTTON = 1,
        MOUSE_M_BUTTON = 2,
    };

    enum Keyboard
    {
#ifdef OS_WINDOWS
        // Utility keys
        KEY_ESCAPE      = 0x1B, // VK_ESCAPE
        KEY_ENTER       = 0x0D,
        KEY_SHIFT       = 0x10,
        KEY_BACKSPACE   = 0x08,

        // Arrows
        KEY_LEFT    = 0x25, // VK_LEFT 
        KEY_RIGHT   = 0x27, // VK_RIGHT
        KEY_UP      = 0x26, // VK_UP   
        KEY_DOWN    = 0x28, // VK_DOWN 
        
        // Letters
        KEY_A = 'A',
        KEY_B = 'B',
        KEY_C = 'C',
        KEY_D = 'D',
        KEY_E = 'E',
        KEY_F = 'F',
        KEY_G = 'G',
        KEY_H = 'H',
        KEY_I = 'I',
        KEY_J = 'J',
        KEY_K = 'K',
        KEY_L = 'L',
        KEY_M = 'M',
        KEY_N = 'N',
        KEY_O = 'O',
        KEY_P = 'P',
        KEY_Q = 'Q',
        KEY_R = 'R',
        KEY_S = 'S',
        KEY_T = 'T',
        KEY_U = 'U',
        KEY_V = 'V',
        KEY_W = 'W',
        KEY_X = 'X',
        KEY_Y = 'Y',
        KEY_Z = 'Z',
        KEY_SPACE = ' ',
        KEY_TILDE = 192,

#elif defined( OS_OSX ) // Defined in NSEvent.h
        // Utility keys
        KEY_ESCAPE  = '\e',
        KEY_ENTER   = 0x0003,
        KEY_SHIFT   = 0xFFFF,
        
        // Arrows
        KEY_LEFT    = 0x02,
        KEY_RIGHT   = 0x03,
        KEY_UP      = 0x0, 
        KEY_DOWN    = 0x01,
        
        // Letters
        KEY_A = 'a',
        KEY_B = 'b',
        KEY_C = 'c',
        KEY_D = 'd',
        KEY_E = 'e',
        KEY_F = 'f',
        KEY_G = 'g',
        KEY_H = 'h',
        KEY_I = 'i',
        KEY_J = 'j',
        KEY_K = 'k',
        KEY_L = 'l',
        KEY_M = 'm',
        KEY_N = 'n',
        KEY_O = 'o',
        KEY_P = 'p',
        KEY_Q = 'q',
        KEY_R = 'r',
        KEY_S = 's',
        KEY_T = 't',
        KEY_U = 'u',
        KEY_V = 'v',
        KEY_W = 'w',
        KEY_X = 'x',
        KEY_Y = 'y',
        KEY_Z = 'z',
        KEY_SPACE = ' ',
#else
#endif


    };

} //namespace Riot

#endif // #ifndef _INPUT_H_
