#include "include/input.hpp"
#include "include/render.hpp"
#include "include/component/game_component.hpp"
#include "include/window_editor/window_scene.hpp"
#include "include/window_editor/window_editor.hpp"
#include "include/window_editor/window_hierarchy.hpp"

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

		// Light のデフォルト
		Light.color = { 1.0f, 1.0f, 1.0f };
		Light.intensity = 1.0f;
		Light.range = 10.0f;

		// Gravity のデフォルト
		Rigidbody.gravity = -9.81f;
		Rigidbody.isGround = false;

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
		m.jump = Move.jump;
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
		g.gravity = Rigidbody.gravity;
		g.isGround = Rigidbody.isGround;
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

	void gameFunctions::AddComponentSprite(int64_t id)
	{
		n_component::SpriteComponent sp;
		sp.spriteId = Sprite.spriteId;
		sp.visible = Sprite.visible;
		EnqueueGameCommand([this, id, sp]() {
			CreateSpriteComponent(id, sp);
			});
	};
	/*--------------------------コンポーネント--------------------------*/


	/*----------------------実際のコンポーネント生成----------------------*/

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

	void gameFunctions::CreateSpriteComponent(int64_t id, const n_component::SpriteComponent& sp)
	{
		spriteComponents[id] = sp;
	}

	/*-----------------------------------------*/
}