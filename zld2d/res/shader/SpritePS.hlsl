//-----------------------------------------------------------------------------
// File : SpritePS_Tex2D.hlsl
// Desc : Spirite Pixel Shader For 2D Texture.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3  Position : POSITION;        //!< 位置座標です.
    float4  Color    : VTXCOLOR;        //!< 頂点カラーです.
    float3  TexCoord : TEXCOORD;        //!< テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4  Position : SV_POSITION;     //!< 位置座標です.
    float4  Color    : VTXCOLOR;        //!< 頂点カラーです.
    float3  TexCoord : TEXCOORD0;       //!< テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4  Color    : SV_TARGET0;      //!< カラーです.
};

///////////////////////////////////////////////////////////////////////////////
// CbPerOnceVS constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbPerOnceVS : register( b0 )
{
    float4x4 TransformMtx;   //!< 変換行列です.
};

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
Texture2D       TextureMap : register( t0 );
SamplerState    TextureSmp : register( s0 );


//-----------------------------------------------------------------------------
//! @brief      ピクセルシェーダメインエントリーポイントです.
//-----------------------------------------------------------------------------
PSOutput main( VSOutput input )
{
    PSOutput output = (PSOutput)0;

    // テクスチャフェッチしたカラーと頂点カラーを乗算.
    output.Color = TextureMap.Sample( TextureSmp, input.TexCoord.xy ) * input.Color;

    if (output.Color.a <= 0.0f)
    { discard; }

    // レンダーターゲットに出力.
    return output;
}
