#ifndef GAME_H
#define GAME_H

#include "include/input.h"
#include "include/render.h"
#include "time.h"
#include "include/component/game_component.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <limits>
#include <vector>
#include <cstdint>

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

			void UpdatePhysicsSystem(float dt);
			void UpdateMovementSystem(float dt);

			// Transform 更新側が呼ぶ（SetTransformComponent 内など）
			void MarkTransformDirty(int64_t entityId);

			// 描画前に呼ぶ（main loop の描画直前）
			// alpha は補間に使うなら渡す（補間しないなら無視可）
			void ProcessRenderSync(float alpha);

		private:
			// もしオンラインにも対応する時、PlayerIDで個別に演出を可能に出来る
			void HandleFinish(int64_t playerId, int64_t finishId, const n_component::FinishComponent& f);
	};

	game& instance_game();
}

#endif // GAME_HPP