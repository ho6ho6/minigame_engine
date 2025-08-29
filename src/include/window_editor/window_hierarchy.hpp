#ifndef WINDOW_HIERARCHY
#define WINDOW_HIERARCHY

#pragma once
#include <imgui.h>
#include "window_base.hpp"

namespace n_windowhierarchy
{
	class window_hierarchy : public n_windowbase::window_base
	{
		public:
			void Render() override;
		private:
			ImVec2 m_LastSize = { 0,0 };
	};
}

#endif // !WINDOW_HIERARCHY
