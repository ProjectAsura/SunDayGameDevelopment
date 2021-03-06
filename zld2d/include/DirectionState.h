﻿//-----------------------------------------------------------------------------
// File : DirectionState.h
// Desc : Direction State.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Vector2i.h>
#include <asdxMath.h>


///////////////////////////////////////////////////////////////////////////////
// DIRECTION_STATE
///////////////////////////////////////////////////////////////////////////////
enum DIRECTION_STATE : uint8_t
{
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_NONE,
};

//-----------------------------------------------------------------------------
//      移動方向を取得します.
//-----------------------------------------------------------------------------
inline Vector2i GetMoveDir(DIRECTION_STATE state)
{
    static const Vector2i kDir[] = {
        Vector2i(-1,  0),
        Vector2i( 1,  0),
        Vector2i( 0, -1),   // 逆向きなので注意.
        Vector2i( 0,  1),   // 逆向きなので注意.
        Vector2i( 0,  0),
    };

    return kDir[state];
}

//-----------------------------------------------------------------------------
//      移動方向を決定するためにサイコロを振ります.
//-----------------------------------------------------------------------------
inline DIRECTION_STATE RollDice(asdx::PCG& random)
{
    auto rnd = 0u;
    while(true)
    {
        rnd = random.GetAsU32();
        // 4の倍数でないものは，偏るため振り直す.
        if (rnd <= 4294967292)
        { break; }
    }

    return DIRECTION_STATE(rnd % 4);
}