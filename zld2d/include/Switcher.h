//-----------------------------------------------------------------------------
// File : Switcher.h
// Desc : Scene Switcher.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <d3d11_4.h>
#include <asdxMath.h>
#include <asdxConstantBuffer.h>
#include <MessageMgr.h>


///////////////////////////////////////////////////////////////////////////////
// SWITCH_TYPE enum
///////////////////////////////////////////////////////////////////////////////
enum SWITCH_TYPE : uint8_t
{
    SWITCH_TYPE_NONE = 0,
    SWITCH_TYPE_FADE = 1,
    SWITCH_TYPE_HOLE = 2,
};

///////////////////////////////////////////////////////////////////////////////
// SwitchData structure
///////////////////////////////////////////////////////////////////////////////
struct SwitchData
{
    asdx::Vector3   Color;      //!< 色.
    SWITCH_TYPE     Type;       //!< 切り替えタイプ.
    float           Time;       //!< 遷移時間(秒).
};

///////////////////////////////////////////////////////////////////////////////
// Switcher class
///////////////////////////////////////////////////////////////////////////////
class Switcher : public IMessageListener
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
    Switcher();
    ~Switcher();
    bool Init(ID3D11Device* pDevice);
    void Term();
    void Update(float elaspedTime, const asdx::Vector2& player);
    bool IsDraw() const;
    void Bind(ID3D11DeviceContext* pContext);
    void Unbind(ID3D11DeviceContext* pContext);
    void OnMessage(const Message& msg) override;

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    asdx::Vector4                   m_Color;
    asdx::Vector2                   m_Center;
    float                           m_Radius;
    float                           m_AspectRatio;
    asdx::ConstantBuffer            m_CB;
    asdx::RefPtr<ID3D11PixelShader> m_PS;
    float                           m_TargetTime;
    float                           m_CurrTime;
    SWITCH_TYPE                     m_Type;
    bool                            m_NextPhase;

    //=========================================================================
    // private methods.
    //=========================================================================
    /* NOTHING */
};
