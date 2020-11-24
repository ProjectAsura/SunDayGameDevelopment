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
static const int kLifeOffsetX = 2;
static const int kLifeOffsetY = 2;
static const int kLifeSize    = 32;

static const char* kTextures[] = {
    "../res/texture/hud/star_full.tga",
    "../res/texture/hud/star_lack.tga"
};

///////////////////////////////////////////////////////////////////////////////
// HUD_TEXTURE enum
///////////////////////////////////////////////////////////////////////////////
enum HUD_TEXTURE
{
    HUD_TEXTURE_LIFE_FULL,  // 満☆
    HUD_TEXTURE_LIFE_LACK,  // 欠☆
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
    for(auto i=0; i<2; ++i)
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
    for(auto i=0; i<2; ++i)
    { m_Texture[i].Release(); }
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Hud::Draw(SpriteSystem& sprite, const Player& player)
{
    auto curLife = player.GetLife();
    auto maxLife = player.GetMaxLife();

    for(auto i=1; i<=maxLife; ++i)
    {
        if (i <= curLife)
        {
            sprite.Draw(
                m_Texture[HUD_TEXTURE_LIFE_FULL].GetSRV(),
                kLifeOffsetX + kLifeSize * (i - 1),
                kLifeOffsetY,
                kLifeSize,
                kLifeSize);
        }
        else
        {
            sprite.Draw(
                m_Texture[HUD_TEXTURE_LIFE_LACK].GetSRV(),
                kLifeOffsetX + kLifeSize * (i - 1),
                kLifeOffsetY,
                kLifeSize,
                kLifeSize);
        }
    }

}