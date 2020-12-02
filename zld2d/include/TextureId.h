//-----------------------------------------------------------------------------
// File : TextureId.h
// Desc : Texture ID.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

///////////////////////////////////////////////////////////////////////////////
// TEXTURE_ID enum
///////////////////////////////////////////////////////////////////////////////
enum TEXTURE_ID : unsigned int
{
    // 単色系.
    TEXTURE_WHITE,      // 無地.

    // マップ系.
    TEXTURE_MAP_ROCK,   // マップ-岩.
    TEXTURE_MAP_TREE,   // マップ-木.
    TEXTURE_MAP_BLOCK,  // マップ-ブロック.

    // HUD系.
    TEXTURE_HUD_LIFE_FULL,  // HUD-満☆.
    TEXTURE_HUD_LIFE_LACK,  // HUD-欠☆.
    TEXTURE_HUD_WINDOW,     // HUD-メッセージウィンドウ.
};
