﻿//-----------------------------------------------------------------------------
// File : Enemy.cpp
// Desc : Enemy.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Enemy.h>
#include <asdxMath.h>



///////////////////////////////////////////////////////////////////////////////
// Enemy class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
Enemy::Enemy()
: m_Random(1234567)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
Enemy::~Enemy()
{ /* DO_NOTHING */  }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool Enemy::Init()
{ return true; }

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void Enemy::Term()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      更新処理を行います.
//-----------------------------------------------------------------------------
void Enemy::Update(UpdateContext& context)
{ UpdateFrame(); }

//-----------------------------------------------------------------------------
//      フレーム番号を更新します.
//-----------------------------------------------------------------------------
void Enemy::UpdateFrame()
{
    if (int(m_Frame) + 1 > 255)
    { m_Frame = 0; }
    else
    { m_Frame++; }
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Enemy::Draw(SpriteSystem& sprite)
{ /* DO_NOTHING */ }
