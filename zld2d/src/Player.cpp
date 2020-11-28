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
static const int    kNonDamageFrame = 180;
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
: m_Box     (0, 0, kSize, kSize)
, m_HitBox  (0, 0, kSize, kSize)
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

    SetTilePos(3, 5);

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
    m_AnimLastTime += context.ElapsedSec;

    auto next = false;

    // 1フレーム分進んだかどうかチェック.
    if (m_AnimLastTime >= kOneFrame * kAnimFrame)
    {
        m_AnimLastTime = 0.0f;
        next = true;
    }

    int x = 0;
    int y = 0;
    if ((context.Map->GetFlags() & GAMEMAP_FLAG_SCROLL) == 0)
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

        if ((x != 0 || y != 0) && next)
        { m_AnimFrame = (m_AnimFrame + 1) & 0x1; }
    }

    if (context.Pad->IsDown(asdx::PAD_A))
    {
        auto offset = kOffset[m_Direction];

        // 攻撃判定範囲を更新.
        m_HitBox.Pos.x  = m_Box.Pos.x + offset.x;
        m_HitBox.Pos.y  = m_Box.Pos.y + offset.y;

        m_Action        = PLAYER_ACTION_ATTACK;
        m_AnimLastTime  = 0.0f;

        // 攻撃判定を設定.
        context.HitBox = &m_HitBox;
    }
    else if (next)
    {
        m_Action = PLAYER_ACTION_NONE;
    }

    auto box = m_Box;
    box.Pos.x += int(x * kAdvancedPixel);
    box.Pos.y -= int(y * kAdvancedPixel);

    // 隣のタイルに移動できるかどうかチェック.
    if (context.Map->CanMove(box))
    {
        if (m_Action == PLAYER_ACTION_NONE)
        { m_Box.Pos = box.Pos; }
    }

#if 1
    // Debug.
    if (context.Pad->IsDown(asdx::PAD_X))
    {
        // リスポーン.
        m_Life = m_MaxLife;
    }

    if (context.Pad->IsDown(asdx::PAD_Y))
    {
        context.Map->ResetGimmicks();
    }
#endif

    if (m_NonDamageFrame == 0)
    { context.DamageBox  = &m_Box; }
    else if (m_NonDamageFrame > 0)
    { m_NonDamageFrame--; }

    context.PlayerDir = m_Direction;

    if (!!(context.Map->GetFlags() & GAMEMAP_FLAG_SCROLL))
    {
        if (next)
        { m_AnimFrame = (m_AnimFrame + 1) & 0x1; }
        Scroll();
    }
    else if (!!(context.Map->GetFlags() & GAMEMAP_FLAG_SCROLL_END))
    {
        m_Scroll.x  = 0;
        m_Scroll.y  = 0;
        m_AnimFrame = 0;

        switch(m_Direction)
        {
        case DIRECTION_LEFT:
            m_Box.Pos.x = kMapMaxiX - kTileSize;
            break;

        case DIRECTION_RIGHT:
            m_Box.Pos.x = kMarginX + kTileSize;
            break;

        case DIRECTION_UP:
            m_Box.Pos.y = kMapMaxiY - kTileSize;
            break;

        case DIRECTION_DOWN:
            m_Box.Pos.y = kMarginY + kTileSize;
            break;
        }
    }
}

//-----------------------------------------------------------------------------
//      ダメージを設定します.
//-----------------------------------------------------------------------------
bool Player::ReceiveDamage()
{
    if (m_Life > 0)
    {
        m_Life--;
        m_NonDamageFrame = kNonDamageFrame;
    }

    return (m_Life == 0);
}

//-----------------------------------------------------------------------------
//      スポーンします.
//-----------------------------------------------------------------------------
void Player::Spawn(int tileX, int tileY, bool resetLife)
{
    SetTilePos(tileX, tileY);

    if (resetLife)
    { m_Life = m_MaxLife; }
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Player::Draw(SpriteSystem& sprite)
{
    if (m_Life == 0)
    { return; }

    sprite.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    // キャラ描画
    if (m_NonDamageFrame % 3 == 0)
    {
        auto id   = GetPlayerId(m_Action, m_Direction, m_AnimFrame);
        auto pSRV = m_PlayerTexture[id].GetSRV();
        auto box  = m_Box;
        box.Pos += m_Scroll;

        sprite.Draw(pSRV, box, 1);
    }

    // 武器描画.
    if (m_Action == PLAYER_ACTION_ATTACK)
    {
        auto pSRV   = m_WeaponTexture[m_Direction].GetSRV();
        sprite.Draw(pSRV, m_HitBox, 1);
    }
}

//-----------------------------------------------------------------------------
//      スクロールによるマップ切り替えを行います.
//-----------------------------------------------------------------------------
void Player::Scroll()
{
    switch(m_Direction)
    {
    case DIRECTION_LEFT:
        if (m_Scroll.x < kMapMaxiX)
        {
            m_Scroll.x += kCharaScrollX;
        }
        break;

    case DIRECTION_RIGHT:
        if (m_Scroll.x > -kMapMaxiX)
        {
            m_Scroll.x -= kCharaScrollX;
        }
        break;

    case DIRECTION_UP:
        if (m_Scroll.y < kMapMaxiY)
        {
            m_Scroll.y += kCharaScrollY;
        }
        break;


    case DIRECTION_DOWN:
        if (-m_Scroll.y < kMapMaxiY)
        {
            m_Scroll.y -= kCharaScrollY;
        }
        break;
    }
}

//-----------------------------------------------------------------------------
//      タイル位置を設定します.
//-----------------------------------------------------------------------------
void Player::SetTilePos(int tileX, int tileY)
{
    m_Box.Pos.x = kMarginX + tileX * kTileSize;
    m_Box.Pos.y = kMarginY + tileY * kTileSize;

    if (m_Box.Pos.x > kMapMaxiX)
    { m_Box.Pos.x = kMapMaxiX; }
    else if (m_Box.Pos.x < kMarginX)
    { m_Box.Pos.x = kMarginX; }

    if (m_Box.Pos.y > kMapMaxiY)
    { m_Box.Pos.y = kMapMaxiY; }
    else if (m_Box.Pos.y < kMarginY)
    { m_Box.Pos.y = kMarginY; }
}