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
#include <asdxSprite.h>
#include <asdxTexture.h>
#include <Player.h>


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
    asdx::Sprite*       m_Sprite        = nullptr;
    asdx::Texture2D     m_Texture;
    Player              m_Player;

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
