#include <enemy/EnemyTest.h>
#include <GameMap.h>
#include <Entity.h>
#include <MessageId.h>


EnemyTest::EnemyTest()
{
}

EnemyTest::~EnemyTest()
{ Term(); }

bool EnemyTest::Init()
{
    // 1発当たったら死亡.
    m_Life = 1;

    if (!LoadTexture2D("../res/texture/enemy_test/enemy.tga", m_Texture))
    { return false; }

    m_Pos.x = kTileSize * 9 + kMarginX;
    m_Pos.y = kTileSize * 5 + kMarginY;

    return true;
}

void EnemyTest::Term()
{ m_Texture.Release(); }

void EnemyTest::Update(UpdateContext& context)
{
    if (!!(context.Map->GetFlags() & GAMEMAP_FLAG_SCROLL))
    { return; }

    UpdateFrame();

    Box box(m_Pos, Vector2i(kTileSize, kTileSize));
    Box nextBox = box;
    nextBox.Pos += GetMoveDir(m_Direction) * 2;

    // 移動.
    {
        //if (context.Map->CanMove(box, m_Direction))
        if (context.Map->CanMove(nextBox))
        { m_Pos += GetMoveDir(m_Direction) * 2; }
        else
        {
            // 移動的なかったら方向を変える.
            m_Direction = RollDice(m_Random);
        }
    }

    // 30フレームに1回方向を変更.
    if ((m_Frame % (30)) == 0)
    {
        m_Direction = RollDice(m_Random);
    }

    if (m_Life > 0)
    {
        if (context.BoxYellow != nullptr)
        {
            if (IsHit(*context.BoxYellow, box))
            {
                m_Life--;
                m_Frame = 0;
            }
        }

        if (context.BoxRed != nullptr)
        {
            if (IsHit(*context.BoxRed, box))
            {
                int damage = 1;
                Message msg(MESSAGE_ID_PLAYER_DAMAGE, &damage, sizeof(damage));
                SendMsg(msg);
            }
        }
    }

    if (m_Life <= 0 && m_Frame == 120)
    {
        m_Life = 1;
        m_Frame = 0;
    }
}

void EnemyTest::Draw(SpriteSystem& sprite)
{
    if (m_Life == 0)
    { return; }

    sprite.Draw(m_Texture.GetSRV(), m_Pos.x, m_Pos.y, 64, 64, 1);
}