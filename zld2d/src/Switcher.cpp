//-----------------------------------------------------------------------------
// File : Switcher.cpp
// Desc : Scene Switcher.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <Switcher.h>
#include <MessageId.h>
#include <asdxLogger.h>


namespace {

//-----------------------------------------------------------------------------
// Constant Values.
//-----------------------------------------------------------------------------
#include "../res/shader/Compiled/SwitchPS.inc"


///////////////////////////////////////////////////////////////////////////////
// SwitchParam structure
///////////////////////////////////////////////////////////////////////////////
struct SwitchParam
{
    asdx::Vector4 Color;
    asdx::Vector2 Center;
    float         Radius;
    float         AspectRatio;
};

} // namespace

///////////////////////////////////////////////////////////////////////////////
// Switcher class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
Switcher::Switcher()
: m_Color       (1.0f, 1.0f, 1.0f, 1.0f)
, m_Center      (0.5f, 0.5f)
, m_Radius      (-1.0f)
, m_TargetTime  (5.0f)
, m_CurrTime    (0.0f)
, m_Type        (SWITCH_TYPE_NONE)
, m_NextPhase   (false)
{
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
Switcher::~Switcher()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理です.
//-----------------------------------------------------------------------------
bool Switcher::Init(ID3D11Device* pDevice)
{
    MessageMgr::Instance().Add(this);
    auto hr = pDevice->CreatePixelShader(SwitchPS, sizeof(SwitchPS), nullptr, m_PS.GetAddress());
    if (FAILED(hr))
    {
        ELOGA("Error : ID3D11Device::CreatePixelShader() Failed. errcode = 0x%x", hr);
        return false;
    }

    if (!m_CB.Init(pDevice, sizeof(SwitchParam)))
    {
        ELOGA("Error : ConstantBuffer::Init() Failed.");
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理です.
//-----------------------------------------------------------------------------
void Switcher::Term()
{
    MessageMgr::Instance().Remove(this);
    m_PS.Reset();
    m_CB.Term();
}

//-----------------------------------------------------------------------------
//      更新処理です.
//-----------------------------------------------------------------------------
void Switcher::Update(float elapsedTime, const asdx::Vector2& player)
{
    if (m_Type == SWITCH_TYPE_NONE)
    { return; }

    m_CurrTime += elapsedTime;
    m_Center = player;

    auto amount = asdx::Saturate(m_CurrTime / m_TargetTime);

    if (m_Type == SWITCH_TYPE_FADE)
    {
        auto s = asdx::Lerp(0.0f, 1.0f, amount);
        m_Color.w = (m_NextPhase) ? 1.0f - s : s;
        m_Radius  = -1.0f;

        if (s >= 1.0f)
        {
            if (!m_NextPhase)
            {
                // マップ更新要求.
                Message msg(MESSAGE_ID_MAP_REQUEST);
                SendMsg(msg);

                m_Color.w = 1.0f;
            }
            else
            {
                // マップ更新完了通知.
                Message msg(MESSAGE_ID_SWITCHER_COMPLETE);
                SendMsg(msg);

                m_Color.w   = 0.0f;
                m_NextPhase = false;
                m_CurrTime  = 0.0f;
                m_Type      = SWITCH_TYPE_NONE;
            }
        }
    }
    else if (m_Type == SWITCH_TYPE_HOLE)
    {
        auto s = asdx::Lerp(0.0f, 1.0f, amount);
        m_Radius  = (m_NextPhase) ? s * 1.5f : 1.5f - s * 1.5f;
        m_Color.w = 1.0f;

        if (s >= 1.0f)
        {
            if (!m_NextPhase)
            {
                // マップ更新要求.
                Message msg(MESSAGE_ID_MAP_REQUEST);
                SendMsg(msg);

                m_Radius = 0.0f;
            }
            else
            {
                // マップ更新完了通知.
                Message msg(MESSAGE_ID_SWITCHER_COMPLETE);
                SendMsg(msg);

                m_NextPhase = false;
                m_CurrTime  = 0.0f;
                m_Type      = SWITCH_TYPE_NONE;
            }
        }
    }
}

//-----------------------------------------------------------------------------
//      描画するかどうか?
//-----------------------------------------------------------------------------
bool Switcher::IsDraw() const
{ return m_Type != SWITCH_TYPE_NONE; }

//-----------------------------------------------------------------------------
//      シェーダをバインドします.
//-----------------------------------------------------------------------------
void Switcher::Bind(ID3D11DeviceContext* pContext)
{
    SwitchParam res = {};
    res.Color       = m_Color;
    res.Center      = m_Center;
    res.Radius      = m_Radius;
    res.AspectRatio = m_AspectRatio;

    auto pCB = m_CB.GetBuffer();
    pContext->UpdateSubresource(pCB, 0, nullptr, &res, 0, 0);
    pContext->PSSetConstantBuffers(0, 1, &pCB);
    pContext->PSSetShader(m_PS.GetPtr(), nullptr, 0);
}

//-----------------------------------------------------------------------------
//      シェーダのバインドを解除します.
//-----------------------------------------------------------------------------
void Switcher::Unbind(ID3D11DeviceContext* pContext)
{
    pContext->PSSetShader(nullptr, nullptr, 0);
}

//-----------------------------------------------------------------------------
//      メッセージ受信処理.
//-----------------------------------------------------------------------------
void Switcher::OnMessage(const Message& msg)
{
    auto type = msg.GetType();
    switch(type)
    {
    case MESSAGE_ID_SWITCHER_REQUEST:
        {
            // 処理中は受け付けないようにガード.
            if (m_Type == SWITCH_TYPE_NONE)
            {
                auto data = msg.GetAs<SwitchData>();
                m_Color.x    = data->Color.x;
                m_Color.y    = data->Color.y;
                m_Color.z    = data->Color.z;
                m_Type       = data->Type;
                m_TargetTime = data->Time;
                m_CurrTime   = 0.0f;
                m_NextPhase  = false;

                if (m_Type == SWITCH_TYPE_FADE)
                {
                    m_Color.w = 0.0f;
                    m_Radius  = -1.0f;
                }
                else if (m_Type == SWITCH_TYPE_HOLE)
                {
                    m_Color.w = 1.0f;
                    m_Radius  = 2.0f;
                }
            }
        }
        break;

    case MESSAGE_ID_MAP_CHANGED:
        {
            m_NextPhase = true;
            m_CurrTime  = 0.0f;
        }
        break;
    }
}