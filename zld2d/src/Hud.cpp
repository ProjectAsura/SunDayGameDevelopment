//-----------------------------------------------------------------------------
// File : Hud.cpp
// Desc : Head Up Display.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Hud.h>
#include <asdxLogger.h>
#include <TextureId.h>
#include <TextureMgr.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const int kLifeOffsetX = 32;
static const int kLifeOffsetY = 8;
static const int kLifeSize    = 32;

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
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Hud::Draw(SpriteSystem& sprite, const Player& player)
{
    // ライフを描画.
    DrawLife(sprite, player.GetLife(), player.GetMaxLife());
}

//-----------------------------------------------------------------------------
//      ライフを描画します.
//-----------------------------------------------------------------------------
void Hud::DrawLife(SpriteSystem& sprite, uint8_t curLife, uint8_t maxLife)
{
    for(auto i=1; i<=maxLife; ++i)
    {
        if (i <= curLife)
        {
            sprite.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
            sprite.Draw(
                GetTexture(TEXTURE_HUD_LIFE_FULL),
                kLifeOffsetX + kLifeSize * (i - 1),
                kLifeOffsetY,
                kLifeSize,
                kLifeSize);
        }
        else
        {
            sprite.SetColor(1.0f, 1.0f, 1.0f, 0.75f);
            sprite.Draw(
                GetTexture(TEXTURE_HUD_LIFE_LACK),
                kLifeOffsetX + kLifeSize * (i - 1),
                kLifeOffsetY,
                kLifeSize,
                kLifeSize);
        }
    }
}

