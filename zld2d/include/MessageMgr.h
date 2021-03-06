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
#include <asdxFrameHeap.h>
#include <asdxQueue.h>


///////////////////////////////////////////////////////////////////////////////
// Message class
///////////////////////////////////////////////////////////////////////////////
class Message : public asdx::Queue<Message>::Node
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
    uint64_t    m_Size      = 0;
    const void* m_pBuffer   = nullptr;

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
    //! @brief      初期化処理を行います.
    //-------------------------------------------------------------------------
    bool Init(size_t size)
    { return m_Heap.Init(size); }

    //-------------------------------------------------------------------------
    //! @brief      解放処理を行います.
    //-------------------------------------------------------------------------
    void Term()
    {
        m_Queue.Clear();
        m_Heap.Term();
    }

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
    //! @brief      メッセージを追加します.
    //-------------------------------------------------------------------------
    void Push(const Message& msg)
    {
        auto buf = m_Heap.Alloc(sizeof(Message));
        assert(buf != nullptr);

        if (msg.GetSize() > 0)
        {
            auto data = m_Heap.Alloc(msg.GetSize());
            memcpy(data, msg.GetBuffer(), msg.GetSize());
        
            auto instance = new (buf) Message(msg.GetType(), data, msg.GetSize());
            m_Queue.Push(instance);
        }
        else
        {
            auto instance = new (buf) Message(msg.GetType());
            m_Queue.Push(instance);
        }
    }

    //-------------------------------------------------------------------------
    //! @brief      メッセージをブロードキャストします.
    //-------------------------------------------------------------------------
    void Process()
    {
        while(!m_Queue.IsEmpty())
        {
            auto msg = m_Queue.Pop();

            for(auto& itr : m_Listeners)
            { itr->OnMessage(*msg); }
        }

        m_Heap.Reset();
    }

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    static MessageMgr               s_Instance;
    std::list<IMessageListener*>    m_Listeners;
    asdx::FrameHeap                 m_Heap;
    asdx::Queue<Message>            m_Queue;

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
{ MessageMgr::Instance().Push(msg); }
