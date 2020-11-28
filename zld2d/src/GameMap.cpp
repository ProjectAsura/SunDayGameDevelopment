//-----------------------------------------------------------------------------
// File : GameMap.cpp
// Desc : Game Map.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cassert>
#include <GameMap.h>
#include <Vector2i.h>
#include <TextureHelper.h>
#include <asdxLogger.h>
#include <Gimmick.h>
#include <Player.h>


namespace {

///////////////////////////////////////////////////////////////////////////////
// GameMapPath structure
///////////////////////////////////////////////////////////////////////////////
struct GameMapPath
{
    GAMEMAP_TEXTURE     Type;   // ゲームテクスチャ番号.
    const char*         Path;   // ファイルパス.
};

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const GameMapPath kGameMapTextures[] = {
    { GAMEMAP_TEXTURE_PLANE, "../res/texture/map/plane.tga" },
    { GAMEMAP_TEXTURE_ROCK,  "../res/texture/map/rock.tga" },
    { GAMEMAP_TEXTURE_TREE,  "../res/texture/map/tree.tga" },
    { GAMEMAP_TEXTURE_BLOCK, "../res/texture/map/block.tga" },
};

} // namemap


///////////////////////////////////////////////////////////////////////////////
// GameMap class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
GameMap::GameMap()
{
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
GameMap::~GameMap()
{ ClearGimmicks(); }

//-----------------------------------------------------------------------------
//      タイルを設定します.
//-----------------------------------------------------------------------------
void GameMap::SetTile(uint8_t id, const Tile& tile)
{
    assert(id < kTileTotalCount);
    m_Tile[id] = tile;
}

//-----------------------------------------------------------------------------
//      タイルを取得します.
//-----------------------------------------------------------------------------
Tile GameMap::GetTile(uint8_t id) const
{
    assert(id < kTileTotalCount);
    return m_Tile[id];
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void GameMap::Draw(SpriteSystem& sprite, int playerY)
{
    auto idx = 0;
    for(auto i=0u; i<kTileCountY; ++i)
    for(auto j=0u; j<kTileCountX; ++j)
    {
        auto tile = m_Tile[idx];
        idx++;

        auto pSRV = GetGameMap(tile.TextureId);
        auto step = (float)(j) / kTileCountX;

        auto x = int(kMarginX + kTileSize * j) + m_Scroll.x;
        auto y = int(kMarginY + kTileSize * i) + m_Scroll.y;

        // キャラよりもY座標が下側なら手前に表示されるように調整.
        auto z = (!tile.Moveable && (playerY < y)) ? 0 : 2;

        sprite.Draw(pSRV, x, y, kTileSize, kTileSize, z);
    }

    for(auto& itr : m_Gimmicks)
    {
        auto y = itr->GetBox().Pos.y;

        // キャラよりもY座標が下側なら手前に表示されるように調整.
        auto z = (playerY < y) ? 0 : 2;

        itr->Draw(sprite, z);
    }
}

//-----------------------------------------------------------------------------
//      移動可能かどうかチェックします.
//-----------------------------------------------------------------------------
bool GameMap::CanMove(const Box& nextBox)
{
    auto idx = CalcTileIndex(
        nextBox.Pos.x + nextBox.Size.x / 2,
        nextBox.Pos.y + nextBox.Size.y / 2);
    auto id  = CalcTileId(idx);

    if (m_Tile[id].Changable)
    {
        m_Flags |= GAMEMAP_FLAG_CHANGE;
    }
    else if (m_Tile[id].Scrollable)
    {
        m_Flags |= GAMEMAP_FLAG_SCROLL;
    }

    if (!m_Tile[id].Moveable)
    { return false; }

    for(auto& itr : m_Gimmicks)
    {
        if (!itr->CanMove(nextBox))
        { return false; }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      更新処理を行います.
//-----------------------------------------------------------------------------
void GameMap::Update(UpdateContext& context)
{
    for(auto& itr : m_Gimmicks)
    { itr->Update(context); }

    if (!!(m_Flags & GAMEMAP_FLAG_SCROLL))
    { Scroll(DIRECTION_STATE(context.PlayerDir)); }
    else if (!!(m_Flags & GAMEMAP_FLAG_SCROLL_END))
    { m_Flags &= ~(GAMEMAP_FLAG_SCROLL_END); }
}

//-----------------------------------------------------------------------------
//      ギミックを破棄します.
//-----------------------------------------------------------------------------
void GameMap::ClearGimmicks()
{
    auto itr = m_Gimmicks.begin();
    while(itr != m_Gimmicks.end())
    {
        auto ptr = *itr;
        delete ptr;
        itr++;
    }
    m_Gimmicks.clear();
}

//-----------------------------------------------------------------------------
//      ギミックをリセットします.
//-----------------------------------------------------------------------------
void GameMap::ResetGimmicks()
{
    for(auto& itr : m_Gimmicks)
    { itr->Reset(); }
}

//-----------------------------------------------------------------------------
//      フラグを取得します.
//-----------------------------------------------------------------------------
uint8_t GameMap::GetFlags() const
{ return m_Flags; }

//-----------------------------------------------------------------------------
//      スクロールによるマップ切り替えを行います.
//-----------------------------------------------------------------------------
void GameMap::Scroll(DIRECTION_STATE dir)
{
    static const int kMaxiX = kMarginX + kTileSize * kTileCountX;
    static const int kMaxiY = kMarginY + kTileSize * kTileCountY;

    switch(dir)
    {
    case DIRECTION_LEFT:
        if (m_Scroll.x < kMapMaxiX)
        {
            m_Scroll.x += kMapScrollX;
        }
        else
        {
            m_Scroll.x = 0.0f;
            m_Flags &= ~(GAMEMAP_FLAG_SCROLL);
            m_Flags |= GAMEMAP_FLAG_SCROLL_END;
        }
        break;

    case DIRECTION_RIGHT:
        if (m_Scroll.x > -kMapMaxiX)
        {
            m_Scroll.x -= kMapScrollX;
        }
        else
        {
            m_Scroll.x = 0.0f;
            m_Flags &= ~(GAMEMAP_FLAG_SCROLL);
            m_Flags |= GAMEMAP_FLAG_SCROLL_END;
        }
        break;

    case DIRECTION_UP:
        if (m_Scroll.y < kMapMaxiY)
        {
            m_Scroll.y += kMapScrollY;
        }
        else
        {
            m_Scroll.y = 0.0f;
            m_Flags &= ~(GAMEMAP_FLAG_SCROLL);
            m_Flags |= GAMEMAP_FLAG_SCROLL_END;
        }
        break;

    case DIRECTION_DOWN:
        if (m_Scroll.y > -kMapMaxiY)
        {
            m_Scroll.y -= kMapScrollY;
        }
        else
        {
            m_Scroll.y = 0.0f;
            m_Flags &= ~(GAMEMAP_FLAG_SCROLL);
            m_Flags |= GAMEMAP_FLAG_SCROLL_END;
        }
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// GameMapTextureMgr class
///////////////////////////////////////////////////////////////////////////////
GameMapTextureMgr GameMapTextureMgr::s_Instance;

//-----------------------------------------------------------------------------
//      シングルインスタンスを取得します.
//-----------------------------------------------------------------------------
GameMapTextureMgr& GameMapTextureMgr::Instance()
{ return s_Instance; }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool GameMapTextureMgr::Init()
{
    auto count = _countof(kGameMapTextures);
    m_Textures.resize(count);

    for(auto i=0u; i<count; ++i)
    {
        if (!LoadTexture2D(
            kGameMapTextures[i].Path,
            m_Textures[kGameMapTextures[i].Type]))
        {
            ELOGA("Error : LoadTexture2D() Failed. path = %s", kGameMapTextures[i].Path);
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void GameMapTextureMgr::Term()
{
    for(size_t i=0; i<m_Textures.size(); ++i)
    { m_Textures[i].Release(); }
}

//-----------------------------------------------------------------------------
//      シェーダリソースビューを取得.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* GameMapTextureMgr::GetSRV(uint32_t index) const
{
    assert(index < m_Textures.size());
    return m_Textures[index].GetSRV();
}
