#include <enemy/EnemyTest.h>
#include <GameMap.h>


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
    UpdateFrame();

    Box box(m_Pos, Vector2i(kTileSize, kTileSize));

    // 移動.
    {
        if (context.Map->CanMove(box, m_Direction))
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

    if (context.HitBox != nullptr)
    {
        if (IsHit(*context.HitBox, box))
        {
            m_Life--;
            m_Frame = 0;
        }
    }

    if (m_Life > 0)
    {
        if (context.DamageBox != nullptr)
        { context.Damage |= IsHit(*context.DamageBox, box); }
    }

    if (m_Life == 0 && m_Frame == 120)
    {
        m_Life = 1;
        m_Frame = 0;
    }
}

void EnemyTest::Draw(SpriteSystem& sprite)
{
    if (m_Life == 0)
    { return; }

    sprite.Draw(m_Texture.GetSRV(), m_Pos.x, m_Pos.y, 64, 64);
}