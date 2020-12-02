//-----------------------------------------------------------------------------
// File : UpdateContext.h
// Desc : Update Context.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Forward Declarations.
//-----------------------------------------------------------------------------
namespace asdx {
class GamePad;
};
class  MapSystem;
struct Box;


///////////////////////////////////////////////////////////////////////////////
// UpdateContext structure
///////////////////////////////////////////////////////////////////////////////
struct UpdateContext
{
    asdx::GamePad*  Pad;            //!< ゲームパッド.
    float           ElapsedSec;     //!< 前フレームからの経過時間.
    MapSystem*      Map;            //!< マップシステム.
    Box*            BoxYellow;      //!< 黄玉.
    Box*            BoxRed;         //!< 赤玉.
    uint8_t         PlayerDir;      //!< プレイヤーが向いている方向.
};