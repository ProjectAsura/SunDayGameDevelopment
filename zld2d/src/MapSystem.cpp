//-----------------------------------------------------------------------------
// File : MapSystem.cpp
// Desc : Game Map System.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cassert>
#include <MapSystem.h>
#include <asdxLogger.h>
#include <Gimmick.h>
#include <Player.h>
#include <MessageId.h>
#include <TextureMgr.h>


#if 0
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
#endif

///////////////////////////////////////////////////////////////////////////////
// MapInstance structure
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      ロード処理を行います.
//-----------------------------------------------------------------------------
bool MapInstance::Load(const char* path)
{
    // TODO : Implementation.
    return true;
}

//-----------------------------------------------------------------------------
//      セーブ処理を行います.
//-----------------------------------------------------------------------------
bool MapInstance::Save(const char* path)
{
    // TODO : Implmenetation.
    return true;
}

//-----------------------------------------------------------------------------
//      破棄処理を行います.
//-----------------------------------------------------------------------------
void MapInstance::Dispose()
{
    // TODO : Implementation.
}


///////////////////////////////////////////////////////////////////////////////
// MapSystem class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
MapSystem::MapSystem()
{
    MessageMgr::Instance().Add(this);
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
MapSystem::~MapSystem()
{
    m_Data = nullptr;
    m_Next = nullptr;
    MessageMgr::Instance().Remove(this);
}

//-----------------------------------------------------------------------------
//      現在のマップデータを設定します.
//-----------------------------------------------------------------------------
void MapSystem::SetData(MapInstance* instance)
{ m_Data = instance; }

//-----------------------------------------------------------------------------
//      次のマップデータを設定します.
//-----------------------------------------------------------------------------
void MapSystem::SetNext(MapInstance* instance)
{ m_Next = instance; }

//-----------------------------------------------------------------------------
//      タイルを取得します.
//-----------------------------------------------------------------------------
Tile MapSystem::GetTile(uint8_t id) const
{
    assert(id < kTileTotalCount);
    return m_Data->Tile[id];
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void MapSystem::Draw(SpriteSystem& sprite, int playerY)
{
    auto idx = 0;
    for(auto i=0u; i<kTileCountY; ++i)
    for(auto j=0u; j<kTileCountX; ++j)
    {
        auto tile = m_Data->Tile[idx];
        idx++;

        auto pSRV = GetTexture(tile.TextureId);
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

    // 次のマップを表示.
    if (m_IsScroll)
    {
        auto dir = GetMoveDir(DIRECTION_STATE(m_ScrollDir));
        auto pos = Vector2i(kTileTotalW, kTileTotalH) * dir;
        pos.x += int(m_Scroll.x);
        pos.y += int(m_Scroll.y);

        idx = 0;
        for(auto i=0u; i<kTileCountY; ++i)
        for(auto j=0u; j<kTileCountX; ++j)
        {
            auto tile = m_Next->Tile[idx];
            idx++;

            auto pSRV = GetTexture(tile.TextureId);
            auto x = int(pos.x + kTileSize * j) + kTileOffsetX;
            auto y = int(pos.y + kTileSize * i) + kTileOffsetY;

            sprite.Draw(pSRV, x, y, kTileSize, kTileSize, 2);
        }

        for(auto& itr : m_Next->Gimmicks)
        {
            auto box  = itr->GetBox();
            auto pSRV = itr->GetSRV();
            auto x = int(pos.x + box.Pos.x);
            auto y = int(pos.y + box.Pos.y);

            sprite.Draw(pSRV, x, y, box.Size.x, box.Size.y, 2);
        }
    }
}

//-----------------------------------------------------------------------------
//      移動可能かどうかチェックします.
//-----------------------------------------------------------------------------
bool MapSystem::CanMove(const Box& nextBox)
{
    // 半キャラ分の可動を許した方が可動領域が広くて操作しやすかったので，
    // 半キャラ分許容する.
    auto idx = CalcTileIndex(
        nextBox.Pos.x + nextBox.Size.x / 2,
        nextBox.Pos.y + nextBox.Size.y / 2);
    auto id  = CalcTileId(idx);

    if (m_Data->Tile[id].Switchable)
    {
        if (!m_IsSwitch)
        { m_IsSwitch = true; }
    }
    else if (m_Data->Tile[id].Scrollable)
    {
        if (!m_IsScroll)
        { m_IsScroll = true; }
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
void MapSystem::Update(UpdateContext& context)
{
    if (m_IsSwitch)
    {
        Message msg(MESSAGE_ID_MAP_SWITCH);
        SendMsg(msg);
    }
    else if (m_IsScroll)
    {
        Message msg(MESSAGE_ID_MAP_SCROLL, &context.PlayerDir, sizeof(context.PlayerDir));
        SendMsg(msg);
    }

    for(auto& itr : m_Data->Gimmicks)
    { itr->Update(context); }

    if (m_IsScroll)
    { Scroll(DIRECTION_STATE(context.PlayerDir)); }
}


//-----------------------------------------------------------------------------
//      ギミックをリセットします.
//-----------------------------------------------------------------------------
void MapSystem::Reset()
{
    for(auto& itr : m_Data->Gimmicks)
    { itr->Reset(); }
}

//-----------------------------------------------------------------------------
//      場面切り替え中かどうか?
//-----------------------------------------------------------------------------
bool MapSystem::IsSwitch() const
{ return m_IsSwitch; }

//-----------------------------------------------------------------------------
//      スクロール切り替え中かどうか?
//-----------------------------------------------------------------------------
bool MapSystem::IsScroll() const
{ return m_IsScroll; }

//-----------------------------------------------------------------------------
//      スクロールによるマップ切り替えを行います.
//-----------------------------------------------------------------------------
void MapSystem::Scroll(DIRECTION_STATE dir)
{
    m_ScrollDir = dir;

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
        m_IsScroll = false;

        // 完了メッセージを送信.
        Message msg(MESSAGE_ID_MAP_CHANGED);
        SendMsg(msg);

        // マップを更新.
        auto data = m_Data;
        m_Data = m_Next;
        m_Next = m_Data;

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
void MapSystem::OnMessage(const Message& msg)
{
    switch(msg.GetType())
    {
    case MESSAGE_ID_MAP_SCROLL:
        { m_IsScroll = true; }
        break;

    case MESSAGE_ID_MAP_SWITCH:
        { m_IsSwitch = true; }
        break;

    default:
        break;
    }
}


