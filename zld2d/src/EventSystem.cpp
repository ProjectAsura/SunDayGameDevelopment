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
    MESSAGE_KIND_CHOICES_2,     //!< ユーザーが2択する.
};

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const wchar_t* kFontName     = L"しねきゃぷしょん";  // フォント名.
static const float    kFontSize     = 32;   // フォントサイズ.
static const int      kWndWidth     = 1000; // メッセージウィンドウ横幅.
static const int      kWndHeight    = 200;  // メッセージウィンドウ縦幅.
static const int      kWndPosX      = 140;  // メッセージウィンドウX座標.
static const int      kWndUpperY    = 64;   // メッセージウィンドウY座標(上側に表示する場合).
static const int      kWndLowerY    = 488;  // メッセージウィンドウY座標(下側に表示する場合).
static const float    kWndColor[]   = { 0.0f, 0.0f, 0.0, 0.8f }; // ウィンドウ乗算カラー.

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
    Term();
    MessageMgr::Instance().Remove(this);
}

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool EventSystem::Init(IDWriteFactory* pFactory, ID2D1DeviceContext* pContext)
{
    m_pContext = pContext;
    return m_Writer.Init(pFactory, pContext, kFontName, kFontSize);
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void EventSystem::Term()
{
    m_Writer.Term();
}

//-----------------------------------------------------------------------------
//      シナリオIDを取得します.
//-----------------------------------------------------------------------------
uint32_t EventSystem::GetScenarioId() const
{ return m_ScenarioId; }

//-----------------------------------------------------------------------------
//      イベントIDを取得します.
//-----------------------------------------------------------------------------
uint32_t EventSystem::GetEventId() const
{ return m_EventId; }

//-----------------------------------------------------------------------------
//      イベント中かどうか?
//-----------------------------------------------------------------------------
bool EventSystem::IsEvent() const
{ return m_IsDraw; }

//-----------------------------------------------------------------------------
//      メッセージウィンドウ枠を表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawWindow(SpriteSystem& sprite, bool upper)
{
    if (!m_IsDraw)
    { return; }

    const int kY = (upper) ? kWndUpperY : kWndLowerY;

    sprite.SetColor(kWndColor[0], kWndColor[1], kWndColor[2], kWndColor[3]);
    sprite.Draw(
        GetTexture(TEXTURE_HUD_WINDOW),
        kWndPosX, kY, kWndWidth, kWndHeight, 0);

    if (m_Type == MESSAGE_KIND_CHOICES_2)
    {
        const int kY = (upper) ? 86 : 512;
        static const int kH = 42; // 32px文字サイズ + 10px上下間隔.

        sprite.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
        sprite.Draw(
            GetTexture(TEXTURE_HUD_SELECT_CURSOR),
            kWndPosX + 32,
            kY + kH * (2 + m_CurrentChoice),
            32,
            32);
    }
}

//-----------------------------------------------------------------------------
//      会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawMsg(ID2D1DeviceContext* context, bool upper)
{
    if (!m_IsDraw)
    { return; }

    //m_Writer.SetColor(0.0f, 0.0f, 0.0f, 1.0f);

    switch(m_Type)
    {
    case MESSAGE_KIND_DEFAULT:
        DrawEventMsg(m_Text, upper);
        break;

    case MESSAGE_KIND_CHOICES_2:
        DrawChoices2(m_Text, m_Option[0], m_Option[1], upper);
        break;
    }
}

//-----------------------------------------------------------------------------
//      選択無しの会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawEventMsg(const wchar_t* msg, bool upper)
{
    m_Writer.DrawLine(m_pContext, msg, 0, upper);
}

//-----------------------------------------------------------------------------
//      2択の会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawChoices2
(
    const wchar_t* msg,
    const wchar_t* choice0,
    const wchar_t* choice1,
    bool           upper
)
{
    m_Writer.DrawLine(m_pContext, msg, 0, upper);
    m_Writer.DrawLine(m_pContext, choice0, 2, upper);
    m_Writer.DrawLine(m_pContext, choice1, 3, upper);
}

//-----------------------------------------------------------------------------
//      メッセージ受信処理です.
//-----------------------------------------------------------------------------
void EventSystem::OnMessage(const Message& msg)
{
    switch(msg.GetType())
    {
    case MESSAGE_ID_EVENT_RAISE:
        {
            // 描画フラグを立てる.
            m_IsDraw = true;

            auto eventMsg = msg.GetAs<EventData>();
            m_ScenarioId  = eventMsg->ScenarioId;
            m_EventId     = eventMsg->EventId;
            wcscpy_s(m_Text, eventMsg->Text);
            m_Type = MESSAGE_KIND_DEFAULT;
        }
        break;

    case MESSAGE_ID_EVENT_BRUNCH:
        {
            // 描画フラグを立てる.
            m_IsDraw = true;

            auto eventMsg = msg.GetAs<BrunchData>();
            m_ScenarioId  = eventMsg->ScenarioId;
            m_EventId     = eventMsg->EventId;
            wcscpy_s(m_Text, eventMsg->Text);
            wcscpy_s(m_Option[0], eventMsg->Option[0]);
            wcscpy_s(m_Option[1], eventMsg->Option[1]);
            m_Type = MESSAGE_KIND_CHOICES_2;
        }
        break;

    case MESSAGE_ID_EVENT_END:
        {
            m_Text[0]       = L'\0';
            m_Option[0][0]  = L'\0';
            m_Option[1][0]  = L'\0';
            m_Type           = 0;

            // 描画フラグをおろす.
            m_IsDraw = false;
        }
        break;

    case MESSAGE_ID_EVENT_UPDATE_CURSOR:
        {
            auto cursor = *msg.GetAs<uint8_t>();
            m_CurrentChoice = cursor;
        }
        break;
    }
}