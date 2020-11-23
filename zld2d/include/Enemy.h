﻿//-----------------------------------------------------------------------------
// File : Enemy.h
// Desc : Enemy.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <asdxTexture.h>
#include <DirectionState.h>
#include <UpdateContext.h>
#include <SpriteSystem.h>
#include <Box.h>


///////////////////////////////////////////////////////////////////////////////
// Enemy class
///////////////////////////////////////////////////////////////////////////////
class Enemy
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

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    Enemy();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~Enemy();

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //-------------------------------------------------------------------------
    bool Init();

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      更新処理を行います.
    //-------------------------------------------------------------------------
    void Update(UpdateContext& context);

    //-------------------------------------------------------------------------
    //! @brief      描画処理を行います.
    //-------------------------------------------------------------------------
    void Draw(SpriteSystem& sprite);

protected:
    //-------------------------------------------------------------------------
    //! @brief      ランダムウォークします.
    //-------------------------------------------------------------------------
    void RandomWalk(int changeFrame);

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    int             m_State     = 0;
    int             m_Life      = 1;
    Box             m_Box       = {};
    DIRECTION_STATE m_Direction = DIRECTION_LEFT;
    uint8_t         m_Frame     = 0;
    asdx::Texture2D m_Texture[8];

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};