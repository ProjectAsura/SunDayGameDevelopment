//-----------------------------------------------------------------------------
// File : SurfacePS.hlsl
// Desc : Surface Effect.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

///////////////////////////////////////////////////////////////////////////////
// CbSurface constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbSurface : register(b0)
{
    float4x4 ColorMatrix;
};

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
Texture2D       ColorMap : register(t0);
SamplerState    ColorSmp : register(s0);

//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
float4 main(const VSOutput input) : SV_TARGET
{
    float4 color = ColorMap.SampleLevel(ColorSmp, input.TexCoord, 0);
    return mul(ColorMatrix, color);
}