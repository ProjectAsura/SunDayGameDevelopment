//-----------------------------------------------------------------------------
// File : Hud.cpp
// Desc : Head Up Display.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Hud.h>
#include <TextureHelper.h>
#include <asdxLogger.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const int kLifeOffsetX = 32;
static const int kLifeOffsetY = 8;
static const int kLifeSize    = 32;

static const char* kTextures[] = {
    "../res/texture/hud/star_full.tga",
    "../res/texture/hud/star_lack.tga",
    "../res/texture/hud/white.tga",
    "../res/texture/hud/wnd.tga",
};


} // namespace


///////////////////////////////////////////////////////////////////////////////
// HUD class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
Hud::Hud()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
Hud::~Hud()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool Hud::Init()
{
    for(auto i=0; i<_countof(kTextures); ++i)
    {
        if (!LoadTexture2D(kTextures[i], m_Texture[i]))
        {
            ELOGA("Error : Texture Load Failed. index = %d", i);
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void Hud::Term()
{
    for(auto i=0; i<_countof(kTextures); ++i)
    { m_Texture[i].Release(); }
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Hud::Draw(SpriteSystem& sprite, const Player& player)
{
    auto curLife = player.GetLife();
    auto maxLife = player.GetMaxLife(); // MAX 10まで.

    for(auto i=1; i<=maxLife; ++i)
    {
        if (i <= curLife)
        {
            sprite.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
            sprite.Draw(
                m_Texture[HUD_TEXTURE_LIFE_FULL].GetSRV(),
                kLifeOffsetX + kLifeSize * (i - 1),
                kLifeOffsetY,
                kLifeSize,
                kLifeSize);
        }
        else
        {
            sprite.SetColor(1.0f, 1.0f, 1.0f, 0.75f);
            sprite.Draw(
                m_Texture[HUD_TEXTURE_LIFE_LACK].GetSRV(),
                kLifeOffsetX + kLifeSize * (i - 1),
                kLifeOffsetY,
                kLifeSize,
                kLifeSize);
        }
    }

}

//-----------------------------------------------------------------------------
//      メッセージウィンドウの枠を描画します.
//-----------------------------------------------------------------------------
void Hud::DrawWnd(SpriteSystem& sprite, bool upper)
{
    static const int kW = 1000;
    static const int kH = 200;

    static const int kX = 140;
    int kY = (upper) ? 64 : 488;

    sprite.SetColor(0.75f, 0.75f, 0.75f, 0.75f);
    sprite.Draw(
        m_Texture[HUD_TEXTURE_WINDOW].GetSRV(),
        kX, kY, kW, kH, 0);
}