//-----------------------------------------------------------------------------
// File : Gimmick.cpp
// Desc : Gimmick Object.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cassert>
#include <Gimmick.h>


namespace {

// rhsをブロックとする.
inline bool GetHitDir(const Box& lhs, const Box& rhs)
{
    // めり込みがある方向を判定
    auto x = lhs.Pos.x - rhs.Pos.x;
    auto y = lhs.Pos.y - rhs.Pos.y;
    if (abs(x) > abs(y))
    { return (x < 0) ? DIRECTION_RIGHT : DIRECTION_LEFT;  }

    return (y < 0) ? DIRECTION_DOWN : DIRECTION_UP;
}

} // namespace


///////////////////////////////////////////////////////////////////////////////
// Block class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
Block::Block()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
Block::~Block()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
void Block::Init
(
    int                         x,
    int                         y,
    int                         w,
    int                         h,
    DIRECTION_STATE             dir,
    ID3D11ShaderResourceView*   pSRV
)
{
    assert(pSRV != nullptr);
    m_Box.Pos.x     = x;
    m_Box.Pos.y     = y;
    m_Box.Size.x    = w;
    m_Box.Size.y    = h;
    m_pSRV          = pSRV;
    m_Dir           = dir;
    m_Frame         = 60;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void Block::Term()
{ m_pSRV = nullptr; }

//-----------------------------------------------------------------------------
//      更新処理を行います.
//-----------------------------------------------------------------------------
bool Block::Update(const Box& playerBox)
{
    auto hit = IsHit(playerBox, m_Box);

    // 動き切ったら処理しない.
    if (m_Complete)
    { return hit; }

    if (hit && m_PrevHit)
    {
        if (m_Dir == GetHitDir(playerBox, m_Box))
        {
            if (m_Frame > 0)
            {
                m_Frame--;
            }
            else if (m_Frame == 0)
            {
                m_Box.Pos += GetMoveDir(m_Dir) * 4;
                m_Moved += 4;
                if (m_Moved == m_Box.Size.x)
                { m_Complete = true; }
            }
        }
        else
        {
            m_Frame = 60;
        }
    }

    m_PrevHit = hit;
    return hit;
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Block::Draw(SpriteSystem& sprite)
{ sprite.Draw(m_pSRV, m_Box); }
