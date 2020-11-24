//-----------------------------------------------------------------------------
// File : Box.h
// Desc : Bounding Box.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Vector2i.h>


///////////////////////////////////////////////////////////////////////////////
// Box structure
///////////////////////////////////////////////////////////////////////////////
struct Box
{
    Vector2i Pos;   // 左上原点.
    Vector2i Size;  // サイズ.

    Box()
    { /* DO_NOTHING */ }

    Box(int x, int y, int w, int h)
    : Pos   (x, y)
    , Size  (w, h)
    { /* DO_NOTHING */ }

    Box(const Vector2i& p, const Vector2i& s)
    : Pos   (p)
    , Size  (s)
    { /* DO_NOTHING */ }
};

//-----------------------------------------------------------------------------
//      衝突判定を行います.
//-----------------------------------------------------------------------------
inline bool IsHit(const Box& lhs, const Box& rhs)
{
    return lhs.Pos.x < (rhs.Pos.x + rhs.Size.x)
        && rhs.Pos.x < (lhs.Pos.x + lhs.Size.x)
        && lhs.Pos.y < (rhs.Pos.y + rhs.Size.y)
        && rhs.Pos.y < (lhs.Pos.y + lhs.Size.y);
}

