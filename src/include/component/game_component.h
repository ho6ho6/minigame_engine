#ifndef GAME_COMPO
#define GAME_COMPO

#include "include/window_editor/window_scene_sprite.h"
#include "include/component/componentDefaults.h"

#include <array>
#include <unordered_map>
#include <deque>
#include <functional>
#include <mutex>
#include <cstdint>
#include <string>

#pragma once

using GameCommand = std::function<void()>;

namespace n_gamecomponent
{
	class gameFunctions
	{
	public:
		gameFunctions();

		// UI から呼び出されるコンポーネントAPI
		void AddComponentTransform(int64_t id);
		void AddComponentMove(int64_t id);
		void AddComponentLight(int64_t id);
		void AddComponentRigidbody(int64_t id);
		void AddComponentStart(int64_t id);
		void AddComponentFinish(int64_t id);
		void AddComponentDelete(int64_t id);
		void AddComponentIsPlayer(int64_t playerId);
		void AddComponentSprite(int64_t id, const n_component::SpriteComponent& sp);

		// ヘルパー
		void EnqueueGameCommand(GameCommand cmd);
		void ProcessGameCommands();

		// デフォルト値
		n_component::TransformDefaults Transform;
		n_component::MoveDefaults Move;
		n_component::LightDefaults Light;
		n_component::RigidbodyDefaults Rigidbody;
		n_component::StartDefaults Start;
		n_component::FinishDefaults Finish;
		n_component::IsPlayerDefaults IsPlayer;
		n_component::SpriteDefaults Sprite;

		// シンプルなコンポーネントストレージ（メインスレッド専用でロック不要にする方針）
		std::unordered_map<int64_t, n_component::TransformComponent> transformComponents;
		std::unordered_map<int64_t, n_component::MoveComponent> moveComponents;
		std::unordered_map<int64_t, n_component::LightComponent> lightComponents;
		std::unordered_map<int64_t, n_component::RigidbodyComponent> rigidbodyComponents;
		std::unordered_map<int64_t, n_component::StartComponent> startComponents;
		std::unordered_map<int64_t, n_component::FinishComponent> finishComponents;
		std::unordered_map<int64_t, n_component::IsPlayerComponent> isplayerComponents;
		std::unordered_map<int64_t, n_component::SpriteComponent> spriteComponents;

	private:

		// コマンドキュー
		using GameCommand = std::function<void()>;
		std::deque<GameCommand> commandQueue;
		std::mutex queueMutex;


		void CreateTransformComponent(int64_t id, const n_component::TransformComponent& t);
		void CreateMoveComponent(int64_t id, const n_component::MoveComponent& m);
		void CreateLightComponent(int64_t id, const n_component::LightComponent& l);
		void CreateRigidbodyComponent(int64_t id, const n_component::RigidbodyComponent& g);
		void CreateStartComponent(int64_t id, const n_component::StartComponent& s);
		void CreateFinishComponent(int64_t id, const n_component::FinishComponent& f);
		void CreateIsPlayerComponent(int64_t id, const n_component::IsPlayerComponent& i);
		void CreateSpriteComponent(int64_t id, const n_component::SpriteComponent& sp);

	};

	gameFunctions& instance_gameFunctions();
}
#endif