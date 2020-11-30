//-----------------------------------------------------------------------------
// File : TextWriter.h
// Desc ; Text Writer.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cassert>
#include <TextWriter.h>
#include <asdxLogger.h>


///////////////////////////////////////////////////////////////////////////////
// TextWriter class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
TextWriter::TextWriter()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
TextWriter::~TextWriter()
{ Term(); }

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool TextWriter::Init
(
    IDWriteFactory*     pFactory,
    ID2D1DeviceContext* pContext,
    const wchar_t*      fontName,
    float               fontSize
)
{
    if (pFactory == nullptr || fontName == nullptr || fontSize <= 0.0f)
    {
        ELOGA("Error : Invalid Argument");
        return false;
    }

    // テキストフォーマットの生成.
    auto hr = pFactory->CreateTextFormat(
        fontName,
        nullptr,
        DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"",
        m_Format.GetAddress() );
    if (FAILED(hr))
    {
        ELOGA("Error : IDWriteFactory::CreateTextFormat() Failed. errcode = 0x%x", hr);
        return false;
    }

    // カラーブラシを生成.
    hr = pContext->CreateSolidColorBrush( D2D1::ColorF(D2D1::ColorF::White), m_Brush.GetAddress() );
    if ( FAILED( hr ) )
    {
        ELOG("Error : ID2D1DeviceContext::CreateSolidBrush() Failed. errcode = 0x%x", hr);
        return false;
    }

    m_FontSize = fontSize;

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void TextWriter::Term()
{
    m_Brush .Reset();
    m_Format.Reset();
}

//-----------------------------------------------------------------------------
//      カラーを設定します.
//-----------------------------------------------------------------------------
void TextWriter::SetColor(float r, float g, float b, float a)
{
    D2D_COLOR_F color = {};
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    m_Brush->SetColor(color);
}

//-----------------------------------------------------------------------------
//      テキストを描画します.
//-----------------------------------------------------------------------------
void TextWriter::Draw(ID2D1DeviceContext* pContext, const wchar_t* text, float x, float y)
{
    UINT length = UINT(wcslen(text));

    D2D_RECT_F rect = {};
    rect.left   = x;
    rect.top    = y;
    rect.right  = x + m_FontSize * 2.0f * length;
    rect.bottom = y + m_FontSize;

    // 左寄せ.
    m_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    pContext->DrawText(text, length, m_Format.GetPtr(), &rect, m_Brush.GetPtr());
}

//-----------------------------------------------------------------------------
//      テキストを描画します.
//-----------------------------------------------------------------------------
void TextWriter::DrawLine(ID2D1DeviceContext* pContext, const wchar_t* text, int line, bool upper)
{
    assert(0 <= line && line < 4);
    UINT length = UINT(wcslen(text));

    static const float kW = 958.0f;
    static const float kH = 42.0f; // 32文字サイズ + 10px上下間隔.
    static const float kX = 182.0f;
    const float kY = (upper) ? 86.0f : 504.0f;

    D2D_RECT_F rect = {};
    rect.left   = kX;
    rect.top    = kY + line * kH;
    rect.right  = kX + kW;
    rect.bottom = kY + m_FontSize + line * kH;

    // 中央寄せ
    m_Format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    pContext->DrawText(text, length, m_Format.GetPtr(), &rect, m_Brush.GetPtr());
}

//-----------------------------------------------------------------------------
//      フォントサイズを取得します.
//-----------------------------------------------------------------------------
float TextWriter::GetFontSize() const
{ return m_FontSize; }