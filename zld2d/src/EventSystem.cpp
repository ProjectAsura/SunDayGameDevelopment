//-----------------------------------------------------------------------------
// File : EventSystem.cpp
// Desc : System For Game Event.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <EventSystem.h>
#include <MessageId.h>
#include <TextureId.h>
#include <TextureMgr.h>


namespace {

///////////////////////////////////////////////////////////////////////////////
// MESSAGE_KIND enum
///////////////////////////////////////////////////////////////////////////////
enum MESSAGE_KIND {
    MESSAGE_KIND_DEFAULT,       //!< 会話のみ.
    MESSAGE_KIND_CHOICE2,       //!< ユーザーが2択する.
    MESSAGE_KIND_CHOICE3,       //!< ユーザーが3択する.
    MESSAGE_KIND_CHOICE4,       //!< ユーザーが4択する.
};

} // namespace


///////////////////////////////////////////////////////////////////////////////
// EventSystem class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
EventSystem::EventSystem()
{
    MessageMgr::Instance().Add(this);
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
EventSystem::~EventSystem()
{
    MessageMgr::Instance().Remove(this);
}

//-----------------------------------------------------------------------------
//      メッセージウィンドウ枠を表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawWindow(SpriteSystem& sprite, bool upper)
{
    if (!m_IsDraw)
    { return; }

    static const int kW = 1000;
    static const int kH = 200;

    static const int kX = 140;
    int kY = (upper) ? 64 : 488;

    sprite.SetColor(0.75f, 0.75f, 0.75f, 0.75f);
    sprite.Draw(
        GetTexture(TEXTURE_HUD_WINDOW),
        kX, kY, kW, kH, 0);
}

//-----------------------------------------------------------------------------
//      会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawMsg(ID2D1DeviceContext* context, bool upper)
{
    if (!m_IsDraw)
    { return; }

    switch(m_Type)
    {
    case MESSAGE_KIND_DEFAULT:
        DrawEventMsg(m_Lines[0]);
        break;

    case MESSAGE_KIND_CHOICE2:
        DrawChoices2(m_Lines[0], m_Lines[1], m_Lines[2]);
        break;

    case MESSAGE_KIND_CHOICE3:
        DrawChoices3(m_Lines[0], m_Lines[1], m_Lines[2], m_Lines[3]);
        break;

    case MESSAGE_KIND_CHOICE4:
        DrawChoices4(m_Lines[0], m_Lines[1], m_Lines[2], m_Lines[3], m_Lines[4]);
        break;
    }
}

//-----------------------------------------------------------------------------
//      選択無しの会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawEventMsg(const wchar_t* msg)
{
}

//-----------------------------------------------------------------------------
//      2択の会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawChoices2
(
    const wchar_t* msg,
    const wchar_t* choice0,
    const wchar_t* choise1
)
{
}

//-----------------------------------------------------------------------------
//      3択の会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawChoices3
(
    const wchar_t* msg,
    const wchar_t* choice0,
    const wchar_t* choise1,
    const wchar_t* choise2
)
{
}

//-----------------------------------------------------------------------------
//      4択の会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawChoices4
(
    const wchar_t* msg,
    const wchar_t* choice0,
    const wchar_t* choise1,
    const wchar_t* choise2,
    const wchar_t* choise3
)
{
}

//-----------------------------------------------------------------------------
//      メッセージ受信処理です.
//-----------------------------------------------------------------------------
void EventSystem::OnMessage(const Message& msg)
{
    switch(msg.GetType())
    {
    case MESSAGE_ID_EVENT_START:
        { m_IsDraw = true; }
        break;

    case MESSAGE_ID_EVENT_NEXT:
        { m_EventId++; }
        break;

    case MESSAGE_ID_EVENT_END:
        { m_IsDraw = false; }
        break;
    }
}