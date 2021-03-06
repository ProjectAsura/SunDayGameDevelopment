﻿//-----------------------------------------------------------------------------
// File : MessageId.h
// Desc : Message ID.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

// ゲーム固有のメッセージ.
enum MESSAGE_ID : unsigned int
{
    MESSAGE_ID_NONE = 0,            // 分類無し.
    MESSAGE_ID_MAP_SCROLL,          // スクロールによるマップ遷移.
    MESSAGE_ID_MAP_SWITCH,          // 場面切り替えによるマップ遷移.
    MESSAGE_ID_MAP_CHANGED,         // マップ切り替え完了.
    MESSAGE_ID_MAP_REQUEST,         // マップ切り替え要求.
    MESSAGE_ID_PLAYER_DAMAGE,       // プレイヤーへのダメージ.
    MESSAGE_ID_PLAYER_DEAD,         // プレイヤーのHPが0になったとき.
    MESSAGE_ID_ENEMY_DEAD,          // 敵のHPが0になったとき.
    MESSAGE_ID_EVENT_RAISE,         // イベント処理.
    MESSAGE_ID_EVENT_BRUNCH,        // 分岐メッセージ.
    MESSAGE_ID_EVENT_NEXT,          // 次のイベントを要求.
    MESSAGE_ID_EVENT_END,           // イベント終了.
    MESSAGE_ID_EVENT_USER_REACTION, // ユーザーによる選択肢確定.
    MESSAGE_ID_EVENT_UPDATE_CURSOR, // 選択肢カーソル更新.
    MESSAGE_ID_SWITCHER_REQUEST,    // スイッチャーに要求.
    MESSAGE_ID_SWITCHER_COMPLETE,   // スイッチャー処理終了. 
};
