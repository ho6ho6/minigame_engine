#ifndef GAME_HPP
#define GAME_HPP

#include "include/input.hpp"
#include "include/render.hpp"
#include "include/component/game_component.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <limits>

namespace n_game
{
	class game
	{
		public:
		void Render(int64_t entityId, std::optional<int> spriteIdOpt);
		// メインループから呼び出されるコマンド処理
		void ProcessTriggers(const Vec2& playerPosition);

		bool IsGrounded(int64_t entityId);
		void ApplyJumpImpulse(int64_t entityId, float strength);

		private:
		// もしオンラインにも対応する時、PlayerIDで個別に演出を可能に出来る
		void HandleFinish(int64_t playerId, int64_t finishId, const n_component::FinishComponent& f);
	};

	game& instance_game();
}

#endif // GAME_HPP