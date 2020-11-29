﻿//-----------------------------------------------------------------------------
// File : Map.h
// Desc : Game Map.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <list>
#include <SpriteSystem.h>
#include <asdxTexture.h>
#include <DirectionState.h>
#include <UpdateContext.h>
#include <Box.h>
#include <Entity.h>


//-----------------------------------------------------------------------------
// Forward Declarations.
//-----------------------------------------------------------------------------
class Gimmick;

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const uint8_t kTileCountX        = 19;
static const uint8_t kTileCountY        = 11;
static const uint8_t kTileTotalCount    = kTileCountX * kTileCountY;
static const uint8_t kTileSize          = 64;
static const int     kMarginX           = 32;   // (1280 - kTileSize * kTileCountX) / 2;
static const int     kMarginY           = 8;    // (720 - kTileSize * kTileCountY) / 2;
static const int     kTileOffsetX       = kMarginX;
static const int     kTileOffsetY       = kMarginY;
static const int     kTileTotalW        = kTileSize * kTileCountX;
static const int     kTileTotalH        = kTileSize * kTileCountY;
static const int     kScrollFrame       = 64;   // kTileSizeを割り切れる値に設定.
static const float   kMapScrollX        = float(kTileTotalW) / float(kScrollFrame);
static const float   kMapScrollY        = float(kTileTotalH) / float(kScrollFrame);
static const float   kCharaScrollX      = float(kTileSize * (kTileCountX - 2) - kTileSize / 2) / float(kScrollFrame);   // 左右1タイル分を除く + 半キャラ分補正.
static const float   kCharaScrollY      = float(kTileSize * (kTileCountY - 2) - kTileSize / 2) / float(kScrollFrame);   // 上下1タイル分を除く + 半キャラ分補正.
static const int     kMapMaxiX          = kTileOffsetX + kTileSize * (kTileCountX - 1);
static const int     kMapMaxiY          = kTileOffsetY + kTileSize * (kTileCountY - 1);


///////////////////////////////////////////////////////////////////////////////
// GAMEMAP_TEXTURE enum
///////////////////////////////////////////////////////////////////////////////
enum GAMEMAP_TEXTURE
{
    GAMEMAP_TEXTURE_PLANE,      // 平面.
    GAMEMAP_TEXTURE_ROCK,       // 岩.
    GAMEMAP_TEXTURE_TREE,       // 木
    GAMEMAP_TEXTURE_BLOCK,      // ブロック.
};

///////////////////////////////////////////////////////////////////////////////
// GAMEMAP_FLAGS
///////////////////////////////////////////////////////////////////////////////
enum GAMEMAP_FLAGS 
{
    GAMEMAP_FLAG_NONE       = 0,            //!< デフォルト.
    GAMEMAP_FLAG_SCROLL     = 0x1 << 0,     //!< スクロールによるマップ切り替え.
    GAMEMAP_FLAG_SCROLL_END = 0x1 << 1,     //!< スクロール終了.
    GAMEMAP_FLAG_SWITCH     = 0x1 << 2,     //!< 場面転換によるマップ切り替え.
    GAMEMAP_FLAG_SWITCH_END = 0x1 << 3      //!< 場面転換終了.
};


///////////////////////////////////////////////////////////////////////////////
// Tile structure
///////////////////////////////////////////////////////////////////////////////
struct Tile
{
    uint8_t     TextureId;      //!< テクスチャ番号.
    bool        Moveable;       //!< 移動可能.
    bool        Scrollable;     //!< スクロールによるマップ切り替え.
    bool        Switchable;     //!< 場面転換によるマップ切り替え(階段など).
    bool        Fallable;       //!< 床無しで落ちる.
};

//-----------------------------------------------------------------------------
//      タイルインデックスを計算します.
//-----------------------------------------------------------------------------
inline Vector2i CalcTileIndex(int posX, int posY)
{
    auto ix = (posX - kTileOffsetX) / kTileSize;
    auto iy = (posY - kTileOffsetY) / kTileSize;

    if (ix >= (kTileCountX - 1))
    { ix = kTileCountX - 1; }
    else if (ix < 0)
    { ix = 0; }
    
    if (iy >= (kTileCountY - 1))
    { iy = kTileCountY - 1; }
    else if (iy < 0)
    { iy = 0; }

    return Vector2i(ix, iy);
};

//-----------------------------------------------------------------------------
//      タイルIDを計算します.
//-----------------------------------------------------------------------------
inline uint8_t CalcTileId(const Vector2i& index)
{ return uint8_t(index.x) + uint8_t(index.y) * kTileCountX; }

//-----------------------------------------------------------------------------
//      タイルインデックスから位置座標を求めます.
//-----------------------------------------------------------------------------
inline Vector2i CalcPosFromTile(int tileX, int tileY)
{
    return Vector2i(
        kTileOffsetX + kTileSize * tileX,
        kTileOffsetY + kTileSize * tileY);
}


///////////////////////////////////////////////////////////////////////////////
// GameMapData structure
///////////////////////////////////////////////////////////////////////////////
struct GameMapData
{
    Tile Tile[kTileCountX * kTileCountY] = {};
    std::list<Gimmick*> Gimmicks;

    bool Load(const char* path);
    bool Save(const char* path);
    void Dispose();
    void Draw(SpriteSystem& sprite, int offsetX, int offsetY);
};


///////////////////////////////////////////////////////////////////////////////
// GameMap class
///////////////////////////////////////////////////////////////////////////////
class GameMap : public IEntity
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    GameMap();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~GameMap();

    //-------------------------------------------------------------------------
    //! @brief      マップデータを設定します.
    //-------------------------------------------------------------------------
    void SetData(GameMapData* value);

    //-------------------------------------------------------------------------
    //! @brief      タイルを取得します.
    //-------------------------------------------------------------------------
    Tile GetTile(uint8_t id) const;

    //-------------------------------------------------------------------------
    //! @brief      更新処理を行います.
    //-------------------------------------------------------------------------
    void Update(UpdateContext& context);

    //-------------------------------------------------------------------------
    //! @brief      描画します.
    //-------------------------------------------------------------------------
    void Draw(SpriteSystem& sprite, int playerY);

    //-------------------------------------------------------------------------
    //! @brief      移動可能かどうかチェックします.
    //-------------------------------------------------------------------------
    bool CanMove(const Box& nextBox);

    //-------------------------------------------------------------------------
    //! @brief      ギミックの状態をリセットします.
    //-------------------------------------------------------------------------
    void ResetGimmicks();

    //-------------------------------------------------------------------------
    //! @brief      フラグを取得します.
    //-------------------------------------------------------------------------
    uint8_t GetFlags() const;

    //-------------------------------------------------------------------------
    //! @brief      メッセージ受信処理を行います.
    //-------------------------------------------------------------------------
    void OnReceive(const Message& msg) override;

    //-------------------------------------------------------------------------
    //! @brief      スクロール値を取得します.
    //-------------------------------------------------------------------------
    Vector2i GetScroll() const;

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    GameMapData*    m_Data      = nullptr;
    asdx::Vector2   m_Scroll    = asdx::Vector2(0.0f, 0.0f);
    uint8_t         m_Flags     = 0;
    uint8_t         m_ScrollFrame = 0;

    //=========================================================================
    // private methods.
    //=========================================================================
    void Scroll(DIRECTION_STATE dir);
};


///////////////////////////////////////////////////////////////////////////////
// GameMapTexture
///////////////////////////////////////////////////////////////////////////////
class GameMapTextureMgr
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      シングルトンインスタンスを取得します.
    //-------------------------------------------------------------------------
    static GameMapTextureMgr& Instance();

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //-------------------------------------------------------------------------
    bool Init();

    //-------------------------------------------------------------------------
    //! @brief      破棄処理を行います.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      シェーダリソースビューを取得します.
    //-------------------------------------------------------------------------
    ID3D11ShaderResourceView* GetSRV(uint32_t index) const;

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    static GameMapTextureMgr        s_Instance;
    std::vector<asdx::Texture2D>    m_Textures;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOHTING */
};

//-----------------------------------------------------------------------------
//! @brief      ゲームマップを取得します.
//-----------------------------------------------------------------------------
inline ID3D11ShaderResourceView* GetGameMap(uint32_t index)
{ return GameMapTextureMgr::Instance().GetSRV(index); }
