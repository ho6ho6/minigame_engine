/************************************************************
 *ゲーム全体の状態管理・シーン生成・ステートマシンなどを宣言			*
 ************************************************************/

#include "include/game.hpp"
#include "include/input.hpp"
#include "include/render.hpp"

namespace
{
	float rectX, rectY;
	float speedX;
}

namespace n_game
{
	bool Game_Start()
	{
		// ゲーム開始時の初期化
		rectX = 100.0f;
		rectY = 100.0f;
		speedX = 200.0f; // 1秒あたり200ピクセル移動
		return true;
	}

	void Game_Update(float deltaTime)
	{
		// 簡易入力: ←→ キーで四角を左右に動かす
		if (n_input::IsKeyDown(VK_LEFT))  rectX -= speedX * deltaTime;
		if (n_input::IsKeyDown(VK_RIGHT)) rectX += speedX * deltaTime;

		// 画面端で跳ね返す
		const float winW = 1920.0f - 50.0f; // 四角の幅 50px
		if (rectX < 0) { rectX = 0; speedX = -speedX; }
		if (rectX > winW) { rectX = winW; speedX = -speedX; }
	}

	// シーン上の各オブジェクトに対して「どう描画するか」を指示する
	void Game_Render()
	{

	}

	void Game_Shutdown()
	{
		// ゲーム終了時のクリーンアップ
		// 特に何もしないが、必要ならここでリソース解放などを行う
	}
}