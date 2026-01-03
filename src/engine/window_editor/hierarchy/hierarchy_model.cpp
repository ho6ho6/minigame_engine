/*
1. データ保持 キャッシュ
2. mutex管理
3. UIとゲームの中立的なデータ取得
*/

#include "include/window_editor/hierarchy/hierarchy_model.h"
#include <algorithm>

namespace n_hierarchy
{
	
	/* ---Sceneに配置されたオブジェクトを追加・削除--- */

	void hierarchyModel::AddObject(const SceneToHierarchyObj& obj)
	{
		// スレッドセーフに追加
		std::lock_guard<std::mutex> lock(mtx_);
		printf("AddObject this=%p id=%lld name=%s\n", this, (long long)obj.id, obj.name.c_str());
		objects_.push_back(obj);
		printf(" AddObject done, objects_.size=%zu\n", objects_.size());
	}

	void hierarchyModel::RemoveObject(EntityId eid)
	{
		std::lock_guard<std::mutex> lock(mtx_);

		// スレッドセーフに削除
		auto before = objects_.size();

		objects_.erase(
			std::remove_if(objects_.begin(), objects_.end(),
				[eid](const SceneToHierarchyObj& obj) {
					return obj.id == eid;   // 削除するオブジェクトを特定
				}),
			objects_.end()
		);

		printf("[HierarchyModel] Remove id=%lld before=%zu after=%zu\n",
			(long long)eid, before, objects_.size());

	}

	/* ---Sceneに配置されたオブジェクトを追加・削除--- */



	/* ---Sceneに配置されたオブジェクトのスナップショットを取得--- */

	std::vector<SceneToHierarchyObj> hierarchyModel::Snapshot() const
	{
		//printf("Snapshot this=%p, sizeof(hierarchyModel)=%zu\n", this, sizeof(hierarchyModel));

		std::lock_guard<std::mutex> lock(mtx_);

		//printf("Snapshot this=%p, objects_.size=%zu\n", this, objects_.size()); 
		for (size_t i = 0; i < objects_.size(); ++i) 
		{ 
			const auto& o = objects_[i]; 
			//printf(" Snapshot[%zu] id=%lld name=%s registered=%d selected=%d\n", i, (long long)o.id, o.name.c_str(), (int)o.registered, (int)o.selected); 
		}

		return objects_;	// 安全にコピーを返す
	}

	/* ---Sceneに配置されたオブジェクトのスナップショットを取得--- */



	/* ---window_sceneから直接選択された時にも、選択されるように--- */ 
	
	void hierarchyModel::SetSelectedEntityFromScene(EntityId eid)
	{
		//EntityId の決定は window_scene 側で行う 
		std::lock_guard<std::mutex> lock(mtx_);
		for(auto& obj : objects_)
		{
			obj.selected = (obj.id == eid);
		}
	}

	/* ---window_sceneから直接選択された時にも、選択されるように--- */ 



	/* ---選択中のエンティティIDを設定・取得--- */

	//void hierarchyModel::SelectEntity(EntityId eid)
	//{
	//	std::lock_guard<std::mutex> lock(mtx_);
	//	selected_ = eid;
	//}

	//EntityId hierarchyModel::GetSelectedEntity_ONLYUI() const
	//{
	//	std::lock_guard<std::mutex> lock(mtx_);
	//	return selected_;
	//}

	/* ---選択中のエンティティIDを設定・取得--- */



	/* ---キャッシュされたエンティティとスプライトIDのペアを取得--- */ 

	//void hierarchyModel::UpdateCache(std::vector<EntitySpritePair>&& pairs)
	//{
	//	std::lock_guard<std::mutex> lock(mtx_);
	//	cachedPairs_ = std::move(pairs);
	//	cacheValid_ = true;
	//}

	// cacheは外部(sync/ui)で管理する
	//std::vector<EntitySpritePair> hierarchyModel::GetCachedPairs() const
	//{
	//	std::lock_guard<std::mutex> lock(mtx_);
	//	return cachedPairs_;
	//}

	// cacheを無効化した時は古いデータも消す
	//void hierarchyModel::InvalidateCache()
	//{
	//	std::lock_guard<std::mutex> lock(mtx_);
	//	cacheValid_ = false;
	//	cachedPairs_.clear();
	//}

	/* ---キャッシュされたエンティティとスプライトIDのペアを取得--- */

}