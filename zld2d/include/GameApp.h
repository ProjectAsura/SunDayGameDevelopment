//-----------------------------------------------------------------------------
// File : GameApp.h
// Desc : Game Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxApp.h>
#include <asdxHid.h>
#include <asdxTexture.h>
#include <asdxTarget.h>
#include <Player.h>
#include <SpriteSystem.h>
#include <MapSystem.h>
#include <Hud.h>
//#include <enemy/EnemyTest.h>
#include <EventSystem.h>


///////////////////////////////////////////////////////////////////////////////
// GameApp class
///////////////////////////////////////////////////////////////////////////////
class GameApp : public asdx::Application
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
    GameApp();
    ~GameApp();


private:
    //=========================================================================
    // private variables.
    //=========================================================================
    asdx::GamePad       m_Pad;
    Player              m_Player;
    SpriteSystem        m_Sprite;
    MapSystem           m_MapSystem;
    EventSystem         m_EventSystem;
    //EnemyTest           m_EnemyTest;
    Hud                 m_Hud;
    MapInstance         m_MapData;
    TextWriter          m_TextWriter;

    //=========================================================================
    // private methods.
    //=========================================================================
    bool OnInit();
    void OnTerm();
    void OnFrameMove(asdx::FrameEventArgs& args);
    void OnFrameRender(asdx::FrameEventArgs& args);
    void OnResize(const asdx::ResizeEventArgs& args);
    void OnKey(const asdx::KeyEventArgs& args);
    void OnMouse(const asdx::MouseEventArgs& args);
    void OnTyping(uint32_t keyCode);
};
