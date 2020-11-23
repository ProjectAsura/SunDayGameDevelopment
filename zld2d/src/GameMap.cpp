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
    { GAMEMAP_TEXTURE_TREE,  "../res/texture/map/tree.tga" }
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
{
}

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
void GameMap::Draw(SpriteSystem& sprite)
{
    auto idx = 0;
    for(auto i=0u; i<kTileCountY; ++i)
    for(auto j=0u; j<kTileCountX; ++j)
    {
        auto tile = m_Tile[idx];
        idx++;

        auto pSRV = GetGameMap(tile.TextureId);

        auto step = (float)(j) / kTileCountX;

        auto x = kMarginX + kTileSize * j;
        auto y = kMarginY + kTileSize * i;
        sprite.Draw(pSRV, x, y, kTileSize, kTileSize);
    }
}

bool GameMap::IsMovable(int& x, int& y, DIRECTION_STATE dir)
{
    auto shiftX = ((x - kMarginX) % kTileSize) != 0;
    auto shiftY = ((y - kMarginY) % kTileSize) != 0;

    // 左上のタイルID.
    auto idxL = CalcTileIndex(x, y);

    // 右下のタイルID.
    auto idxR = CalcTileIndex(x + kTileSize, y + kTileSize);

    auto result = true;

    switch(dir)
    {
    case DIRECTION_LEFT:
        {
            if (!shiftX)
            { idxL.x -= 1; }

            auto id0 = CalcTileId(idxL);
            result = m_Tile[id0].Moveable;
            if (shiftY)
            {
                auto id1 = CalcTileId(Vector2i(idxL.x, idxR.y));
                result &= m_Tile[id1].Moveable;
            }
        }
        break;

    case DIRECTION_RIGHT:
        {
            auto id0 = CalcTileId(Vector2i(idxR.x, idxL.y));
            result = m_Tile[id0].Moveable;
            if (shiftY)
            {
                auto id1 = CalcTileId(idxR);
                result &= m_Tile[id1].Moveable;
            }
        }
        break;

    case DIRECTION_UP:
        {
            if (!shiftY)
            { idxL.y -= 1; }

            auto id0 = CalcTileId(idxL);
            result = m_Tile[id0].Moveable;
            if (shiftX)
            {
                auto id1 = CalcTileId(Vector2i(idxR.x, idxL.y));
                result &= m_Tile[id1].Moveable;
            }
        }
        break;

    case DIRECTION_DOWN:
        {
            auto id0 = CalcTileId(Vector2i(idxL.x, idxR.y));
            result = m_Tile[id0].Moveable;
            if (shiftX)
            {
                auto id1 = CalcTileId(idxR);
                result &= m_Tile[id1].Moveable;
            }
        }
        break;
    }

    return result;
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
