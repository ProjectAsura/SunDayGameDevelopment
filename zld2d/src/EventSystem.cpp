//-----------------------------------------------------------------------------
// File : EventSystem.cpp
// Desc : System For Game Event.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <EventSystem.h>
#include <MessageId.h>
#include <TextureId.h>
#include <TextureMgr.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
static const wchar_t* kFontName         = L"しねきゃぷしょん";  // フォント名.
static const float    kFontSize         = 32;   // フォントサイズ.
static const int      kWndWidth         = 1000; // メッセージウィンドウ横幅.
static const int      kWndHeight        = 200;  // メッセージウィンドウ縦幅.
static const int      kWndPosX          = 140;  // メッセージウィンドウX座標.
static const int      kWndUpperY        = 64;   // メッセージウィンドウY座標(上側に表示する場合).
static const int      kWndLowerY        = 488;  // メッセージウィンドウY座標(下側に表示する場合).
static const float    kWndColor[]       = { 0.0f, 0.0f, 0.0f, 0.8f }; // ウィンドウ乗算カラー.
static const float    kTextColor[]      = { 1.0f, 1.0f, 1.0f, 1.0f }; // テキスト乗算カラー.
static const float    kActiveColor[]    = { 0.1f, 1.0f, 0.1f, 1.0f }; // アクティブカラー.


///////////////////////////////////////////////////////////////////////////////
// EventTablePath structure
///////////////////////////////////////////////////////////////////////////////
struct EventTablePath
{
    uint32_t    ScenarioId;     //!< シナリオID.
    const char* Path;           //!< ファイルパス.
};

// シナリオテーブル.
static const EventTablePath kEventTablePath[] = {
    { 0, "res/event/scenario_0.record" }, 
};


//-----------------------------------------------------------------------------
//      イベントテーブルを読み込みます.
//-----------------------------------------------------------------------------
bool LoadEventTable(const char* path, std::map<uint32_t, EventRecord>& result)
{
    FILE* pFile = nullptr;

    auto err = fopen_s(&pFile, path, "r");
    if (err != 0)
    {
        ELOGA("Error : File Open Failed. path = %s", path);
        return false;
    }

    while(!feof(pFile))
    {
        uint32_t    eventId     = 0;
        int         flag        = 0;
        EventRecord record      = {};
        char        text[141]   = {};
        char        optionA[28] = {};
        char        optionB[28] = {};

        auto count = fscanf_s(pFile, "%u\t%d\t%s\t%s\t%s",
            &eventId,
            &flag,
            text, 141,
            optionA, 28,
            optionB, 28);

        if (count <= 0)
        { break; }

        record.HasBrunch  = (flag == 1);
        record.UserSelect = 0;
        MultiByteToWideChar(CP_UTF8, 0, text,   141, record.Text,   141);
        MultiByteToWideChar(CP_UTF8, 0, optionA, 28, record.OptionA, 28);
        MultiByteToWideChar(CP_UTF8, 0, optionB, 28, record.OptionB, 28);

    #if 0
        //ILOGA("eventId = %u, hasBrunch = %d, text = %ls, optionA = %ls, optionB = %ls",
        //    eventId, record.HasBrunch, record.Text, record.OptionA, record.OptionB);
    #endif

        result[eventId] = record;
    }

    fclose(pFile);

    return true;
}

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

    if (!LoadScenario(m_ScenarioId))
    { return false; }

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
    if (!m_IsDraw || m_Table.empty())
    { return; }

    if (m_Table.find(m_EventId) == m_Table.end())
    { return; }

    const int kY = (upper) ? kWndUpperY : kWndLowerY;

    sprite.SetColor(kWndColor[0], kWndColor[1], kWndColor[2], kWndColor[3]);
    sprite.Draw(
        GetTexture(TEXTURE_HUD_WINDOW),
        kWndPosX, kY, kWndWidth, kWndHeight, 0);

    auto& record = m_Table.at(m_EventId);
    if (record.HasBrunch)
    {
        const int kY = (upper) ? 86 : 512;
        static const int kH = 42; // 32px文字サイズ + 10px上下間隔.

        sprite.SetColor(kTextColor[0], kTextColor[1], kTextColor[2], kTextColor[3]);
        sprite.Draw(
            GetTexture(TEXTURE_HUD_SELECT_CURSOR),
            kWndPosX + 32,
            kY + kH * (2 + record.UserSelect),
            32,
            32);
    }
}

//-----------------------------------------------------------------------------
//      会話メッセージを表示します.
//-----------------------------------------------------------------------------
void EventSystem::DrawMsg(ID2D1DeviceContext* context, bool upper)
{
    if (!m_IsDraw || m_Table.empty())
    { return; }

    if (m_Table.find(m_EventId) == m_Table.end())
    { return; }

    SetDefaultColor();

    auto& record = m_Table.at(m_EventId);
    if (record.HasBrunch)
    {
        DrawChoices2(
            record.Text,
            record.OptionA,
            record.OptionB,
            record.UserSelect,
            upper);
    }
    else
    { DrawEventMsg(record.Text, upper); }
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
    const wchar_t* optionA,
    const wchar_t* optionB,
    uint8_t        cursor,
    bool           upper
)
{
    m_Writer.DrawLine(m_pContext, msg, 0, upper);

    if (cursor == 0)
    { SetActiveColor(); }
    else
    { SetDefaultColor(); }
    m_Writer.DrawLine(m_pContext, optionA, 2, upper);

    if (cursor == 1)
    { SetActiveColor(); }
    else
    { SetDefaultColor(); }
    m_Writer.DrawLine(m_pContext, optionB, 3, upper);

    SetDefaultColor();
}

//-----------------------------------------------------------------------------
//      アクティブカラーを設定します.
//-----------------------------------------------------------------------------
void EventSystem::SetActiveColor()
{
    m_Writer.SetColor(
        kActiveColor[0],
        kActiveColor[1],
        kActiveColor[2],
        kActiveColor[3]);
}

//-----------------------------------------------------------------------------
//      デフォルトカラーを設定します.
//-----------------------------------------------------------------------------
void EventSystem::SetDefaultColor()
{
    m_Writer.SetColor(
        kTextColor[0],
        kTextColor[1],
        kTextColor[2],
        kTextColor[3]);
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
            auto eventMsg = msg.GetAs<EventData>();

            // シナリオが変わったら，イベントテーブルを読み込み.
            if (eventMsg->ScenarioId != m_ScenarioId)
            {
                if (!LoadScenario(eventMsg->ScenarioId))
                { break; }
            }

            // 描画フラグを立てる.
            m_IsDraw = true;

            // シナリオIDとイベントIDを更新.
            m_ScenarioId  = eventMsg->ScenarioId;
            m_EventId     = eventMsg->EventId;

            assert(m_Table.find(m_EventId) != m_Table.end());
            auto& record = m_Table.at(m_EventId);

            // 分岐アリならブロードキャストしておく.
            if (record.HasBrunch)
            {
                Message msg(MESSAGE_ID_EVENT_BRUNCH);
                SendMsg(msg);
            }
        }
        break;

    case MESSAGE_ID_EVENT_END:
        {
            // 描画フラグをおろす.
            m_IsDraw = false;
        }
        break;

    case MESSAGE_ID_EVENT_UPDATE_CURSOR:
        {
            auto cursor = *msg.GetAs<uint8_t>();
            auto& record = m_Table.at(m_EventId);
            record.UserSelect = cursor;
        }
        break;
    }
}

//-----------------------------------------------------------------------------
//      シナリオをロードします.
//-----------------------------------------------------------------------------
bool EventSystem::LoadScenario(uint32_t scenarioId)
{
    // そんなにシナリオ作らないから線形探索.
    auto idx = -1;
    for(auto i=0; i<_countof(kEventTablePath); ++i)
    {
        if (kEventTablePath[i].ScenarioId == scenarioId)
        {
            idx = i;
            break;
        }
    }

    if (idx == -1)
    {
        ELOGA("Error : Not Found ScenarioId = %u", scenarioId);
        assert(false); // ありえないので止める.
        return false;
    }

    std::string path;
    if (!asdx::SearchFilePathA(kEventTablePath[idx].Path, path))
    {
        ELOGA("Error : Not Found EventTable. ScenarioId = %u", scenarioId);
        assert(false); // ありえないので止める.
        return false;
    }

    std::map<uint32_t, EventRecord> table;

    // シナリオデータ読み込み.
    if (!LoadEventTable(path.c_str(), table))
    {
        ELOGA("Error : LoadEventTable() Failed. path = %s", path.c_str());
        return false;
    }

    // テーブル差し替え.
    m_Table = table;

    return true;
}