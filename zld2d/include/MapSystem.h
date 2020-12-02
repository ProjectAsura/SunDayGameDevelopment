//-----------------------------------------------------------------------------
// File : MapSystem.h
// Desc : System for Game Map.
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
#include <MessageMgr.h>


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
static const int     kScrollFrame       = 64;
static const float   kMapScrollX        = float(kTileTotalW) / float(kScrollFrame);
static const float   kMapScrollY        = float(kTileTotalH) / float(kScrollFrame);
static const float   kCharaScrollX      = float(kTileSize * (kTileCountX - 2) - kTileSize / 2) / float(kScrollFrame);   // 左右1タイル分を除く + 半キャラ分補正.
static const float   kCharaScrollY      = float(kTileSize * (kTileCountY - 2) - kTileSize / 2) / float(kScrollFrame);   // 上下1タイル分を除く + 半キャラ分補正.
static const int     kMapMaxiX          = kTileOffsetX + kTileSize * (kTileCountX - 1);
static const int     kMapMaxiY          = kTileOffsetY + kTileSize * (kTileCountY - 1);


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
// MapInstance structure
///////////////////////////////////////////////////////////////////////////////
struct MapInstance
{
    Tile                Tile[kTileCountX * kTileCountY] = {};
    std::list<Gimmick*> Gimmicks;

    //-------------------------------------------------------------------------
    //! @brief      ファイルからロードを行います.
    //! 
    //! @param[in]      path        ファイルパス.
    //! @retval true    ロードに成功.
    //! @retval false   ロードに失敗.
    //-------------------------------------------------------------------------
    bool Load(const char* path);

    //-------------------------------------------------------------------------
    //! @brief      ファイルにセーブを行います.
    //! 
    //! @param[in]      path        ファイルパス.
    //! @retval true    セーブに成功.
    //! @retval fasle   セーブに失敗.
    //-------------------------------------------------------------------------
    bool Save(const char* path);

    //-------------------------------------------------------------------------
    //! @brief      破棄処理を行います.
    //-------------------------------------------------------------------------
    void Dispose();
};


///////////////////////////////////////////////////////////////////////////////
// MapSystem class
///////////////////////////////////////////////////////////////////////////////
class MapSystem : public IMessageListener
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    MapSystem();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~MapSystem();

    //-------------------------------------------------------------------------
    //! @brief      現在のマップデータを設定します.
    //-------------------------------------------------------------------------
    void SetData(MapInstance* value);

    //-------------------------------------------------------------------------
    //! @brief      次のマップデータを設定します.
    //-------------------------------------------------------------------------
    void SetNext(MapInstance* value);

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
    void Reset();

    //-------------------------------------------------------------------------
    //! @brief      メッセージ受信処理を行います.
    //-------------------------------------------------------------------------
    void OnMessage(const Message& msg) override;

    //-------------------------------------------------------------------------
    //! @brief      場面切り替え中かどうか?
    //-------------------------------------------------------------------------
    bool IsSwitch() const;

    //-------------------------------------------------------------------------
    //! @brief      スクロールによる切り替え中かどうか?
    //-------------------------------------------------------------------------
    bool IsScroll() const;

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    MapInstance*    m_Data          = nullptr;
    MapInstance*    m_Next          = nullptr;
    asdx::Vector2   m_Scroll        = asdx::Vector2(0.0f, 0.0f);
    bool            m_IsSwitch      = false;
    bool            m_IsScroll      = false;
    uint8_t         m_ScrollFrame   = 0;
    uint8_t         m_ScrollDir     = 0;

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      スクロール処理を行います.
    //-------------------------------------------------------------------------
    void Scroll(DIRECTION_STATE dir);
};

