//-----------------------------------------------------------------------------
// File : Player.cpp
// Desc : Player.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

#include <Player.h>
#include <TextureHelper.h>
#include <asdxRenderState.h>
#include <asdxLogger.h>


namespace {

///////////////////////////////////////////////////////////////////////////////
// Vec2i structure
///////////////////////////////////////////////////////////////////////////////
struct Vec2i
{
    int x;
    int y;
};

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const float kOneFrame = 1.0f / 60.0f;

static const char* kTextures[] = {
    "../res/texture/player/PlayerL_0.tga",  // 0
    "../res/texture/player/PlayerL_1.tga",  // 1
    "../res/texture/player/PlayerL_S.tga",  // 2
    "../res/texture/player/PlayerR_0.tga",  // 3
    "../res/texture/player/PlayerR_1.tga",  // 4
    "../res/texture/player/PlayerR_s.tga",  // 5
    "../res/texture/player/PlayerB_0.tga",  // 6
    "../res/texture/player/PlayerB_1.tga",  // 7
    "../res/texture/player/PlayerB_S.tga",  // 8
    "../res/texture/player/PlayerF_0.tga",  // 9
    "../res/texture/player/PlayerF_1.tga",  // 10
    "../res/texture/player/PlayerF_S.tga",  // 11
};

static const Vec2i kTextureSize[] = {
    { 64, 64 },
    { 64, 64 },
    { 128, 64 },
    { 64, 64 },
    { 64, 64 },
    { 128, 64 },
    { 64, 64 },
    { 64, 64 },
    { 64, 128 },
    { 64, 64 },
    { 64, 64 },
    { 64, 128 },
};

static const Vec2i kPosOffset[] = {
    { 0, 0 },
    { 0, 0 },
    { -64, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, -64 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
};

} // namespace


Player::Player()
{
}

Player::~Player()
{
}

bool Player::Init()
{
    for(auto i=0; i<12; ++i)
    {
        if (!LoadTexture2D(kTextures[i], m_Texture[i]))
        { return false; }
    }

    return true;
}

void Player::Term()
{
    for(auto i=0; i<12; ++i)
    { m_Texture[i].Release(); }
}

void Player::Update(asdx::GamePad& pad, float elapsedMsec)
{
    m_AnimLastTime += elapsedMsec;
    m_MoveLastTime += elapsedMsec;

    const int kAnimFrame = 4;
    const int kMoveFrame = 2;

    auto frame = 0;
    if (m_AnimLastTime > kOneFrame * kAnimFrame)
    {
        frame = 1;
        m_AnimLastTime = 0.0f;
    }

    auto seed = 0;
    if (m_MoveLastTime > kOneFrame * kMoveFrame)
    {
        seed = 1;
        m_MoveLastTime = 0.0f;
    }

    auto x = pad.GetNormalizedThumbLX();
    auto y = pad.GetNormalizedThumbLY();
    x = (x > 0) ? 1 : (x < 0) ? -1 : 0;
    y = (y > 0) ? 1 : (y < 0) ? -1 : 0;

    if (frame > 0)
    {
        // 右に進む.
        if (x > 0.0f)
        {
            m_State = (m_State == PLAYER_RIGHT_0) ? PLAYER_RIGHT_1 : PLAYER_RIGHT_0;
        }
        // 左に進む.
        else if (x < 0.0f)
        {
            m_State = (m_State == PLAYER_LEFT_0) ? PLAYER_LEFT_1 : PLAYER_LEFT_0;
        }
        // 上に進む.
        else if (y > 0.0f)
        {
            m_State = (m_State == PLAYER_UP_0) ? PLAYER_UP_1 : PLAYER_UP_0;
        }
        // 下に進む.
        else if (y < 0.0f)
        {
            m_State = (m_State == PLAYER_DOWN_0) ? PLAYER_DOWN_1: PLAYER_DOWN_0;
        }
    }

    if (seed)
    {
        const int kAdvancedPixel = 8;
        if (x > 0.0 || x < 0.0f)
        { m_X += x * kAdvancedPixel; }
        else
        { m_Y -= y * kAdvancedPixel; }
    }

    if (pad.IsPush(asdx::PAD_A))
    {
        if (m_State == PLAYER_LEFT_0 || m_State == PLAYER_LEFT_1)
        { m_State = PLAYER_LEFT_A; }
        else if (m_State == PLAYER_RIGHT_0 || m_State == PLAYER_RIGHT_1)
        { m_State = PLAYER_RIGHT_A; }
        else if (m_State == PLAYER_DOWN_0 || m_State == PLAYER_DOWN_1)
        { m_State = PLAYER_DOWN_A; }
        else if (m_State == PLAYER_UP_0 || m_State == PLAYER_UP_1)
        { m_State = PLAYER_UP_A; }
    }
    else
    {
        if (m_State == PLAYER_LEFT_A)
        { m_State = PLAYER_LEFT_0; }
        else if (m_State == PLAYER_RIGHT_A)
        { m_State = PLAYER_RIGHT_0; }
        else if (m_State == PLAYER_DOWN_A)
        { m_State = PLAYER_DOWN_0; }
        else if (m_State == PLAYER_UP_A)
        { m_State = PLAYER_UP_0; }
    }
}

void Player::Draw(asdx::Sprite* sprite)
{
    auto state = m_State;
    if (state > 12)
    { state = 0; }

    auto pSmp = asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearClamp);
    auto size = kTextureSize[state];
    auto offset = kPosOffset[state];
    sprite->SetTexture(m_Texture[state].GetSRV(), pSmp);
    sprite->Draw(m_X + offset.x, m_Y + offset.y, size.x, size.y);
}