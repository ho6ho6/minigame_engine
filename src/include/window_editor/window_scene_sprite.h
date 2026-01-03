/*sceneにテクスチャを落とし込むために構造体spriteを宣言*/

#ifndef SCENE_SPRITE_H
#define SCENE_SPRITE_H

#include <string>
#include "include/assets/texture.h"


// sceneに表示するための構造体
struct SceneSprite
{
    std::string name;     // スプライト名（テクスチャ名と同じで良い）
    Texture* texture = 0;    // テクスチャハンドル

    // posはTransformコンポーネントから参照
    float pos_x = 0.0f;   // シーン内のX座標
    float pos_y = 0.0f;   // シーン内のY座標
    
    float dragOffsetX = 0.0f; // ドラッグ中のオフセットX
    float dragOffsetY = 0.0f; // ドラッグ中のオフセットY
    
    int width = 16;  // スプライトの幅
    int height = 16; // スプライトの高さ
    int z_order = 1;   // 描画順序（大きいほど前面に描画）
	int64_t id = 0;   // ユニークID
    bool selected = false; // 選択状態
};

// hierarchyに登録するための構造体
struct SceneToHierarchyObj {
    std::string name;
	Texture* texture = 0;           // テクスチャハンドル
    float x = 0.0f;
    float y = 0.0f;             // world (左上基準(最終的には中央を原点にする), logical px)
    int width = 0;
    int height = 0;             // px
    int z_order = 0;
    bool registered = false;    // Componentが追加されたか
    int64_t id;                // entity id
    bool selected = false;
};

#endif // !SCENE_SPRITE_H
