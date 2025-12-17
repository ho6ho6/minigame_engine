#include "include/game.hpp"
#include "include/game_component.hpp"
#include "include/input.hpp"
#include "include/render.hpp"
#include "include/window_editor/window_scene.hpp"
#include "include/window_editor/window_hierarchy.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <limits>
#undef max

namespace n_game
{
	/*--数値計算--*/
	
	// シンプルな距離判定
	static float DistanceSq(const Vec3& a, const Vec3& b)
	{
		float dx = a[0] - b[0];
		float dy = a[1] - b[1];
		float dz = a[2] - b[2];

		return dx * dx + dy * dy + dz * dz;
	}

	// playerPosition は現在のプレイヤー座標
	void game::ProcessTriggers(const Vec3& playerPosition)
	{
		for (auto& kv : n_gamecomponent::instance_gameFunctions().finishComponents) {
			int64_t finishId = kv.first;
			n_component::FinishComponent& f = kv.second;
			if (!f.active) continue;

			// window_scene のオブジェクト座標を優先
			Vec3 finishPos;
			bool hasScenePos = n_windowscene::instance_winSce().GetSpritePosition(finishId, finishPos);

			if (!hasScenePos) finishPos = f.position;

			float r2 = f.radius * f.radius;
			// 判定は playerPosition と finishPos を比較
			if (DistanceSq(playerPosition, finishPos) <= r2) {

				// 最も近いプレイヤー／敵を探す
				int64_t resolvedPlayerId = -1;
				int64_t resolvedEnemyId = -1;
				float bestPlayerDist2 = std::numeric_limits<float>::max();
				float bestEnemyDist2 = std::numeric_limits<float>::max();

				for (const auto& pkv : n_gamecomponent::instance_gameFunctions().isplayerComponents) {
					int64_t entityId = pkv.first;
					const n_component::IsPlayerComponent& flag = pkv.second;

					// スプライト位置を取得できなければスキップ
					Vec3 entPos;
					if (!n_windowscene::instance_winSce().GetSpritePosition(entityId, entPos)) continue;

					float d2 = DistanceSq(entPos, finishPos);

					if (flag.IsPlayerActive) {
						if (d2 < bestPlayerDist2) {
							bestPlayerDist2 = d2;
							resolvedPlayerId = flag.PlayerId; // ゲーム内のプレイヤーIDを使う
						}
					}
					if (flag.IsEnemyActive) {
						if (d2 < bestEnemyDist2) {
							bestEnemyDist2 = d2;
							resolvedEnemyId = entityId; // 敵は entityId を使うなど設計次第
						}
					}
				}

				// プレイヤー優先で HandleFinish を呼ぶ（存在しなければ敵 or unknown）
				if (resolvedPlayerId >= 0 && bestPlayerDist2 <= r2) {
					HandleFinish(resolvedPlayerId, finishId, f);
				}
				else if (resolvedEnemyId >= 0 && bestEnemyDist2 <= r2) {
					// 敵がゴールした扱いにするなら playerId = -2 等の特別値か別ハンドラを使う
					HandleFinish(-2 /*enemy marker*/, finishId, f);
				}
				else {
					HandleFinish(-1 /*unknown*/, finishId, f);
				}

				if (f.oneShot) f.active = false;
			}
		}
	}

	void game::HandleFinish(int64_t playerId, int64_t finishId, const n_component::FinishComponent& f)
	{
		if (playerId >= 0)
		{
			printf("Player  Finish \n");
			// スコア表示、演出など <- これもコンポーネントで行う予定
		}
		else
		{
			printf("Where is Everybody ??\n");
		}
	}
}
