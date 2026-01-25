#ifndef SCENE_INPUT_H
#define SCENE_INPUT_H

#include <imgui.h>
#include "include/window_editor/window_scene_sprite.h"
#include "include/window_editor/scene/scene_ctx.h"
#include <vector>

namespace n_windowscene::input
{
    /* 当たり判定/選択/ドラッグは、ScreenToSceneを使用*/
    ImVec2 ScreenToScene(const SceneViewContext& ctx);

    // 右クリックを動かす処理を論理ピクセルで
    void HandlePan(SceneViewContext& ctx);

    // クリックされたら
    int HandleClick(SceneViewContext& ctx, bool mouseInContent, std::vector<SceneSprite>& sprites);

    // どのアセットが選択されたか？
    int PickSpriteIndex(const SceneViewContext& ctx, const std::vector<SceneSprite>& sprites);

    // ある程度動いたらドラッグ開始
    void FindDraggingObj(SceneViewContext& ctx, std::vector<SceneSprite>& sprites);

    // ドラッグ処理
    void BeginDrag(SceneViewContext& ctx, const SceneSprite& sprite);

    // 移動を適用
    void ApplyTransformToECS(SceneViewContext& ctx, std::vector<SceneSprite>& sprites);

    void EndDrag(SceneViewContext& ctx);

    // ドラッグ移動
    bool ComputeDragPosition(const SceneViewContext& ctx, SceneSprite& sprite);
}

#endif // !SCENE_INPUT_H
