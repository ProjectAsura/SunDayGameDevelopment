#pragma once

#include <Enemy.h>


class EnemyTest : public Enemy
{
public:
    EnemyTest();
    ~EnemyTest();
    bool Init() override;
    void Term() override;
    void Update(UpdateContext& context);
    void Draw(SpriteSystem& sprite);

private:
    asdx::Texture2D m_Texture;
};