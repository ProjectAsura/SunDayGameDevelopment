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
#include <TextureHelper.h>
#include <gimmick/Block.h>


///////////////////////////////////////////////////////////////////////////////
// GameApp class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
GameApp::GameApp()
: asdx::Application(L"Sample Game", 1280, 720, nullptr, nullptr, nullptr)
{
    m_SwapChainFormat   = DXGI_FORMAT_R10G10B10A2_UNORM;
    m_EnableMultiSample = false;
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
    if (!m_Sprite.Init(m_pDevice, float(m_Width), float(m_Height)))
    {    
        ELOGA("Error : Sprite::Init() Failed.");
        return false;
    }

    // ゲームマップテクスチャ初期化.
    if (!GameMapTextureMgr::Instance().Init())
    {
        ELOGA("Error : GameMapTextureMgr::Init() Failed.");
        return false;
    }

    // プレイヤー初期化
    if (!m_Player.Init())
    {
        ELOGA("Error : Player::Init() Failed");
        return false;
    }

    // HUD初期化.
    if (!m_Hud.Init())
    {
        ELOGA("Error : Hud::Init() Failed.");
        return false;
    }

    if (!m_EnemyTest.Init())
    {
        return false;
    }

    //m_Block.Init( 300, 400, 64, 64, DIRECTION_RIGHT, GetGameMap(GAMEMAP_TEXTURE_ROCK));

    auto block = new Block();
    block->SetTilePos(4, 5).SetSize(64, 64).SetSRV(GetGameMap(GAMEMAP_TEXTURE_BLOCK));
    //block->SetDir(DIRECTION_RIGHT);
    m_Map.AddGimmick(block);

    // テスト用タイルデータ.
    {
        uint8_t id = 0;
        for(uint8_t i=0; i<kTileCountY; ++i)
        for(uint8_t j=0; j<kTileCountX; ++j)
        {
            if (i == 0 || j == 0 || j == (kTileCountX - 1) || i == (kTileCountY - 1)
                || (i == 3 && j == 4) || ( i == 8 && j == 11 ))
            {
                Tile tile = {};
                tile.Moveable = false;
                tile.Transition = false;
                tile.TextureId = GAMEMAP_TEXTURE_TREE;
                m_Map.SetTile(id, tile);
            }
            else
            {
                Tile tile = {};
                tile.Moveable = true;
                tile.Transition = false;
                tile.TextureId = GAMEMAP_TEXTURE_PLANE;
                m_Map.SetTile(id, tile);
            }

            id++;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void GameApp::OnTerm()
{
    m_Sprite.Term();
    m_Player.Term();
    m_Hud   .Term();

    m_EnemyTest.Term();
    m_Map.ClearGimmicks();

    GameMapTextureMgr::Instance().Term();
}

//-----------------------------------------------------------------------------
//      フレーム遷移を行います.
//-----------------------------------------------------------------------------
void GameApp::OnFrameMove(asdx::FrameEventArgs& args)
{
    // パッド情報を更新.
    m_Pad.UpdateState();

    UpdateContext context = {};
    context.ElapsedSec  = float(args.ElapsedTime);
    context.Pad         = &m_Pad;
    context.Map         = &m_Map;

    // プレイヤー更新.
    m_Player.Update(context);

    // マップ更新.
    m_Map.Update(context);

    // 敵更新.
    m_EnemyTest.Update(context);

    // ダメージ判定があった場合.
    auto dead = false;
    if (context.Damage)
    { dead = m_Player.ReceiveDamage(); }

    // 死んでしまった場合.
    if (dead)
    {
    }
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
        auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::Default);

        float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT mask = 0xffffff;
        m_pDeviceContext->OMSetBlendState(pBS, blendFactor, mask);
        m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);
        m_Sprite.Begin(m_pDeviceContext);

        // マップ描画.
        m_Map.Draw(m_Sprite, m_Player.GetBox().Pos.y);

        // 敵描画.
        m_EnemyTest.Draw(m_Sprite);

        // キャラ描画.
        m_Player.Draw(m_Sprite);

        // UI描画.
        m_Hud.Draw(m_Sprite, m_Player);

        m_Sprite.End(m_pDeviceContext);

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

