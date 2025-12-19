#include "include/game.hpp"
#include "include/input.hpp"
#include "include/render.hpp"
#include "include/window_editor/window_scene.hpp"
#include "include/window_editor/window_hierarchy.hpp"
#include "include/component/component_api.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <limits>
#undef max

struct MoveComponent {
	float speed;
	Vec3 direction;
	float acceleration;
	float jump;
	int jumpKey; // デフォルト Space のキーコード（環境に合わせて変更
};

struct RigidbodyComponent
{
	float gravity;
	bool isGround;
};


// 状態保持（エッジ検出用）
static std::unordered_map<int64_t, bool> s_wasJumpPressed;

namespace n_game
{
	game& instance_game()
	{
		static game inst;
		return inst;
	}


	void game::Render(int64_t entityId, std::optional<int> spriteIdOpt)
	{
		auto mcOpt = n_compoapi::GetMoveComponent(entityId);
		if (!mcOpt) return;
		auto mc = *mcOpt;

		if (mc.jumpKey < 0) {
			s_wasJumpPressed.erase(entityId);
			return;
		}

		// キー状態取得とエッジ検出
		SHORT state = GetAsyncKeyState(mc.jumpKey);
		bool isDown = (state & 0x8000) != 0;
		bool wasDown = false;
		auto it = s_wasJumpPressed.find(entityId);
		if (it != s_wasJumpPressed.end()) wasDown = it->second;

		// 押下開始（エッジ）でジャンプを発生させる
		if (isDown && !wasDown) {
			// 地上判定を確認してからジャンプ
			auto rbOpt = n_compoapi::GetRigidbodyComponent(entityId);
			bool grounded = false;
			if (rbOpt) grounded = rbOpt->isGround;

			if (grounded) {
				n_input::UpdateInputAndJumpForAll();
				// デバッグログ
				printf("[game::Render] entity=%lld jump triggered (jump=%f)\n", (long long)entityId, mc.jump);
			}
		}

		// spriteId があればエフェクトやデバッグ表示に使う
		if (spriteIdOpt) {
			int spriteId = *spriteIdOpt;
			// 例: ジャンプ時にエフェクトをスプライト位置に出す、またはデバッグ枠を描くためのフラグを立てる
			// n_render_registry::RequestEffectAtSprite(spriteId, EffectType::Jump); // 仮API
		}

		// 必要なら MoveComponent の状態を更新して戻す（例: lastJumpTime）
		// mc.lastJumpTime = n_time::Time::Now();
		// n_compoapi::SetMoveComponent(entityId, mc);

		// 押下状態を保存
		s_wasJumpPressed[entityId] = isDown;

	}


	/*--数値計算--*/
	
	// シンプルな距離判定
	static float DistanceSq(const Vec2& a, const Vec2& b)
	{
		float dx = a[0] - b[0];
		float dy = a[1] - b[1];

		return dx * dx + dy * dy;
	}

	// Ground 判定：PhysicsComponent の grounded フラグを参照する実装例

	/*このIsGroundedとApplyJumpImpulse*/
	bool game::IsGrounded(int64_t entityId) {
		auto rbOpt = n_compoapi::GetRigidbodyComponent(entityId);
		if (!rbOpt) return false;
		return rbOpt->isGround;
	}

	void game::ApplyJumpImpulse(int64_t entityId, float strength) {
		auto rbOpt = n_compoapi::GetRigidbodyComponent(entityId);
		if (!rbOpt) {
			printf("ApplyJumpImpulse: no Rigidbody for %lld\n", (long long)entityId);
			return;
		}
		auto rb = *rbOpt;
		rb.gravity += strength; // 既存の落下速度と合成
		rb.isGround = false;
		n_compoapi::SetRigidbodyComponent(entityId, rb);
		printf("ApplyJumpImpulse_Impulse: entity=%lld strength=%f\n", (long long)entityId, strength);
	}


	// playerPosition は現在のプレイヤー座標
	void game::ProcessTriggers(const Vec2& playerPosition)
	{
		for (auto& kv : n_gamecomponent::instance_gameFunctions().finishComponents) {
			int64_t finishId = kv.first;
			n_component::FinishComponent& f = kv.second;
			if (!f.active) continue;

			// window_scene のオブジェクト座標を優先
			Vec2 finishPos;
			bool hasScenePos = n_windowscene::instance_winSce().GetSpritePosition(finishId, finishPos);

			if (!hasScenePos) {
				// エンティティに Transform/Translate コンポーネントがあればそれを使う
				bool usedTransform = false;

				// 例: TransformComponent を優先して取得
				if (n_compoapi::HasTransformComponent(finishId)) {
					auto optT = n_compoapi::GetTransformComponent(finishId);
					if (optT) {
						const auto& tc = *optT;
						finishPos = tc.position; // Transform のワールド位置を想定
						usedTransform = true;
					}
				}

				if (!usedTransform)
				{
					printf("[game]positionが見つからないから、AddTransformComponentするとか何とか\n");
				}

			}

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
					Vec2 entPos;
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
