//-----------------------------------------------------------------------------
// File : GameApp.cpp
// Desc : Game Application.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <GameApp.h>
#include <asdxLogger.h>
#include <asdxRenderState.h>

// 横17マス.
// 縦11マス

///////////////////////////////////////////////////////////////////////////////
// GameApp class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
GameApp::GameApp()
: asdx::Application(L"Sample Game", 960, 540, nullptr, nullptr, nullptr)
{
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
GameApp::~GameApp()
{
}

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool GameApp::OnInit()
{
    // プレイヤー0として設定.
    m_Pad.SetPlayerIndex(0);

    // スプライトシステム初期化.
    {
        m_Sprite = new asdx::Sprite();
       if (!m_Sprite->Init(m_pDevice, m_Width, m_Height))
       {    
            ELOGA("Error : Sprite::Init() Failed.");
            return false;
       }
    }

    // プレイヤー初期化
    if (!m_Player.Init())
    {
        ELOGA("Error : Player::Init() Failed");
        return false;
    }


    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void GameApp::OnTerm()
{
    if (m_Sprite != nullptr)
    {
        m_Sprite->Term();
        delete m_Sprite;
        m_Sprite = nullptr;
    }
}

//-----------------------------------------------------------------------------
//      フレーム遷移を行います.
//-----------------------------------------------------------------------------
void GameApp::OnFrameMove(asdx::FrameEventArgs& args)
{
    // パッド情報を更新.
    m_Pad.UpdateState();

    m_Player.Update(m_Pad, args.ElapsedTime);
}

//-----------------------------------------------------------------------------
//      フレーム描画を行います.
//-----------------------------------------------------------------------------
void GameApp::OnFrameRender(asdx::FrameEventArgs& args)
{
    auto pRTV = m_ColorTarget2D.GetTargetView();
    auto pDSV = m_DepthTarget2D.GetTargetView();
    if (pRTV == nullptr || pDSV == nullptr)
    { return; }

    // ターゲットをクリア.
    m_pDeviceContext->ClearRenderTargetView(pRTV, m_ClearColor);
    m_pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

    m_pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
    m_pDeviceContext->RSSetViewports(1, &m_Viewport);
    m_pDeviceContext->RSSetScissorRects(1, &m_ScissorRect);

    // スワップチェインに描画.
    {
        auto pSmp = asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearClamp);
        auto pBS  = asdx::RenderState::GetInstance().GetBS(asdx::BlendType::AlphaBlend);

        float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT mask = 0xffffff;
        m_pDeviceContext->OMSetBlendState(pBS, blendFactor, mask);
        m_Sprite->Begin(m_pDeviceContext, asdx::Sprite::SHADER_TYPE_TEXTURE2D);

        m_Player.Draw(m_Sprite);

        m_Sprite->End(m_pDeviceContext);
    }

    // 画面に表示.
    Present(1);
}

//-----------------------------------------------------------------------------
//      リサイズ処理を行います.
//-----------------------------------------------------------------------------
void GameApp::OnResize(const asdx::ResizeEventArgs& args)
{
}

//-----------------------------------------------------------------------------
//      キー処理を行います.
//-----------------------------------------------------------------------------
void GameApp::OnKey(const asdx::KeyEventArgs& args)
{
}

//-----------------------------------------------------------------------------
//      マウス処理を行います.
//-----------------------------------------------------------------------------
void GameApp::OnMouse(const asdx::MouseEventArgs& args)
{
}

//-----------------------------------------------------------------------------
//      タイピング処理を行います.
//-----------------------------------------------------------------------------
void GameApp::OnTyping(uint32_t keyCode)
{
}

