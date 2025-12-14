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
    float dragOffsetX = 0.0f; // ドラッグ中のオフセットX
    float dragOffsetY = 0.0f; // ドラッグ中のオフセットY
    int width = 16;  // スプライトの幅
    int height = 16; // スプライトの高さ
    int z_order = 1;   // 描画順序（大きいほど前面に描画）
    bool selected = false; // 選択状態
};

// hierarchyに登録するための構造体
struct SceneToHierarchyNode {
    uint64_t id;           // unique id
    std::string name;
    Texture* texture;      // nullable
    float x, y;            // world (content-left-top 基準, logical px)
    int width, height;     // px
    int z_order;
    bool selected;
};

#endif // !SCENE_SPRITE
