﻿//-----------------------------------------------------------------------------
// File : Hud.h
// Desc : Head Up Display.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SpriteSystem.h>
#include <Player.h>


///////////////////////////////////////////////////////////////////////////////
// Hud class
///////////////////////////////////////////////////////////////////////////////
class Hud
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================
    Hud();
    ~Hud();
    void Draw(SpriteSystem& sprite, const Player& player);

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // private methods.
    //=========================================================================
    void DrawLife(SpriteSystem& sprite, uint8_t curLife, uint8_t maxLife);
};