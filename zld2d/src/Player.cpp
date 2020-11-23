//-----------------------------------------------------------------------------
// File : Player.cpp
// Desc : Player.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Player.h>
#include <TextureHelper.h>
#include <asdxRenderState.h>
#include <asdxLogger.h>
#include <asdxHid.h>
#include <GameMap.h>
#include <Vector2i.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const float  kOneFrame       = 1.0f / 60.0f;
static const int    kAnimFrame      = 4;
static const int    kAdvancedPixel  = 8;
static const int    kSize           = 64;

// キャラテクスチャ.
static const char* kPlayerTextures[] = {
    "../res/texture/player/PlayerL_0.tga",  // 0
    "../res/texture/player/PlayerL_1.tga",  // 1
    "../res/texture/player/PlayerL_A.tga",  // 2
    "../res/texture/player/PlayerR_0.tga",  // 3
    "../res/texture/player/PlayerR_1.tga",  // 4
    "../res/texture/player/PlayerR_A.tga",  // 5
    "../res/texture/player/PlayerB_0.tga",  // 6
    "../res/texture/player/PlayerB_1.tga",  // 7
    "../res/texture/player/PlayerB_A.tga",  // 8
    "../res/texture/player/PlayerF_0.tga",  // 9
    "../res/texture/player/PlayerF_1.tga",  // 10
    "../res/texture/player/PlayerF_A.tga",  // 11
};

// 武器テクスチャ.
static const char* kSpearTextures[] = {
    "../res/texture/player/Spear_L.tga",
    "../res/texture/player/Spear_R.tga",
    "../res/texture/player/Spear_B.tga",
    "../res/texture/player/Spear_F.tga"
};

// 武器表示オフセット.
static const Vector2i kOffset[] = {
    { -64,  0 },
    {  64,  0 },
    {  0, -64 },
    {  0,  64 }
};

//-----------------------------------------------------------------------------
//      テクスチャ番号取得.
//-----------------------------------------------------------------------------
int GetPlayerId(int state, int dir, int frame)
{
    int result = dir * 3;
    if (state == PLAYER_ACTION_NONE)
    { result += frame; }
    else if (state == PLAYER_ACTION_ATTACK)
    { result += 2; }

    return result;
}


} // namespace

///////////////////////////////////////////////////////////////////////////////
// Player class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
Player::Player()
{
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
Player::~Player()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool Player::Init()
{
    for(auto i=0; i<12; ++i)
    {
        if (!LoadTexture2D(kPlayerTextures[i], m_PlayerTexture[i]))
        { return false; }
    }

    for(auto i=0; i<4; ++i)
    {
        if (!LoadTexture2D(kSpearTextures[i], m_WeaponTexture[i]))
        { return false; }
    }

    m_X = kTileSize * 8;
    m_Y = kTileSize * 5;

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void Player::Term()
{
    for(auto i=0; i<12; ++i)
    { m_PlayerTexture[i].Release(); }

    for(auto i=0; i<4; ++i)
    { m_WeaponTexture[i].Release(); }
}

//-----------------------------------------------------------------------------
//      更新処理を行います.
//-----------------------------------------------------------------------------
void Player::Update(UpdateContext& context)
{
    m_AnimLastTime += context.ElapsedMsec;

    auto next = false;

    // 1フレーム分進んだかどうかチェック.
    if (m_AnimLastTime >= kOneFrame * kAnimFrame)
    {
        m_AnimLastTime = 0.0f;
        next = true;
    }

    int x = 0;
    int y = 0;
    {
        // 右に進む.
        if (context.Pad->IsPush(asdx::PAD_RIGHT))
        {
            m_Direction = DIRECTION_RIGHT;
            x = 1;
        }
        // 左に進む.
        else if (context.Pad->IsPush(asdx::PAD_LEFT))
        {
            m_Direction = DIRECTION_LEFT;
            x = -1;
        }
        // 上に進む.
        else if (context.Pad->IsPush(asdx::PAD_UP))
        {
            m_Direction = DIRECTION_UP;
            y = 1;
        }
        // 下に進む.
        else if (context.Pad->IsPush(asdx::PAD_DOWN))
        {
            m_Direction = DIRECTION_DOWN;
            y = -1;
        }

        if ((abs(x) > 0 || abs(y) > 0) && next)
        { m_AnimFrame = (m_AnimFrame + 1) & 0x1; }
    }

    if (context.Pad->IsDown(asdx::PAD_A))
    {
        m_Action = PLAYER_ACTION_ATTACK;
        m_AnimLastTime = 0.0f;
    }
    else if (next)
    {
        m_Action = PLAYER_ACTION_NONE;
    }

    // 隣のタイルに移動できるかどうかチェック.
    if (context.Map->IsMovable(m_X, m_Y, m_Direction))
    {
        if (m_Action == PLAYER_ACTION_NONE)
        {
            if (x != 0)
            { m_X += int(x * kAdvancedPixel); }
            else if (y != 0)
            { m_Y -= int(y * kAdvancedPixel); }
        }
    }

}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Player::Draw(SpriteSystem& sprite)
{
    sprite.SetColor(1.0f, 1.0f, 1.0f, 1.0f);


    // キャラ描画
    {
        auto id   = GetPlayerId(m_Action, m_Direction, m_AnimFrame);
        auto pSRV = m_PlayerTexture[id].GetSRV();
        sprite.Draw(pSRV, m_X, m_Y, kSize, kSize);
    }

    // 武器描画.
    if (m_Action == PLAYER_ACTION_ATTACK)
    {
        auto offset = kOffset[m_Direction];
        auto pSRV   = m_WeaponTexture[m_Direction].GetSRV();
        sprite.Draw(pSRV, m_X + offset.x, m_Y + offset.y, kSize, kSize);
    }
}