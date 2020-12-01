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
#include <MessageId.h>


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
// GameMapData structure
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      ロード処理を行います.
//-----------------------------------------------------------------------------
bool GameMapData::Load(const char* path)
{
    // TODO : Implementation.
    return true;
}

//-----------------------------------------------------------------------------
//      セーブ処理を行います.
//-----------------------------------------------------------------------------
bool GameMapData::Save(const char* path)
{
    // TODO : Implmenetation.
    return true;
}

//-----------------------------------------------------------------------------
//      破棄処理を行います.
//-----------------------------------------------------------------------------
void GameMapData::Dispose()
{
    // TODO : Implementation.
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void GameMapData::Draw(SpriteSystem& sprite, int offsetX, int offsetY)
{
    // この描画処理はマップ切り替えでスクロールインするための専用処理です.
    auto idx = 0;
    for(auto i=0u; i<kTileCountY; ++i)
    for(auto j=0u; j<kTileCountX; ++j)
    {
        auto tile = Tile[idx];
        idx++;

        auto pSRV = GetGameMap(tile.TextureId);
        auto x = int(offsetX + kTileSize * j);
        auto y = int(offsetY + kTileSize * i);

        sprite.Draw(pSRV, x, y, kTileSize, kTileSize, 2);
    }

    // ギミックはオフセットを考慮した配置になっているので，
    // スクロールさせるためにオフセット分を差し引く必要あり.
    for(auto& itr : Gimmicks)
    {
        auto box  = itr->GetBox();
        auto pSRV = itr->GetSRV();
        auto x = int(offsetX + box.Pos.x) - kTileOffsetX;
        auto y = int(offsetY + box.Pos.y) - kTileOffsetY;

        sprite.Draw(pSRV, x, y, box.Size.x, box.Size.y, 2);
    }
}


///////////////////////////////////////////////////////////////////////////////
// GameMap class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
GameMap::GameMap()
{
    MessageMgr::Instance().Add(this);
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
GameMap::~GameMap()
{
    m_Data = nullptr;
    MessageMgr::Instance().Remove(this);
}

//-----------------------------------------------------------------------------
//      タイルを設定します.
//-----------------------------------------------------------------------------
void GameMap::SetData(GameMapData* value)
{ m_Data = value; }

//-----------------------------------------------------------------------------
//      タイルを取得します.
//-----------------------------------------------------------------------------
Tile GameMap::GetTile(uint8_t id) const
{
    assert(id < kTileTotalCount);
    return m_Data->Tile[id];
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
        auto tile = m_Data->Tile[idx];
        idx++;

        auto pSRV = GetGameMap(tile.TextureId);
        auto step = (float)(j) / kTileCountX;

        auto x = int(kTileOffsetX + kTileSize * j) + int(m_Scroll.x);
        auto y = int(kTileOffsetY + kTileSize * i) + int(m_Scroll.y);

        // キャラよりもY座標が下側なら手前に表示されるように調整.
        auto z = (!tile.Moveable && (playerY < y)) ? 0 : 2;

        sprite.Draw(pSRV, x, y, kTileSize, kTileSize, z);
    }

    for(auto& itr : m_Data->Gimmicks)
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
    // 半キャラ分の可動を許した方が可動領域が広くて操作しやすかったので，
    // 半キャラ分許容する.
    auto idx = CalcTileIndex(
        nextBox.Pos.x + nextBox.Size.x / 2,
        nextBox.Pos.y + nextBox.Size.y / 2);
    auto id  = CalcTileId(idx);

    if (m_Data->Tile[id].Switchable)
    {
        // 最初の一回だけフラグを立てたいから.
        if (!(m_Flags & GAMEMAP_FLAG_SWITCH))
        { m_Flags |= GAMEMAP_FLAG_SWITCH; }
    }
    else if (m_Data->Tile[id].Scrollable)
    {
        // 最初の一回だけフラグを立てたいから.
        if (!(m_Flags & GAMEMAP_FLAG_SCROLL))
        { m_Flags |= GAMEMAP_FLAG_SCROLL; }
    }

    // 移動しちゃダメなタイルなら処理終了.
    if (!m_Data->Tile[id].Moveable)
    { return false; }

    // 移動できないものがあるかどうかチェック.
    for(auto& itr : m_Data->Gimmicks)
    {
        if (!itr->CanMove(nextBox))
        { return false; }
    }

    // 移動してOK!
    return true;
}

//-----------------------------------------------------------------------------
//      更新処理を行います.
//-----------------------------------------------------------------------------
void GameMap::Update(UpdateContext& context)
{
    if (!!(m_Flags & GAMEMAP_FLAG_SWITCH))
    {
        Message msg(MESSAGE_ID_MAP_SWITCH);
        SendMsg(msg);
    }
    else if (!!(m_Flags & GAMEMAP_FLAG_SCROLL))
    {
        Message msg(MESSAGE_ID_MAP_SCROLL, &context.PlayerDir, sizeof(context.PlayerDir));
        SendMsg(msg);
    }

    for(auto& itr : m_Data->Gimmicks)
    { itr->Update(context); }

    if (!!(m_Flags & GAMEMAP_FLAG_SCROLL))
    { Scroll(DIRECTION_STATE(context.PlayerDir)); }
}


//-----------------------------------------------------------------------------
//      ギミックをリセットします.
//-----------------------------------------------------------------------------
void GameMap::ResetGimmicks()
{
    for(auto& itr : m_Data->Gimmicks)
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
    if (m_ScrollFrame < kScrollFrame)
    {
        m_ScrollFrame++;
    }
    else
    {
        // リセット.
        m_ScrollFrame = 0;
        m_Scroll.x    = 0;
        m_Scroll.y    = 0;

        // フラグをおろす.
        m_Flags &= ~(GAMEMAP_FLAG_SCROLL);

        // 完了メッセージを送信.
        Message msg(MESSAGE_ID_MAP_CHANGED);
        SendMsg(msg);

        // おしまい.
        return;
    }

    switch(dir)
    {
    case DIRECTION_LEFT:
        { m_Scroll.x += kMapScrollX; }
        break;

    case DIRECTION_RIGHT:
        { m_Scroll.x -= kMapScrollX; }
        break;

    case DIRECTION_UP:
        { m_Scroll.y += kMapScrollY; }
        break;

    case DIRECTION_DOWN:
        { m_Scroll.y -= kMapScrollY; }
        break;
    }
}

//-----------------------------------------------------------------------------
//      メッセージ受信処理を行います.
//-----------------------------------------------------------------------------
void GameMap::OnMessage(const Message& msg)
{
    switch(msg.GetType())
    {
    case MESSAGE_ID_MAP_SCROLL:
        { m_Flags |= GAMEMAP_FLAG_SCROLL; }
        break;

    case MESSAGE_ID_MAP_SWITCH:
        { m_Flags |= GAMEMAP_FLAG_SWITCH; }
        break;

    default:
        break;
    }
}

//-----------------------------------------------------------------------------
//      スクロール値を取得します.
//-----------------------------------------------------------------------------
Vector2i GameMap::GetScroll() const
{ return Vector2i(int(m_Scroll.x), int(m_Scroll.y)); }


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
