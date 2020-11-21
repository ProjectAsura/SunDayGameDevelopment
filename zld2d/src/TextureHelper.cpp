//-----------------------------------------------------------------------------
// File : TextureHelper.cpp
// Desc : Texture Helper.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <TextureHelper.h>
#include <asdxMisc.h>
#include <asdxLogger.h>
#include <asdxDeviceContext.h>


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