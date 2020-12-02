//-----------------------------------------------------------------------------
// File : TextureMgr.cpp
// Desc : Texture Manager.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cassert>
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <asdxDeviceContext.h>
#include <TextureMgr.h>


namespace {

//-----------------------------------------------------------------------------
//      テクスチャをロードします.
//-----------------------------------------------------------------------------
bool LoadTexture2D(const char* path, asdx::Texture2D& result)
{
    std::string texturePath;
    if (!asdx::SearchFilePathA(path, texturePath))
    {
        ELOGA("Error : File Not Found. path = %s", path);
        return false;
    }

    asdx::ResTexture res;
    if (!res.LoadFromFileA(texturePath.c_str()))
    {
        ELOGA("Error : ResTexsture::LoadFromFileA() Failed. path = %s", texturePath.c_str());
        return false;
    }

    auto pDevice  = asdx::DeviceContext::Instance().GetDevice();
    auto pContext = asdx::DeviceContext::Instance().GetContext();
    if (!result.Create(pDevice, pContext, res))
    {
        ELOGA("Error : Texture2D::Create() Failed. path = %s", texturePath.c_str());
        return false;
    }

    return true;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////
// TextureMgr class
///////////////////////////////////////////////////////////////////////////////
TextureMgr TextureMgr::s_Instance = {};

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
TextureMgr::TextureMgr()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
TextureMgr::~TextureMgr()
{ Term(); }

//-----------------------------------------------------------------------------
//      シングルトンインスタンスを取得します.
//-----------------------------------------------------------------------------
TextureMgr& TextureMgr::Instance()
{ return s_Instance; }

//-----------------------------------------------------------------------------
//      テクスチャをロードします.
//-----------------------------------------------------------------------------
bool TextureMgr::Load(uint32_t count, const char** paths)
{
    m_Textures.resize(count);
    for(auto i=0u; i<count; ++i)
    {
        if (!LoadTexture2D(paths[i], m_Textures[i]))
        { return false; }
    }

    return true;
}

//-----------------------------------------------------------------------------
//      テクスチャを破棄します.
//-----------------------------------------------------------------------------
void TextureMgr::Term()
{
    for(size_t i=0; i<m_Textures.size(); ++i)
    { m_Textures[i].Release(); }

    m_Textures.clear();
}

//-----------------------------------------------------------------------------
//      テクスチャを取得します.
//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* TextureMgr::GetSRV(uint32_t index)
{
    assert(index < m_Textures.size());
    return m_Textures[index].GetSRV();
}