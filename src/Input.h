/*********************************************************\
File:           Input.h
Purpose:        
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 5:28:31 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_

namespace Riot
{

    enum Keyboard
    {
        // Utility keys
        KEY_ESCAPE = 0x1B, // VK_ESCAPE

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


    };

} //namespace Riot

#endif // #ifndef _INPUT_H_
