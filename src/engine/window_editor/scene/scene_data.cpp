#include "include/window_editor/scene/scene_data.h"
#include "include/window_editor/window_hierarchy.h"
#include "include/window_editor/hierarchy/hierarchy_sync.h"
#include "include/component/component_api.h"
#include "include/component/game_component.h"
#include <algorithm>

namespace n_windowscene::data
{
    // 遅延キューに追加されたオブジェクトを保存
	void EnqueuePayloadFromAssetsWindow(SceneViewContext& ctx, std::vector<std::string>& m_PendingDrop, std::vector<ImVec2>& m_PendingDropPos)
	{
        // assetsのドラッグ＆ドロップを受け取る
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PAYLOAD"))
            {
                const char* data = reinterpret_cast<const char*>(payload->Data);
                int data_size = payload->DataSize;

                std::string asset_name(data, data + data_size); // 文字列化

                if (!asset_name.empty() && asset_name.back() == '\0') asset_name.pop_back();

                // マウス座標　論理座標
                ImVec2 mouse_pos = ImGui::GetMousePos();

                // 論理ピクセルで保存
                ImVec2 localLogical = ImVec2((mouse_pos.x - ctx.content.screenPos.x) / ctx.content.fbScale.x,
                    (mouse_pos.y - ctx.content.screenPos.y) / ctx.content.fbScale.y);

                ImGui::EndDragDropTarget();


                // 遅延キューへ登録（ImGui の外で処理する） クラッシュの原因回避
                m_PendingDrop.push_back(std::move(asset_name));
                m_PendingDropPos.push_back(localLogical);

            }
            else
            {
                ImGui::EndDragDropTarget();
            }
        }
	}

    // オブジェクト削除を通知
    void DeleteAssetFromScene(EntityId eid, std::vector<SceneSprite>& sprites)
    {
        if (eid == NOT_SELECTED) return;
        printf("[window_scene/DeletAsFrScene] DeleteObjFromScene id=%llu\n", (signed long long)eid);

        // コンテナから削除 EntityIdで検索　添え字で削除するとズレてしまい、ヒエラルキーにオブジェクトが残る
        sprites.erase(
            std::remove_if(
                sprites.begin(),
                sprites.end(),
                [eid](const SceneSprite& s)
                {
                    return s.id == eid;
                }),
            sprites.end()
        );

        // ヒエラルキーウィンドウへ
        n_hierarchy::sync::OnSceneObjectRemoved(eid);
    }

}