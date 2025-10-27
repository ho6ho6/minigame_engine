/*sceneにテクスチャを落とし込むために構造体spriteを宣言*/

#ifndef SCENE_SPRITE
#define SCENE_SPRITE

#pragma once
#include <string>
#include "include/assets/texture.hpp"


// sceneに表示するための構造体
struct SceneSprite
{
    std::string name;     // スプライト名（テクスチャ名と同じで良い）
    Texture* texture;    // テクスチャハンドル
    float pos_x = 0.0f;   // シーン内のX座標
    float pos_y = 0.0f;   // シーン内のY座標
    float width = 32.0f;  // スプライトの幅
    float height = 32.0f; // スプライトの高さ
    int z_order = 0;   // 描画順序（大きいほど前面に描画）
    bool selected = false; // 選択状態
};

#endif // !SCENE_SPRITE
