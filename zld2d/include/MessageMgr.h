﻿//-----------------------------------------------------------------------------
// File : MessageMgr.h
// Desc : Message Manager.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <cassert>
#include <list>


///////////////////////////////////////////////////////////////////////////////
// Message class
///////////////////////////////////////////////////////////////////////////////
class Message
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
    // puiblic methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      引数付きコンストラクタです.
    //-------------------------------------------------------------------------
    Message(uint32_t type, const void* buffer = nullptr, uint64_t size = 0)
    : m_Type(type), m_pBuffer(buffer), m_Size(size)
    { /* DO_NOTHING */ }

    //-------------------------------------------------------------------------
    //! @brief      メッセージタイプを取得します.
    //-------------------------------------------------------------------------
    uint32_t GetType() const
    { return m_Type; }

    //-------------------------------------------------------------------------
    //! @brief      バッファを取得します.
    //-------------------------------------------------------------------------
    const void* GetBuffer() const
    { return m_pBuffer; }

    //-------------------------------------------------------------------------
    //! @brief      バッファサイズを取得します.
    //-------------------------------------------------------------------------
    uint64_t GetSize() const
    { return m_Size; }

    //-------------------------------------------------------------------------
    //! @brief      指定形式でバッファを取得します.
    //-------------------------------------------------------------------------
    template<typename T>
    const T* GetAs() const
    {
        assert(m_Size == sizeof(T));
        return reinterpret_cast<const T*>(m_pBuffer);
    }

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    uint32_t    m_Type      = 0;
    const void* m_pBuffer   = nullptr;
    uint64_t    m_Size      = 0;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};


///////////////////////////////////////////////////////////////////////////////
// IMessageListener interface
///////////////////////////////////////////////////////////////////////////////
struct IMessageListener
{
    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    virtual ~IMessageListener()
    { /* DO_NOTHING */ }

    //-------------------------------------------------------------------------
    //! @brief      メッセージ受信時の処理です.
    //-------------------------------------------------------------------------
    virtual void OnMessage(const Message& msg) = 0;
};


///////////////////////////////////////////////////////////////////////////////
// MessageMgr class
///////////////////////////////////////////////////////////////////////////////
class MessageMgr
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
    //! @brief      シングルトンインスタンスを取得します.
    //-------------------------------------------------------------------------
    static MessageMgr& Instance()
    { return s_Instance; }

    //-------------------------------------------------------------------------
    //! @brief      メッセージリスナーを追加します.
    //-------------------------------------------------------------------------
    void Add(IMessageListener* instance)
    { m_Listeners.push_back(instance); }
    
    //-------------------------------------------------------------------------
    //! @brief      メッセージリスナーを削除します.
    //-------------------------------------------------------------------------
    void Remove(IMessageListener* instance)
    { m_Listeners.remove(instance); }

    //-------------------------------------------------------------------------
    //! @brief      全メッセージリスナーを破棄します.
    //-------------------------------------------------------------------------
    void Clear()
    { m_Listeners.clear(); }

    //-------------------------------------------------------------------------
    //! @brief      メッセージをブロードキャストします.
    //-------------------------------------------------------------------------
    void Send(const Message& msg)
    {
        for(auto& itr : m_Listeners)
        { itr->OnMessage(msg); }
    }

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    static MessageMgr               s_Instance;
    std::list<IMessageListener*>    m_Listeners;

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    MessageMgr()
    { /* DO_NOTHING */ }

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~MessageMgr()
    { Clear(); }

    MessageMgr              (const MessageMgr&) = delete;   // アクセス禁止.
    MessageMgr& operator =  (const MessageMgr&) = delete;   // アクセス禁止.
};


//-----------------------------------------------------------------------------
//! @brief      メッセージをブロードキャストします.
//-----------------------------------------------------------------------------
inline void SendMsg(const Message& msg)
{ MessageMgr::Instance().Send(msg); }