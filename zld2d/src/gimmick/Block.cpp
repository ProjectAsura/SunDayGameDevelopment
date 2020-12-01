//-----------------------------------------------------------------------------
// File : Block.cpp
// Desc : Block Gimmick.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cassert>
#include <gimmick/Block.h>
#include <GameMap.h>
#include <asdxLogger.h>
#include <MessageId.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const uint8_t    kMoving      = 1;
static const uint8_t    kComplete    = 2;
static const int        kMovePixel   = 2;
static const int        kDetectFrame = 30;

} // namespace

///////////////////////////////////////////////////////////////////////////////
// Block class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
Block::Block()
: m_Frame(kDetectFrame)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
Block::~Block()
{ m_pSRV = nullptr; }

//-----------------------------------------------------------------------------
//      移動方向を設定します.
//-----------------------------------------------------------------------------
void Block::SetDir(DIRECTION_STATE dir)
{
    m_InitDir = dir;
    m_Dir     = dir;
}

//-----------------------------------------------------------------------------
//      フラグと移動方向をリセットします.
//-----------------------------------------------------------------------------
void Block::Reset()
{
    m_Box.Pos -= GetMoveDir(m_Dir) * m_Moved;
    m_Moved    = 0;
    m_Flags    = 0;
    m_Frame    = kDetectFrame;
    m_PrevHit  = false;
    m_Dir      = m_InitDir;
}

//-----------------------------------------------------------------------------
//      プレイヤーが稼働可能か?
//-----------------------------------------------------------------------------
bool Block::CanMove(const Box& playerBox)
{
    m_CurrHit = IsHit(playerBox, m_Box);
    return !m_CurrHit;
}

//-----------------------------------------------------------------------------
//      更新処理を行います.
//-----------------------------------------------------------------------------
void Block::Update(UpdateContext& context)
{
    if (context.BoxRed == nullptr)
    { return; }

    // 動き切ったら処理しない.
    if (m_Flags == kComplete)
    { return; }

    if (m_CurrHit && m_PrevHit)
    {
        if (m_Dir == context.PlayerDir)
        {
            if (m_Frame > 0)
            {
                m_Frame--;
            }
            else if (m_Frame == 0)
            {
                m_Flags = kMoving;
            }
        }
        else
        {
            m_Frame = kDetectFrame;

            // 方向が設定されていないときは最初に検知した方向にする.
            if (m_InitDir == DIRECTION_NONE)
            { m_Dir = DIRECTION_STATE(context.PlayerDir); }
        }
    }

    if (m_Flags == kMoving)
    {
        m_Box.Pos += GetMoveDir(m_Dir) * kMovePixel;
        m_Moved   += kMovePixel;

        // タイルサイズ分移動したら完了.
        if (m_Moved >= kTileSize)
        { m_Flags = kComplete; }
    }

    m_PrevHit = m_CurrHit;
}

//-----------------------------------------------------------------------------
//      メッセージ受信処理を行います.
//-----------------------------------------------------------------------------
void Block::OnMessage(const Message& msg)
{
    switch(msg.GetType())
    {
    case MESSAGE_ID_MAP_SCROLL:
        OnScroll(msg);
        break;

    case MESSAGE_ID_MAP_CHANGED:
        OnScrollCompleted();
        break;

    default:
        break;
    }
}
