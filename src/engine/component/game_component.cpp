#include "include/input.h"
#include "include/render.h"
#include "include/component/game_component.h"
#include "include/component/component_manager.hpp"
#include "include/component/component_api.h"
#include "include/window_editor/window_scene.h"
#include "include/window_editor/window_hierarchy.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <limits>

namespace n_gamecomponent
{

	// コンストラクタでデフォルト値を設定
	gameFunctions::gameFunctions()
	{
		// Transform のデフォルト
		Transform.position = { 0.0f, 0.0f};
		Transform.rotation = { 0.0f, 0.0f};
		Transform.scale = { 1.0f, 1.0f};

		// Move のデフォルト
		Move.speed = 5.0f;
		Move.acceleration = 10.0f;
		Move.jump = 1.0f;
		Move.direction = { 1.0f, 0.0f};
		Move.jumpKey = ImGuiKey_Space;
		Move.directionUpKey = ImGuiKey_W;
		Move.directionRightKey = ImGuiKey_A;
		Move.directionDownKey = ImGuiKey_S;
		Move.directionLeftKey = ImGuiKey_D;

		// Light のデフォルト
		Light.color = { 1.0f, 1.0f, 1.0f };
		Light.intensity = 1.0f;
		Light.range = 10.0f;

		// Gravity のデフォルト
		Rigidbody.Vel[0] = 0.0f;
		Rigidbody.Vel[1] = 0.0f;
		Rigidbody.gravity[0] = 0.0f;
		Rigidbody.gravity[1] = -9.81f;
		Rigidbody.isGround = false;
		Rigidbody.isJump = false;
		Rigidbody.jumpElapsed = 0.0f;

		// Start のデフォルト
		Start.spawnRadius = 0.5f;
		Start.priority = 0;
		Start.active = false;

		// Finish のデフォルト
		Finish.radius = 1.0f;
		Finish.oneShot = true;
		Finish.active = false;

		// IsPlayer のデフォルト
		IsPlayer.IsEnemyActive = false;
		IsPlayer.IsPlayerActive = false;
		IsPlayer.EntityId = -2;
		IsPlayer.PlayerId = -1;

	};

	gameFunctions& instance_gameFunctions()
	{
		static gameFunctions instance;
		return instance;
	}

	// キュー登録
	void gameFunctions::EnqueueGameCommand(GameCommand cmd)
	{
		std::lock_guard<std::mutex> lk(queueMutex);
		commandQueue.push_back(std::move(cmd));
	}

	// mainloopで呼ぶ　キューをスワップしてロック時間の短縮
	void gameFunctions::ProcessGameCommands()
	{
		std::deque<GameCommand> local;
		{
			std::lock_guard<std::mutex> lk(queueMutex);
			local.swap(commandQueue);
		}
		for (auto& c : local) {
			try { c(); }
			catch (const std::exception& e) {
				std::cerr << "Game command exception: " << e.what() << "\n";
			}
		}
	}

	/*--------------------------コンポーネント--------------------------*/
	void gameFunctions::AddComponentTransform(int64_t id)
	{
		n_component::TransformComponent t;
		t.position = Transform.position;
		t.rotation = Transform.rotation;
		t.scale = Transform.scale;

		// バリデーション
		for (float s : t.scale) {
			if (s <= 0.0f) {
				std::cerr << "[AddComponentTransform] invalid default scale for object " << id << "\n";
				return;
			}
		}

		// キューに入れてメインスレッドで実行
		EnqueueGameCommand([this, id, t]() {
			CreateTransformComponent(id, t);
			// 追加後に Inspector を開く等の処理があればここで行う
			});
	};

	void gameFunctions::AddComponentMove(int64_t id)
	{
		n_component::MoveComponent m;
		m.direction = Move.direction;
		m.acceleration = Move.acceleration;

		m.jumpKey = Move.jumpKey;
		m.directionUpKey = Move.directionUpKey;
		m.directionRightKey = Move.directionRightKey;
		m.directionDownKey = Move.directionDownKey;
		m.directionLeftKey = Move.directionLeftKey;

		m.speed = Move.speed;

		EnqueueGameCommand([this, id, m]() {
			CreateMoveComponent(id, m);
			});
	};

	void gameFunctions::AddComponentLight(int64_t id)
	{
		n_component::LightComponent l;
		l.color = Light.color;
		l.intensity = Light.intensity;
		l.range = Light.range;

		EnqueueGameCommand([this, id, l]() {
			CreateLightComponent(id, l);
			});
	};

	void gameFunctions::AddComponentRigidbody(int64_t id)
	{
		n_component::RigidbodyComponent g;
		g.Vel[0] = Rigidbody.Vel[0];
		g.Vel[1] = Rigidbody.Vel[1];
		g.gravity = Rigidbody.gravity;
		g.isGround = Rigidbody.isGround;
		g.isJump = Rigidbody.isJump;
		g.jumpElapsed = Rigidbody.jumpElapsed;
		EnqueueGameCommand([this, id, g]() {
			CreateRigidbodyComponent(id, g);
			});
	};

	void gameFunctions::AddComponentStart(int64_t id)
	{
		n_component::StartComponent s;
		s.active = Start.active;
		s.priority = Start.priority;
		s.spawnRadius = Start.spawnRadius;
		EnqueueGameCommand([this, id, s]() {
			CreateStartComponent(id, s);
			// optional: notify editor to open inspector
			});
	};

	void gameFunctions::AddComponentFinish(int64_t id)
	{
		n_component::FinishComponent f;
		f.active = Finish.active;
		f.oneShot = Finish.oneShot;
		f.radius = Finish.radius;
		//f.tag = Finish.tag;
		EnqueueGameCommand([this, id, f]() {
			CreateFinishComponent(id, f);
			});
	};

	void gameFunctions::AddComponentDelete(int64_t id)
	{
		EnqueueGameCommand([this, id]() {
			// DeleteObjectAndComponents の簡易実装
			transformComponents.erase(id);
			moveComponents.erase(id);
			lightComponents.erase(id);
			rigidbodyComponents.erase(id);
			startComponents.erase(id);
			finishComponents.erase(id);
			isplayerComponents.erase(id);
			});
	};

	void gameFunctions::AddComponentIsPlayer(int64_t playerId)
	{
		n_component::IsPlayerComponent i;
		i.IsPlayerActive = IsPlayer.IsPlayerActive;
		i.IsEnemyActive = IsPlayer.IsEnemyActive;
		i.PlayerId = playerId; // -1: 未割当, >=0: プレイヤー番号, -2: 敵

		int64_t EntityId = -2;

		EnqueueGameCommand([this, EntityId, i]() {
			CreateIsPlayerComponent(EntityId, i);
			});
	};

	void gameFunctions::AddComponentSprite(int64_t id, const n_component::SpriteComponent& sp)
	{
		SceneSprite s;
		s.id = sp.spriteId;
		s.selected = sp.visible;

		// this をコピーキャプチャし、sp は値コピーしてラムダに渡す
		EnqueueGameCommand([this, id, sp,s]() {
			fprintf(stderr, "[EnqueuedLambda] start eid=%lld thread=%zu\n",
				(long long)id, (size_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
			fflush(stderr);
			this->CreateSpriteComponent(id, sp);

			if (auto tOpt = n_compoapi::GetTransformComponent(id))
			{
				Vec2 pos = { tOpt->position[0], tOpt->position[1] };
				n_compoapi::RegisterSpriteAndSyncTransform(id, s, pos);
			}

		});
		fprintf(stderr, "[QueueExec] running lambda thread=%zu\n", (size_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));
		fflush(stderr);
	};
	/*--------------------------コンポーネント--------------------------*/


	/*----------------------実際のコンポーネント生成----------------------*/


	// これらはデータを保持するだけ
	void gameFunctions::CreateTransformComponent(int64_t id, const n_component::TransformComponent& t)
	{
		// 存在チェックやマージ方針をここで決める
		transformComponents[id] = t;
		// 必要ならイベント発行や UI 更新フラグを立てる
	}

	void gameFunctions::CreateMoveComponent(int64_t id, const n_component::MoveComponent& m)
	{
		moveComponents[id] = m;
	}

	void gameFunctions::CreateLightComponent(int64_t id, const n_component::LightComponent& l)
	{
		lightComponents[id] = l;
	}

	void gameFunctions::CreateRigidbodyComponent(int64_t id, const n_component::RigidbodyComponent& g)
	{
		rigidbodyComponents[id] = g;
	}

	void gameFunctions::CreateStartComponent(int64_t id, const n_component::StartComponent& s)
	{
		startComponents[id] = s;
	}

	void gameFunctions::CreateFinishComponent(int64_t id, const n_component::FinishComponent& f)
	{
		finishComponents[id] = f;
	}

	void gameFunctions::CreateIsPlayerComponent(int64_t id, const n_component::IsPlayerComponent& i)
	{
		isplayerComponents[id] = i;
	}


	// 描画に関係するコンポーネントだけ登録処理を持たせる Lightもこのような処理になる可能性がある
	void gameFunctions::CreateSpriteComponent(int64_t id, const n_component::SpriteComponent& sc)
	{
		if (n_compomanager::g_componentManager.HasComponent<n_component::SpriteComponent>(id)) {
			printf("[game] CreateSpriteComponent skipped: already exists id=%lld\n", (long long)id);
			return;
		}
		n_compomanager::g_componentManager.SetComponent<n_component::SpriteComponent>(id, sc);
		SceneSprite s;
		s.id = sc.spriteId;
		s.pos_x = 0;
		s.pos_y = 0;
		s.selected = sc.visible;
		n_windowscene::instance_winSce().RegisterSprite(id, s);
		fprintf(stderr, "[CreateSpriteComponent] eid=%lld spriteId=%lld\n", (long long)id, (long long)sc.spriteId);
		fflush(stderr);
	}

	/*-----------------------------------------*/
}