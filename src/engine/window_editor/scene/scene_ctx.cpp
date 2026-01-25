#include "include/window_editor/scene/scene_ctx.h"
#include "include/window_editor/scene/scene_input.h"


void BuildSceneViewContext(SceneViewContext& ctx)
{
    static bool prevLeftDown = false;
    static bool prevRightDown = false;
    bool nowLeftDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool nowRightDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);
    ImGuiIO& io = ImGui::GetIO();

    ImVec2 min = ctx.content.screenPos;
    ImVec2 max = {
        ctx.content.screenPos.x + ctx.content.size.x,
        ctx.content.screenPos.y + ctx.content.size.y
    };

    ctx.input.mouseInSprite = ctx.input.mouseScreen.x >= min.x && ctx.input.mouseScreen.x <= max.x &&
                              ctx.input.mouseScreen.y >= min.y && ctx.input.mouseScreen.y <= max.y;

    ctx.content.screenPos   = ImGui::GetCursorScreenPos();
    ctx.content.size        = ImGui::GetContentRegionAvail();
    ctx.content.fbScale     = io.DisplayFramebufferScale;

    ctx.input.mouseScreen   = ImGui::GetMousePos();

    // マウスが押された？
    ctx.input.leftDown      = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    ctx.input.rightDown     = ImGui::IsMouseDown(ImGuiMouseButton_Right);
    
    // マウスがImGuiウィンドウの内部にある？
    ctx.input.leftClicked   = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    ctx.input.rightClicked  = ImGui::IsMouseClicked(ImGuiMouseButton_Right);
    
    // マウスをリリースした？
    ctx.input.leftReleased  = (!nowLeftDown && prevLeftDown);
    ctx.input.leftDown      = nowLeftDown;
    ctx.input.rightReleased = (!nowRightDown && prevRightDown);
    ctx.input.rightDown     = nowRightDown;

    // マウスをドラッグしているか？
    ctx.input.leftDragging  = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
    ctx.input.rightDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Right);
}
