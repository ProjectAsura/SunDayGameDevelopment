﻿//-----------------------------------------------------------------------------
// File : TextWriter.h
// Desc : Text Writer.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxRef.h>
#include <d2d1_1.h>
#include <dwrite_1.h>


///////////////////////////////////////////////////////////////////////////////
// TextWriter class
///////////////////////////////////////////////////////////////////////////////
class TextWriter
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
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    TextWriter();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~TextWriter();

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //-------------------------------------------------------------------------
    bool Init(IDWriteFactory* pFactory, ID2D1DeviceContext* pContext, const wchar_t* fontName, float size);

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      カラーを設定します.
    //-------------------------------------------------------------------------
    void SetColor(float r, float g, float b, float a);

    //-------------------------------------------------------------------------
    //! @brief      テキストを描画します.
    //-------------------------------------------------------------------------
    void Draw(ID2D1DeviceContext* pContext, const wchar_t* text, float x, float y);

    //-------------------------------------------------------------------------
    //! @brief      テキストを描画します.
    //-------------------------------------------------------------------------
    void DrawLine(ID2D1DeviceContext* pContext, const wchar_t* text, int line, bool upper = false);

    //-------------------------------------------------------------------------
    //! @brief      フォントサイズを取得します.
    //-------------------------------------------------------------------------
    float GetFontSize() const;

    //-------------------------------------------------------------------------
    //! @brief      縁取りカラーを設定します.
    //-------------------------------------------------------------------------
    void SetOutlineColor(float r, float g, float b, float a);

    //-------------------------------------------------------------------------
    //! @brief      縁取りフラグを設定します.
    //-------------------------------------------------------------------------
    void SetOutline(bool enable);

    //-------------------------------------------------------------------------
    //! @brief      縁取りフラグを取得します.
    //-------------------------------------------------------------------------
    bool GetOutline() const;

private:
    //=========================================================================
    // private varaibles.
    //=========================================================================
    asdx::RefPtr<IDWriteTextFormat>     m_Format;
    asdx::RefPtr<ID2D1SolidColorBrush>  m_Brush;
    asdx::RefPtr<ID2D1SolidColorBrush>  m_OutlineBrush;
    float                               m_FontSize;
    bool                                m_Outline;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};
