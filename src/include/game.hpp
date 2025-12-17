#ifndef GAME_HPP
#define GAME_HPP

#include "include/input.hpp"
#include "include/render.hpp"
#include "include/game_component.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <limits>

namespace n_game
{
	class game
	{
		public:
		// メインループから呼び出されるコマンド処理
		void ProcessTriggers(const Vec3& playerPosition);

		private:
		// もしオンラインにも対応する時、PlayerIDで個別に演出を可能に出来る
		void HandleFinish(int64_t playerId, int64_t finishId, const n_component::FinishComponent& f);
	};
}

#endif // GAME_HPP