#include "include/game.h"
#include "include/input.h"
#include "include/render.h"
#include "include/window_editor/window_scene.h"
#include "include/window_editor/window_hierarchy.h"
#include "include/component/component_api.h"
#include "include/component/component_manager.hpp"
#include "include/component/componentDefaults.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <limits>
#undef max


// 状態保持（エッジ検出用）
static std::unordered_map<int64_t, bool> s_wasJumpPressed;


namespace n_game
{

	game& instance_game()
	{
		static game inst;
		return inst;
	}

	void game::MarkTransformDirty(int64_t eid) {
		std::lock_guard<std::mutex> lk(g_dirtyMutex);
		g_dirtyTransforms.insert(eid);
	}

	// ↓不必要かもしれない
	void game::Render(int64_t entityId, std::optional<int> spriteIdOpt)
	{
		auto mcOpt = n_compoapi::GetMoveComponent(entityId);
		if (!mcOpt) return;
		auto mc = *mcOpt;

		if (mc.directionUpKey < 0) {
			s_wasJumpPressed.erase(entityId);
			return;
		}

		// spriteId があればエフェクトやデバッグ表示に使う
		if (spriteIdOpt) {
			int spriteId = *spriteIdOpt;
			// 例: ジャンプ時にエフェクトをスプライト位置に出す、またはデバッグ枠を描くためのフラグを立てる
			// n_render_registry::RequestEffectAtSprite(spriteId, EffectType::Jump); // 仮API
		}

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

	void game::ApplyJumpImpulse(int64_t entityId, float strength)
	{
		auto rbOpt = n_compoapi::GetRigidbodyComponent(entityId); 
		if (!rbOpt) return; 
		auto rb = *rbOpt; 

		// 地上にいる時のみジャンプを開始 
		if (!rb.isGround) return; 
		
		// strength は MoveComponent の設定値を優先して使う 
		if (n_compoapi::HasMoveComponent(entityId)) 
		{ 
			if (auto mcOpt = n_compoapi::GetMoveComponent(entityId)) 
			{ 
				if (mcOpt->jump > 0.0f) strength = mcOpt->jump; 
			} 
		} 
		
		// 初速を与える（座標系に合わせて符号を調整） 
		rb.Vel[1] = strength; 

		// ジャンプ状態をセット 
		rb.isJump = true; 
		rb.jumpElapsed = 0.0f; 
		rb.isGround = false;

		n_compoapi::SetRigidbodyComponent(entityId, rb);
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


	void game::UpdatePhysicsSystem(float dt)
	{
		auto all = n_compoapi::GetAllEntities();
		for (auto eid : all) {
			if (!n_compomanager::g_componentManager.HasComponent<n_component::RigidbodyComponent>(eid))
				continue;
			auto rbOpt = n_compoapi::GetRigidbodyComponent(eid);
			if (!rbOpt) continue;
			auto rb = *rbOpt;

			auto optT = n_compoapi::GetTransformComponent(eid);
			if (!optT) continue;
			auto t = *optT;

			// --- 1) 速度に加速度（重力）を加算 ---
			// rb.gravity は Vec2 の加速度 (ax, ay)
			rb.Vel[0] += rb.gravity[0] * dt; // 横加速度があるなら反映
			rb.Vel[1] += rb.gravity[1] * dt; // 縦加速度（重力）

			// --- 2) ジャンプ処理（ジャンプ開始時に初速を与える設計が望ましい） ---
			// もし isJump フラグでジャンプ中の特別処理をしたいなら、初速はジャンプ開始時に設定しておく
			// ここでは isJump フラグはジャンプ状態の追跡に使い、物理は速度で統一する
			if (rb.isJump) {
				// jumpElapsed を使って何か特殊なジャンプカーブを作るならここで使うが、
				// 基本は初速を与えて重力で落ちるのが自然
				rb.jumpElapsed += dt;
			}

			// --- 3) 速度を位置に反映（半暗示的オイラー） ---
			t.position[0] += rb.Vel[0] * dt;
			t.position[1] += rb.Vel[1] * dt;

			// --- 4) 地面判定（ワールド座標系の原点・符号に注意） ---
			if (t.position[1] <= 0.0f) {
				t.position[1] = 0.0f;
				rb.isGround = true;
				rb.isJump = false;
				rb.jumpElapsed = 0.0f;
				rb.Vel[1] = 0.0f;
			}
			else {
				rb.isGround = false;
			}

			// ここでのみ Transform / Rigidbody を書き戻す
			n_compoapi::SetTransformComponent(eid, t);
			n_compoapi::SetRigidbodyComponent(eid, rb);
		}
	}


	void game::UpdateMovementSystem(float dt)
	{
		auto all = n_compoapi::GetAllEntities();
		for (auto eid : all)
		{
			if (!n_compomanager::g_componentManager.HasComponent<n_component::MoveComponent>(eid)) continue;
			auto mvOpt = n_compoapi::GetMoveComponent(eid);
			if (!mvOpt) continue;
			auto mc = *mvOpt;

			auto tOpt = n_compoapi::GetTransformComponent(eid);
			if (!tOpt) continue;
			auto t = *tOpt;

			// Rigidbody があるか確認
			bool hasRb = n_compomanager::g_componentManager.HasComponent<n_component::RigidbodyComponent>(eid);
			n_component::RigidbodyComponent rb;
			if (hasRb) {
				if (auto rbOpt = n_compoapi::GetRigidbodyComponent(eid)) rb = *rbOpt;
			}

			bool isPlayer = n_compomanager::g_componentManager.HasComponent<n_component::IsPlayerComponent>(eid);


			Vec2 desiredVel{ 0.0f, 0.0f }; 
			float dirLen2 = mc.direction[0] * mc.direction[0] + mc.direction[1] * mc.direction[1]; 

			desiredVel[0] = mc.direction[0] * mc.speed;
			desiredVel[1] = mc.direction[1] * mc.speed;

			
			// 優先ルール: isPlayer が true のときはプレイヤー入力を優先して速度を設定する 
			if (isPlayer) 
			{ 
				if (hasRb) 
				{ 
					rb.Vel[0] = desiredVel[0];
					rb.Vel[1] = desiredVel[1];
				} 
				else
				{ 
					// Rigidbody が無ければ Transform を直接移動
					t.position[0] += desiredVel[0] * dt; 
					t.position[1] += desiredVel[1] * dt;
				} 
			}
			else
			{
				// 非プレイヤー（AI やスクリプト）: MoveComponent の意図があれば加速モード等で合成する 
				if (hasRb)
				{
					// 加速度が指定されていれば方向に沿って加速 
					if (mc.acceleration != 0.0f && dirLen2 > 1e-8f)
					{
						// mv.direction 正規化済み
						rb.Vel[0] += mc.direction[0] * mc.acceleration * dt;
						rb.Vel[1] += mc.direction[1] * mc.acceleration * dt;
					}
					// もし非プレイヤーでも「即時目標速度」を与えたいなら別条件で上書きする 
					// if (mv.forceSetVelocity) rb.Vel = desiredVel; 
				}
				else
				{
					// Rigidbody が無い非プレイヤーは Transform を直接移動（加速合成など）
					if (mc.acceleration != 0.0f && dirLen2 > 1e-8f)
					{
						// 簡易: 速度相当の移動量を加える（必要なら別の速度保持を使う）
						t.position[0] += mc.direction[0] * mc.acceleration * dt;
						t.position[1] += mc.direction[1] * mc.acceleration * dt;
					}
				}
			} 
			// ジャンプ処理（Rigidbody がある場合に初速を与える
			if (mc.jump > 0.0f && hasRb)
			{
				// 地上判定があるならチェック
				if (rb.isGround)
				{
					// 符号は座標系に合わせる（ここでは上方向が負なら -mc.jump）
					ApplyJumpImpulse(eid, mc.jump);
				}
				else
				{
					// 空中での二段ジャンプを許可するなら別処理 
				}
			}

			// MoveComponent の変更を保存
			n_compoapi::SetMoveComponent(eid, mc);
			// Rigidbody の変更を保存 
			n_compoapi::SetRigidbodyComponent(eid, rb);
		}
	}


	void game::ProcessRenderSync(float alpha)
	{
		std::vector<int64_t> toProcess;
		{
			std::lock_guard<std::mutex> lk(g_dirtyMutex);
			toProcess.assign(g_dirtyTransforms.begin(), g_dirtyTransforms.end());
			g_dirtyTransforms.clear();
		}

		//printf("[ProcessRenderSync] toProcess.size=%zu\n", toProcess.size());
		for (auto eid : toProcess) {
			auto optT = n_compoapi::GetTransformComponent(eid);
			if (!optT) continue;

			// 必要ならワールド→スクリーン変換をここで行う
			const std::array<float, 2> pos = { optT->position[0], optT->position[1] };
			//printf("[ProcessRenderSync] SetSpritePosition eid=%lld pos=(%f,%f)\n", (long long)eid, pos[0], pos[1]);
			bool ok = n_windowscene::instance_winSce().SetSpritePosition(eid, pos);
			if (!ok) {
				printf("[ProcessRenderSync] SetSpritePosition FAILED eid=%lld\n", (long long)eid);
			}
		}
	}

}
