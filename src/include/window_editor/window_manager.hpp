/*各editorウィンドウの生成・削除を一元管理*/

#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#pragma once
#include <memory>
#include <vector>
#include "window_manager_abstra.hpp"
#include "window_manager_other.hpp"
#include <type_traits>

namespace engine::window
{

	class window_manager : public window_manager_abst
	{
		public:
			template<typename T>
			void window_manager_Register()
			{
				static_assert(std::is_base_of_v<window_manager_other, T>);
				windows_.emplace_back(std::make_unique<T>());
			}

			void window_manager_RenderAll() override;

		private:
			std::vector<std::unique_ptr<window_manager_other>> windows_; // “マネージャの抽象” ではなく “ウィンドウの抽象” を要素にする
	};
}

#endif // !WINDOW_MANAGER