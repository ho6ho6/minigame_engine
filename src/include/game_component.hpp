#ifndef GAME_COMPO
#define GAME_COMPO

#include "include/componentDefaults.hpp"
#include "include/window_editor/window_scene_sprite.hpp"
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
		void AddComponentGravity(int64_t id);
		void AddComponentStart(int64_t id);
		void AddComponentFinish(int64_t id);
		void AddComponentDelete(int64_t id);
		void AddComponentIsPlayer(int64_t playerId);

		// ヘルパー
		void EnqueueGameCommand(GameCommand cmd);
		void ProcessGameCommands();

		// デフォルト値
		n_component::TransformDefaults Transform;
		n_component::MoveDefaults Move;
		n_component::LightDefaults Light;
		n_component::GravityDefaults Gravity;
		n_component::StartDefaults Start;
		n_component::FinishDefaults Finish;
		n_component::IsPlayerDefaults IsPlayer;

		// シンプルなコンポーネントストレージ（メインスレッド専用でロック不要にする方針）
		std::unordered_map<int64_t, n_component::TransformComponent> transformComponents;
		std::unordered_map<int64_t, n_component::MoveComponent> moveComponents;
		std::unordered_map<int64_t, n_component::LightComponent> lightComponents;
		std::unordered_map<int64_t, n_component::GravityComponent> gravityComponents;
		std::unordered_map<int64_t, n_component::StartComponent> startComponents;
		std::unordered_map<int64_t, n_component::FinishComponent> finishComponents;
		std::unordered_map<int64_t, n_component::IsPlayerComponent> isplayerComponents;


	private:

		// コマンドキュー
		using GameCommand = std::function<void()>;
		std::deque<GameCommand> commandQueue;
		std::mutex queueMutex;


		void CreateTransformComponent(int64_t id, const n_component::TransformComponent& t);
		void CreateMoveComponent(int64_t id, const n_component::MoveComponent& m);
		void CreateLightComponent(int64_t id, const n_component::LightComponent& l);
		void CreateGravityComponent(int64_t id, const n_component::GravityComponent& g);
		void CreateStartComponent(int64_t id, const n_component::StartComponent& s);
		void CreateFinishComponent(int64_t id, const n_component::FinishComponent& s);
		void CreateIsPlayerComponent(int64_t id, const n_component::IsPlayerComponent& i);

	};

	gameFunctions& instance_gameFunctions();
}
#endif // !GAME_COMPO
