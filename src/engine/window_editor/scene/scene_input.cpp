#include "include/window_editor/window_scene.h"
#include "include/window_editor/scene/scene_input.h"
#include "include/component/component_api.h"
#include "include/component/game_component.h"

#include <iostream>

namespace n_windowscene::input
{
    /* 当たり判定/選択/ドラッグは、ScreenToSceneを使用*/
    ImVec2 ScreenToScene(const SceneViewContext& ctx)
    {
        // 左上の原点に変換されている 最終的には中心を原点にする
        const ImVec2 local{
            ctx.input.mouseScreen.x - ctx.content.screenPos.x,
            ctx.input.mouseScreen.y - ctx.content.screenPos.y
        };

        const ImVec2 localLogical{
            local.x / ctx.content.fbScale.x,
            local.y / ctx.content.fbScale.y
        };

        return {
            localLogical.x + ctx.camera.viewOffset.x,
            localLogical.y + ctx.camera.viewOffset.y
        };
    }

    // 右クリックを動かす処理を論理ピクセルで
    void HandlePan(SceneViewContext& ctx)
    {
        if (!ctx.input.rightDragging) return;

        const ImVec2 deltaFb = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

        ctx.camera.viewOffset.x -= deltaFb.x / ctx.content.fbScale.x;
        ctx.camera.viewOffset.y -= deltaFb.y / ctx.content.fbScale.y;

        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
    }

    // クリックされたら
    int HandleClick(SceneViewContext& ctx, bool mouseInContent, std::vector<SceneSprite>& sprites)
    {
        if (!mouseInContent || !ctx.input.leftClicked)
            return -1;

        std::cout << "Clicked" << std::endl;

        int clickedIndex = PickSpriteIndex(ctx, sprites);

        for (auto& s : sprites)
            s.selected = false;

        if (clickedIndex >= 0)
            sprites[clickedIndex].selected = true;

        return clickedIndex;
    }

    // どのアセットが選択されたか？
    int PickSpriteIndex(const SceneViewContext& ctx, const std::vector<SceneSprite>& sprites)
    {
        ImVec2 mouseScene = ScreenToScene(ctx);

        // もっとも最初に中心座標を左上か中心にするかを決めてなかった弊害
        for (int i = (int)sprites.size() - 1; i >= 0; --i)
        {
            const auto& s = sprites[i];

            const float halfW = s.width * 0.5f;
            const float halfH = s.height * 0.5f;

            const float minX = s.pos_x - halfW;
            const float maxX = s.pos_x + halfW;
            const float minY = s.pos_y - halfH;
            const float maxY = s.pos_y + halfH;

            if (mouseScene.x >= minX && mouseScene.x <= maxX &&
                mouseScene.y >= minY && mouseScene.y <= maxY)
            {
                std::cout << "PickObjectIndex = " << i << std::endl;
                return i;
            }
        }
        return -1;
    }

    // ある程度動いたらドラッグ開始
    void FindDraggingObj(SceneViewContext& ctx, std::vector<SceneSprite>& sprites)
    {

        // 選択されたものを見つける
        if (!ctx.selection.dragging && ctx.input.sceneActive && ctx.input.leftDown && ctx.input.leftDragging && ctx.input.mouseInSprite)
        {
            std::cout << "Find Obj" << std::endl;
            for (auto& s : sprites)
            {
                if (s.selected)
                {
                    input::BeginDrag(ctx, s);
                    break;
                }
            }
        }
    }

    // ドラッグ処理
    void BeginDrag(SceneViewContext& ctx, const SceneSprite& sprite)
    {
        ImVec2 mouseScene = ScreenToScene(ctx);
        std::cout << "Begin Dragg" << std::endl;
        ctx.selection.dragging = true;
        ctx.selection.draggingEntity = sprite.id;
        ctx.selection.dragOffset.x = mouseScene.x - sprite.pos_x;
        ctx.selection.dragOffset.y = mouseScene.y - sprite.pos_y;
    }

    // 移動を適用
    void ApplyTransformToECS(SceneViewContext& ctx, std::vector<SceneSprite>& sprites)
    {
        // ドラッグ中：SceneSprite だけ更新
        if (ctx.selection.dragging && ctx.input.sceneActive && ctx.input.leftDragging)
        {
            std::cout << "Apply PreMove" << std::endl;
            for (auto& s : sprites)
            {
                ComputeDragPosition(ctx, s);
            }
            return;
        }

        // ドラッグ終了フレームで ECS に反映
        if (ctx.input.leftReleased && ctx.selection.dragging)
        {
            std::cout << "Apply Move" << std::endl;
            for (auto& s : sprites)
            {
                if (s.id != ctx.selection.draggingEntity) continue;

                auto tOpt = n_compoapi::GetTransformComponent(s.id);
                if (!tOpt) continue;

                auto t = *tOpt;
                t.position[0] = s.pos_x;
                t.position[1] = s.pos_y;

                n_gamecomponent::instance_gameFunctions()
                    .EnqueueGameCommand([eid = s.id, t]()
                        {
                            n_compoapi::SetTransformComponent(eid, t);
                        });
            }

            ctx.selection.dragging = false;
            ctx.selection.draggingEntity = NOT_SELECTED;
        }
    }

    void EndDrag(SceneViewContext& ctx)
    {
        ctx.selection.dragging = false;
    }

    // ドラッグ移動を行う
    bool ComputeDragPosition(const SceneViewContext& ctx, SceneSprite& sprite)
    {
        if (!ctx.selection.dragging) return false;
        if (!sprite.selected) return false;

        ImVec2 mouseScene = ScreenToScene(ctx);

        sprite.pos_x = mouseScene.x - ctx.selection.dragOffset.x;
        sprite.pos_y = mouseScene.y - ctx.selection.dragOffset.y;

        return true;
    }
}