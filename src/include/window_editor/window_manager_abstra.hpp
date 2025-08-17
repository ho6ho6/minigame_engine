/*window_manager用*/

#ifndef WINDOW_MANAGER_ABSTRA
#define WINDOW_MANAGER_ABSTRA


#pragma once
#include "window_manager_other.hpp"

namespace engine::window
{
	class window_manager_abst
	{
		public:
			virtual		 ~window_manager_abst() = default;

			// 毎フレーム描画
			virtual void window_manager_RenderAll() = 0;	//登録済みのウィンドウ表示
	};
}

#endif