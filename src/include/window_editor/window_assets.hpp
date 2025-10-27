#ifndef WINDOW_ASSETS
#define WINDOW_ASSETS

#pragma once
#include <imgui.h>
#include "window_base.hpp"
// �p�X���C��: assets_manager �f�B���N�g���� include �z���ɂ���ꍇ
#include "include/assets/assets_manager/texture_manager.hpp"

namespace n_windowassets
{
	class window_assets : public n_windowbase::window_base
	{
		public:
			void Render() override;
		private:
			ImVec2 m_LastSize = { 0,0 };
	};
}

#endif // !WINDOW_ASSETS
