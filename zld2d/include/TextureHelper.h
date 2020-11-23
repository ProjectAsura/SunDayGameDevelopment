﻿//-----------------------------------------------------------------------------
// File : TextureHelper.h
// Desc : Texture Helper.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <asdxTexture.h>


//-----------------------------------------------------------------------------
//! @brief      テクスチャをロードします.
//!
//! @param[in]      path        ファイルパス
//! @param[out]     result      テクスチャの格納先です.
//! @retval true    ロードに成功
//! @retval false   ロードに失敗.
//-----------------------------------------------------------------------------
bool LoadTexture2D(const char* path, asdx::Texture2D& result);
