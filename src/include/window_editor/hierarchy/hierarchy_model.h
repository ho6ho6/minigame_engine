/*ヒエラルキーの中でも[状態]に関するものを管理*/

#ifndef HIERARCHY_MODEL_H
#define HIERARCHY_MODEL_H

//#define NOT_SELECTED 0

#include <vector>
#include "include/window_editor/window_scene_sprite.h"
#include "include/component/componentDefaults.h" // EntityIdのみ
#include <mutex>
#include <optional>
#include <unordered_map>

namespace n_hierarchy
{
    // Sceneに配置されたオブジェクト(sprite)とEntityを紐づける
	// 実際にゲームとして扱うのはEntity　spriteは直接操作しない
    //struct EntitySpritePair
    //{
    //    EntityId entity;
    //    EntityId sprite;
    //};


	class hierarchyModel
	{
        public:
			// Sceneに配置されたオブジェクトを追加・削除
            void AddObject(const SceneToHierarchyObj& obj);
            void RemoveObject(EntityId id);

            std::vector<SceneToHierarchyObj> Snapshot() const;

            // window_sceneから直接選択された時にも、選択されるように
			void SetSelectedEntityFromScene(EntityId eid);

			// 選択中のエンティティIDを設定・取得
            //void SelectEntity(EntityId id);
            //EntityId GetSelectedEntity_ONLYUI() const; // UI専用

			// キャッシュされたエンティティとスプライトIDのペアを取得
            //void UpdateCache(std::vector<EntitySpritePair>&& pairs);
            //std::vector<EntitySpritePair> GetCachedPairs() const;

			// キャッシュを無効化
            //void InvalidateCache();

			// エンティティIDとスプライトIDのマッピングを更新・取得
            //void UpdateSpriteMap(EntityId eid, EntityId spriteId);
			//std::optional<EntityId> GetSpriteIdForEntity(EntityId eid) const;

        private:
            mutable std::mutex mtx_;
            std::vector<SceneToHierarchyObj> objects_;
            //std::unordered_map<EntityId, EntityId> entityToSprite_;
            //EntityId selected_ = NOT_SELECTED;
            //bool cacheValid_ = false;
            //std::vector<EntitySpritePair> cachedPairs_;
    };
}

#endif