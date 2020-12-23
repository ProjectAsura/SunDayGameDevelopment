﻿//-----------------------------------------------------------------------------
// File : Component.h
// Desc : Component.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <UpdateContext.h>
#include <MessageMgr.h>


//-----------------------------------------------------------------------------
// Forward Decalarations.
//-----------------------------------------------------------------------------
class Entity;


///////////////////////////////////////////////////////////////////////////////
// Component class
///////////////////////////////////////////////////////////////////////////////
class Component
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    friend class Entity;

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
    Component() = default;

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    virtual ~Component()
    { /* DO_NOTHING */ }
    
    //-------------------------------------------------------------------------
    //! @brief      更新処理です.
    //-------------------------------------------------------------------------
    virtual void OnUpdate(UpdateContext& context)
    { /* DO_NOTHING */ };

    //-------------------------------------------------------------------------
    //! @brief      描画処理です.
    //-------------------------------------------------------------------------
    virtual void OnDraw(SpriteSystem& sprite)
    { /* DO_NOTHING */ }
    
    //-------------------------------------------------------------------------
    //! @brief      メッセージ受信処理です.
    //-------------------------------------------------------------------------
    virtual void OnReceive(const Message& msg) 
    { /* DO_NOTHING */ };
    
    //-------------------------------------------------------------------------
    //! @brief      エンティティを取得します.
    //-------------------------------------------------------------------------
    Entity* GetEntity() const
    { return m_Entity; }

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    Entity* m_Entity = nullptr;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};
