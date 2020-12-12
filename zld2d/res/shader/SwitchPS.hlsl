//-----------------------------------------------------------------------------
// File : SceneSwitchPS.hlsl
// Desc : Pixel Shader For Scene Switching.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

///////////////////////////////////////////////////////////////////////////////
// CbPinHole constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbSwitch : register(b0)
{
    float4 Color;
    float2 Center;
    float  Radius;
    float  AspectRatio;
};

//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
float4 main(const VSOutput input) : SV_TARGET
{
    float4 color = Color;

    if (Radius > 0.0)
    { color.a = smoothstep(Radius - 0.1f, Radius, length(Center - input.TexCoord)); }

    return color;
}