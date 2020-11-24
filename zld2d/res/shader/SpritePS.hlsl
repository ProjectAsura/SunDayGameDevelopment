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
    float3  Position : POSITION;        //!< �ʒu���W�ł�.
    float4  Color    : VTXCOLOR;        //!< ���_�J���[�ł�.
    float3  TexCoord : TEXCOORD;        //!< �e�N�X�`�����W�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4  Position : SV_POSITION;     //!< �ʒu���W�ł�.
    float4  Color    : VTXCOLOR;        //!< ���_�J���[�ł�.
    float3  TexCoord : TEXCOORD0;       //!< �e�N�X�`�����W�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4  Color    : SV_TARGET0;      //!< �J���[�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// CbPerOnceVS constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbPerOnceVS : register( b0 )
{
    float4x4 TransformMtx;   //!< �ϊ��s��ł�.
};

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
Texture2D       TextureMap : register( t0 );
SamplerState    TextureSmp : register( s0 );


//-----------------------------------------------------------------------------
//! @brief      �s�N�Z���V�F�[�_���C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
PSOutput main( VSOutput input )
{
    PSOutput output = (PSOutput)0;

    // �e�N�X�`���t�F�b�`�����J���[�ƒ��_�J���[����Z.
    output.Color = TextureMap.Sample( TextureSmp, input.TexCoord.xy ) * input.Color;

    if (output.Color.a <= 0.0f)
    { discard; }

    // �����_�[�^�[�Q�b�g�ɏo��.
    return output;
}
