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
            localLogical.x + ctx.camera->viewOffset.x,
            localLogical.y + ctx.camera->viewOffset.y
        };
    }

    // 右クリックを動かす処理を論理ピクセルで
    void HandlePan(SceneViewContext& ctx)
    {
        // 右クリックを動かす処理を論理ピクセルで
        if (ctx.input.rightDragging)
        {
            ImVec2 dragFreamebuffer = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right); // 累積 delta

            // framebuffer -> 論理ピクセル変換
            float dragX = dragFreamebuffer.x / ctx.content.fbScale.x;
            float dragY = dragFreamebuffer.y / ctx.content.fbScale.y;

            ctx.camera->viewOffset.x -= dragX;
            ctx.camera->viewOffset.y -= dragY;

            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right); // 次フレームは 0 から計測

        }
    }

    // クリックされたら
    int HandleClick(SceneViewContext& ctx, bool mouseInContent, std::vector<SceneSprite>& sprites)
    {

        ImVec2 mouseScene = ScreenToScene(ctx);

        std::cout << "Clicked" << std::endl;

        int clickedIndex = PickSpriteIndex(ctx, sprites, mouseScene);

        for (size_t i = 0; i < sprites.size(); ++i)
        {
            sprites[i].selected = ((int)i == clickedIndex);
            if (sprites[i].selected)
            {
                // ドラッグオフセットを記録
                sprites[i].dragOffsetX = mouseScene.x - sprites[i].pos_x;
                sprites[i].dragOffsetY = mouseScene.y - sprites[i].pos_y;
            }
        }

       return clickedIndex;
    }

    // どのアセットが選択されたか？
    int PickSpriteIndex(const SceneViewContext& ctx, const std::vector<SceneSprite>& sprites, ImVec2 mouseScene)
    {

        // もっとも最初に中心座標を左上か中心にするかを決めてなかった弊害
        for (int i = (int)sprites.size() - 1; i >= 0; --i)
        {
            const auto& s = sprites[i];
            if (mouseScene.x >= s.pos_x && mouseScene.x <= s.pos_x + s.width &&
                mouseScene.y >= s.pos_y && mouseScene.y <= s.pos_y + s.height)
            {
                std::cout << "PickObjectIndex = " << i << std::endl;
                return i;
            }
        }
        return -1;
    }

    // 移動を適用 (マウスで操作されたもの) システム寄りのTransformの入り口 dataスクリプトに移動しても良い気がするけど
    void ApplyTransformToECS(SceneViewContext& ctx, std::vector<SceneSprite>& sprites)
    {
        // ドラッグ移動は選択済みのみに
        if (ctx.input.leftDown && ctx.input.leftDragging)
        {
            ImVec2 curMouseScene = input::ScreenToScene(ctx);
            for (auto& s : sprites)
            {
                if (!s.selected) continue;
                float newX = curMouseScene.x - s.dragOffsetX;
                float newY = curMouseScene.y - s.dragOffsetY;

                // Transform 更新
                auto tOpt = n_compoapi::GetTransformComponent(s.id);
                if (tOpt)
                {
                    auto t = *tOpt;
                    t.position[0] = newX;
                    t.position[1] = newY;
                    n_gamecomponent::instance_gameFunctions().EnqueueGameCommand([eid = s.id, t]() {
                        n_compoapi::SetTransformComponent(eid, t);
                        });
                }
            }
        }
    }
}