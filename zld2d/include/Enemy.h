﻿//-----------------------------------------------------------------------------
// File : Enemy.h
// Desc : Enemy.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <list>
#include <asdxTexture.h>
#include <DirectionState.h>
#include <UpdateContext.h>
#include <SpriteSystem.h>
#include <Box.h>
#include <MessageMgr.h>


//-----------------------------------------------------------------------------
// Forwad Decalarations.
//-----------------------------------------------------------------------------
class Enemy;


///////////////////////////////////////////////////////////////////////////////
// IEnemyComponent interface
///////////////////////////////////////////////////////////////////////////////
struct IEnemyComponent
{
    virtual ~IEnemyComponent() {}
    virtual void Update(UpdateContext& context, Enemy& enemy) {}
    virtual void OnReceive(const Message& msg, Enemy& enemy) {}
};


///////////////////////////////////////////////////////////////////////////////
// Enemy class
///////////////////////////////////////////////////////////////////////////////
class Enemy : public IMessageListener
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    Enemy();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~Enemy();

    //-------------------------------------------------------------------------
    //! @brief      コンポーネントを追加します.
    //-------------------------------------------------------------------------
    void AddComponent(IEnemyComponent* component);

    //-------------------------------------------------------------------------
    //! @brief      更新処理を行います.
    //-------------------------------------------------------------------------
    void Update(UpdateContext& context);

    //-------------------------------------------------------------------------
    //! @brief      描画処理を行います.
    //-------------------------------------------------------------------------
    void Draw(SpriteSystem& sprite);

    //-------------------------------------------------------------------------
    //! @brief      バウンディングボックスを設定します.
    //-------------------------------------------------------------------------
    void SetBox(const Box& box);

    //-------------------------------------------------------------------------
    //! @brief      バウンディングボックスを取得します.
    //-------------------------------------------------------------------------
    const Box& GetBox() const;

    //-------------------------------------------------------------------------
    //! @brief      HPを取得します.
    //-------------------------------------------------------------------------
    int GetLife() const;

    //-------------------------------------------------------------------------
    //! @brief      ダメージ量を設定します.
    //-------------------------------------------------------------------------
    void SetDamage(int value);

    //-------------------------------------------------------------------------
    //! @brief      HPを設定します.
    //-------------------------------------------------------------------------
    void SetLife(int value);

    //-------------------------------------------------------------------------
    //! @brief      移動方向を取得します.
    //-------------------------------------------------------------------------
    DIRECTION_STATE GetDir() const;

    //-------------------------------------------------------------------------
    //! @brief      移動方向を設定します.
    //-------------------------------------------------------------------------
    void SetDir(DIRECTION_STATE value);
 
    //-------------------------------------------------------------------------
    //! @brief      メッセージ受信処理を行います.
    //-------------------------------------------------------------------------
    void OnReceive(const Message& message);

    //-------------------------------------------------------------------------
    //! @brief      疑似乱数発生器を取得します.
    //-------------------------------------------------------------------------
    asdx::PCG& GetRandom();

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    Box                         m_Box       = {};
    int                         m_Life      = 1;
    DIRECTION_STATE             m_Dir       = DIRECTION_LEFT;
    uint32_t                    m_Frame     = 0;
    asdx::PCG                   m_Random;
    std::list<IEnemyComponent*> m_Components;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};
