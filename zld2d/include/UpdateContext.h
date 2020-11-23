//-----------------------------------------------------------------------------
// File : UpdateContext.h
// Desc : Update Context.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once


namespace asdx {
class GamePad;
};

class GameMap;

struct UpdateContext
{
    asdx::GamePad*  Pad;
    float           ElapsedMsec;
    GameMap*        Map;
};