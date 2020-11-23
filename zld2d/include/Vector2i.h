//-----------------------------------------------------------------------------
// File : Vector2i.h
// Desc : Int Vector2.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxMath.h>


///////////////////////////////////////////////////////////////////////////////
// Vector2i structure
///////////////////////////////////////////////////////////////////////////////
struct Vector2i
{
    int x;
    int y;

    Vector2i()
    : x(0), y(0)
    {}

    Vector2i(int nx, int ny)
    : x(nx), y(ny)
    {}

    Vector2i operator + (const Vector2i& value) const
    { return Vector2i(x + value.x, y + value.y); }

    Vector2i operator - (const Vector2i& value) const
    { return Vector2i(x - value.x, y - value.y); }

    Vector2i operator * (int scale) const
    { return Vector2i(x * scale, y * scale); }

    Vector2i& operator += (const Vector2i& value)
    {
        x += value.x;
        y += value.y;
        return *this;
    }

    Vector2i& operator -= (const Vector2i& value)
    {
        x -= value.x;
        y -= value.y;
        return *this;
    }

    Vector2i& operator *= (int scale)
    {
        x *= scale;
        y *= scale;
        return *this;
    }

    static Vector2i Clamp(const Vector2i& value, const Vector2i& mini, const Vector2i& maxi)
    {
        return Vector2i(
            asdx::Clamp<int>(value.x, mini.x, maxi.x),
            asdx::Clamp<int>(value.y, mini.y, maxi.y));
    }

    static Vector2i Max(const Vector2i& lhs, const Vector2i &rhs)
    {
        return Vector2i(
            asdx::Max<int>(lhs.x, rhs.x),
            asdx::Max<int>(lhs.y, rhs.y));
    }

    static Vector2i Min(const Vector2i& lhs, const Vector2i& rhs)
    {
        return Vector2i(
            asdx::Min<int>(lhs.x, rhs.x),
            asdx::Min<int>(lhs.y, rhs.y));
    }
};
