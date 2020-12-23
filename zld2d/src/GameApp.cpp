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
#include <asdxColorMatrix.h>
#include <gimmick/Block.h>
#include <TextureId.h>
#include <TextureMgr.h>


namespace {

#include "../res/shader/Compiled/SurfaceVS.inc"
#include "../res/shader/Compiled/SurfacePS.inc"

// TEXTURE_IDと同じ順番で定義すること!!
static const char* kTexturePath[] = {
    // 単色系.
    "../res/texture/map/plane.tga",

    // Map系.
    "../res/texture/map/rock.tga",
    "../res/texture/map/tree.tga",
    "../res/texture/map/block.tga",

    // HUD系.
    "../res/texture/hud/star_full.tga",
    "../res/texture/hud/star_lack.tga",
    "../res/texture/hud/wnd.tga",
    "../res/texture/hud/select_cursor.tga",
    "../res/texture/hud/hole.tga",
};

} // namespace

///////////////////////////////////////////////////////////////////////////////
// GameApp class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
GameApp::GameApp()
: asdx::Application(L"Sample Game", 1280, 720, nullptr, nullptr, nullptr)
{
    m_SwapChainFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
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
    if (!TextureMgr::Instance().Load(_countof(kTexturePath), kTexturePath))
    {
        ELOGA("Error : GameMapTextureMgr::Init() Failed.");
        return false;
    }

    // イベントシステム初期化.
    if (!m_EventSystem.Init(m_pFactoryDW.GetPtr(), m_pDeviceContext2D.GetPtr()))
    {
        ELOGA("Error : EventSystem::Init() Failed.");
        return false;
    }

    // メッセージマネージャ初期化.
    {
        // 32個までキューイング可能.
        auto size = (sizeof(Message) + sizeof(EventData)) * 32;

        if (!MessageMgr::Instance().Init(size))
        {
            ELOGA("Error : MessageMgr::Init() Failed.");
            return false;
        }
    }

    // プレイヤー初期化
    if (!m_Player.Init())
    {
        ELOGA("Error : Player::Init() Failed");
        return false;
    }

    // スイッチャー初期化.
    if (!m_Switcher.Init(m_pDevice))
    {
        ELOGA("Error : Switcher::Init() Failed.");
        return false;
    }

    //if (!m_EnemyTest.Init())
    //{
    //    return false;
    //}

    //m_Block.Init( 300, 400, 64, 64, DIRECTION_RIGHT, GetGameMap(GAMEMAP_TEXTURE_ROCK));

    auto block = new Block();
    block->SetTilePos(4, 5).SetSize(64, 64).SetSRV(GetTexture(TEXTURE_MAP_BLOCK));
    //block->SetDir(DIRECTION_RIGHT);
    m_MapData.Gimmicks.push_back(block);

    // テスト用タイルデータ.
    {
        uint8_t id = 0;
        for(uint8_t i=0; i<kTileCountY; ++i)
        for(uint8_t j=0; j<kTileCountX; ++j)
        {
            if ((i == 5 && j == 0) || (i == 0 && j == 10) || (j == (kTileCountX - 1) && i == 5) || (j == 10 && i == (kTileCountY - 1)))
            {
                Tile tile = {};
                tile.Moveable = true;
                tile.Switchable = false;
                tile.Scrollable = true;
                tile.TextureId = TEXTURE_WHITE;
                m_MapData.Tile[id] = tile;
            }
            else if (i == 0 || j == 0 || j == (kTileCountX - 1) || i == (kTileCountY - 1)
                || (i == 3 && j == 4) || ( i == 8 && j == 11 ))
            {
                Tile tile = {};
                tile.Moveable = false;
                tile.Switchable = false;
                tile.Scrollable = false;
                tile.TextureId = TEXTURE_MAP_TREE;
                m_MapData.Tile[id] = tile;
            }
            else
            {
                Tile tile = {};
                tile.Moveable = true;
                tile.Scrollable = false;
                tile.Switchable = false;
                tile.TextureId = TEXTURE_WHITE;
                m_MapData.Tile[id] = tile;
            }

            id++;
        }
    }

    m_MapSystem.SetData(&m_MapData);
    m_MapSystem.SetNext(&m_MapData);


    // カラーターゲット生成.
    {
        asdx::TargetDesc2D desc;
        desc.Width              = m_Width;
        desc.Height             = m_Height;
        desc.ArraySize          = 1;
        desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.MipLevels          = 1;
        desc.SampleDesc.Count   = 1;
        desc.SampleDesc.Quality = 0;

        if (!m_SceneColor.Create(m_pDevice, desc))
        {
            ELOGA("Error : ColorTarget2D::Create() Failed.");
            return false;
        }
    }

    // フルスクリーン矩形生成.
    {
        struct Vertex
        {
            asdx::Vector2   Position;
            asdx::Vector2   TexCoord;
        };

        const Vertex kTriangleVertices[] = {
            { asdx::Vector2(-1.0f,  1.0f), asdx::Vector2(0.0f, 0.0f) },
            { asdx::Vector2( 3.0f,  1.0f), asdx::Vector2(2.0f, 0.0f) },
            { asdx::Vector2(-1.0f, -3.0f), asdx::Vector2(0.0f, 2.0f) },
        };

        if (!m_TriangleVB.Init(m_pDevice, sizeof(kTriangleVertices), sizeof(Vertex), kTriangleVertices))
        {
            ELOGA("Error : VertexBuffer::Init() Failed.");
            return false;
        }

        const Vertex kQuadVertices[] = {
            { asdx::Vector2(-1.0f,  1.0f), asdx::Vector2(0.0f, 0.0f) },
            { asdx::Vector2( 1.0f,  1.0f), asdx::Vector2(1.0f, 0.0f) },
            { asdx::Vector2( 1.0f, -1.0f), asdx::Vector2(1.0f, 1.0f) },

            { asdx::Vector2(-1.0f,  1.0f), asdx::Vector2(0.0f, 0.0f) },
            { asdx::Vector2( 1.0f, -1.0f), asdx::Vector2(1.0f, 1.0f) },
            { asdx::Vector2(-1.0f, -1.0f), asdx::Vector2(0.0f, 1.0f) },
        };

        if (!m_QuadVB.Init(m_pDevice, sizeof(kQuadVertices), sizeof(Vertex), kQuadVertices))
        {
            ELOGA("Error : VertexBuffer::Init() Failed.");
            return false;
        }
    }

    // フルスクリーン用頂点シェーダ生成.
    {
        D3D11_INPUT_ELEMENT_DESC elements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        if (!m_SurfaceVS.Init(m_pDevice, SurfaceVS, sizeof(SurfaceVS), _countof(elements), elements))
        {
            ELOGA("Error : VertexShader::Init() Failed.");
            return false;
        }
    }


    // コピー用
    {
        auto hr = m_pDevice->CreatePixelShader(SurfacePS, sizeof(SurfacePS), nullptr, m_SurfacePS.GetAddress());
        if (FAILED(hr))
        {
            ELOGA("Error : ID3D11Device::CreatePixelShader() Failed. errcode = 0x%x", hr);
            return false;
        }

        if (!m_SurfaceCB.Init(m_pDevice, sizeof(asdx::Matrix)))
        {
            ELOGA("Error : ConstantBuffer::Init() Failed.");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void GameApp::OnTerm()
{
    m_TriangleVB.Term();

    m_SurfaceVS.Term();
    m_SurfacePS.Reset();
    m_SurfaceCB.Term();

    m_SceneColor.Release();

    m_Sprite.Term();
    m_Player.Term();

    m_Switcher.Term();

    //m_EnemyTest.Term();

    m_EventSystem.Term();
    MessageMgr::Instance().Term();
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
    context.Map         = &m_MapSystem;
    context.ScenarioId  = m_EventSystem.GetScenarioId();
    context.EventId     = m_EventSystem.GetEventId();
    context.IsEvent     = m_EventSystem.IsEvent();

    // プレイヤー更新.
    m_Player.Update(context);

    // マップ更新.
    m_MapSystem.Update(context);

    //// 敵更新.
    //m_EnemyTest.Update(context);

    // スイッチャー更新.
    {
        auto w = float(m_Width);
        auto h = float(m_Height);
        auto x = (m_Player.GetBox().Pos.x + m_Player.GetBox().Size.x * 0.5f) / w;
        auto y = (m_Player.GetBox().Pos.y + m_Player.GetBox().Size.y * 0.5f) / h;

        m_Switcher.Update(float(args.ElapsedTime), asdx::Vector2(x, y));
    }

    // メッセージ実行.
    MessageMgr::Instance().Process();

    // カラー行列更新.
    {
        auto pCB    = m_SurfaceCB.GetBuffer();
        auto matrix = asdx::Matrix::CreateIdentity();
        m_pDeviceContext->UpdateSubresource(pCB, 0, nullptr, &matrix, 0, 0);
    }
}

//-----------------------------------------------------------------------------
//      フレーム描画を行います.
//-----------------------------------------------------------------------------
void GameApp::OnFrameRender(asdx::FrameEventArgs& args)
{
    auto pMainRTV = m_SceneColor.GetTargetView();
    auto pRTV = m_ColorTarget2D.GetTargetView();
    auto pDSV = m_DepthTarget2D.GetTargetView();
    if (pRTV == nullptr || pDSV == nullptr || pMainRTV == nullptr)
    { return; }

    // 会話メッセージを表示する場合.
    auto showMsg = true;
    auto upper = (m_Player.GetBox().Pos.y >= 424);
    {
        // ターゲットをクリア.
        float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        m_pDeviceContext->ClearRenderTargetView(pMainRTV, m_ClearColor);
        m_pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

        m_pDeviceContext->OMSetRenderTargets(1, &pMainRTV, pDSV);

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX   = kMarginX;
        viewport.TopLeftY   = kMarginY;
        viewport.Width      = kTileTotalW;
        viewport.Height     = kTileTotalH;
        viewport.MinDepth   = 0.0f;
        viewport.MaxDepth   = 1.0f;

        D3D11_RECT scissor = {};
        scissor.left        = kMarginX;
        scissor.right       = kMarginX + kTileTotalW;
        scissor.top         = kMarginY;
        scissor.bottom      = kMarginY + kTileTotalH;

        m_pDeviceContext->RSSetViewports(1, &viewport);
        m_pDeviceContext->RSSetScissorRects(1, &scissor);

        auto pSmp = asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearClamp);
        auto pBS  = asdx::RenderState::GetInstance().GetBS(asdx::BlendType::AlphaBlend);
        auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::Default);

        float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT mask = 0xffffff;
        m_pDeviceContext->OMSetBlendState(pBS, blendFactor, mask);
        m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);

        // スプライト描画開始.
        m_Sprite.Begin(m_pDeviceContext);
        m_Sprite.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        // マップ描画.
        m_MapSystem.Draw(m_Sprite, m_Player.GetBox().Pos.y);

        //// 敵描画.
        //m_EnemyTest.Draw(m_Sprite);

        // キャラ描画.
        m_Player.Draw(m_Sprite);

        // スプライト描画終了.
        m_Sprite.End(m_pDeviceContext);
    }

    // スワップチェインに描画.
    {
        m_pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX   = 0;
        viewport.TopLeftY   = 0;
        viewport.Width      = m_Width;
        viewport.Height     = m_Height;
        viewport.MinDepth   = 0.0f;
        viewport.MaxDepth   = 1.0f;

        D3D11_RECT scissor = {};
        scissor.left        = 0;
        scissor.right       = m_Width;
        scissor.top         = 0;
        scissor.bottom      = m_Height;

        m_pDeviceContext->RSSetViewports(1, &viewport);
        m_pDeviceContext->RSSetScissorRects(1, &scissor);

        auto pBS  = asdx::RenderState::GetInstance().GetBS(asdx::BlendType::AlphaBlend);
        auto pDSS = asdx::RenderState::GetInstance().GetDSS(asdx::DepthType::Default);

        float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT mask = 0xffffff;
        m_pDeviceContext->OMSetBlendState(pBS, blendFactor, mask);
        m_pDeviceContext->OMSetDepthStencilState(pDSS, 0);

        // 描画シーンを貼り付け.
        {
            auto pSmp = asdx::RenderState::GetInstance().GetSmp(asdx::SamplerType::LinearClamp);
            auto pSRV = m_SceneColor.GetShaderResource();
            auto pCBV = m_SurfaceCB.GetBuffer();

            m_pDeviceContext->PSSetShader(m_SurfacePS.GetPtr(), nullptr, 0);
            m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
            m_pDeviceContext->PSSetSamplers(0, 1, &pSmp);
            m_pDeviceContext->PSSetConstantBuffers(0, 1, &pCBV);

            DrawTriangle();
        }

        // シーン切り替え.
        if (m_Switcher.IsDraw())
        {
            m_Switcher.Bind(m_pDeviceContext);
            DrawTriangle();
            m_Switcher.Unbind(m_pDeviceContext);
        }

        // スプライト描画開始.
        m_Sprite.Begin(m_pDeviceContext);
        m_Sprite.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        // HUD表示.
        m_Hud.Draw(m_Sprite, m_Player);

        // メッセージウィンドウ枠表示.
        m_EventSystem.DrawWindow(m_Sprite, upper);

        // スプライト描画終了.
        m_Sprite.End(m_pDeviceContext);

        ID3D11ShaderResourceView* pNullSRV[] = { nullptr };
        m_pDeviceContext->PSSetShaderResources(0, 1, pNullSRV);
    }

    // メッセージ描画.
    if (showMsg)
    {
        auto context = m_pDeviceContext2D.GetPtr();
        context->BeginDraw();
        context->SetTarget(m_pBitmap2D.GetPtr());

        m_EventSystem.DrawMsg(context, upper);

        m_pDeviceContext2D->EndDraw();
    }

    // 画面に表示.
    Present(1);
}

//-----------------------------------------------------------------------------
//      矩形を描画します.
//-----------------------------------------------------------------------------
void GameApp::DrawTriangle()
{
    auto pVB    = m_TriangleVB.GetBuffer();
    auto stride = m_TriangleVB.GetStride();
    auto offset = 0u;

    m_SurfaceVS.Bind(m_pDeviceContext);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    m_pDeviceContext->Draw(3, 0);
    m_SurfaceVS.UnBind(m_pDeviceContext);
}

//-----------------------------------------------------------------------------
//      矩形を描画します.
//-----------------------------------------------------------------------------
void GameApp::DrawQuad()
{
    auto pVB    = m_QuadVB.GetBuffer();
    auto stride = m_QuadVB.GetStride();
    auto offset = 0u;

    m_SurfaceVS.Bind(m_pDeviceContext);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
    m_pDeviceContext->Draw(6, 0);
    m_SurfaceVS.UnBind(m_pDeviceContext);
}

//-----------------------------------------------------------------------------
//      リサイズ処理を行います.
//-----------------------------------------------------------------------------
void GameApp::OnResize(const asdx::ResizeEventArgs& args)
{
    m_SceneColor.Resize(m_pDevice, args.Width, args.Height);
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


