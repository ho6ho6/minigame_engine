#ifndef WINDOW_INSPECTOR_H
#define WINDOW_INSPECTOR_H

#include "window_base.h"
#include "window_manager.h"
#include "include/component/componentDefaults.h"
#include <optional>

namespace n_windowinspector
{
	class window_inspector : public n_windowbase::window_base
	{
		public:
			void SetWindowManager(n_windowmanager::window_manager* wm);
			void Render() override;

		private:
			n_windowmanager::window_manager* windowManager_ = nullptr;
	};
}

#endif // !WINDOW_HIERARCHY_H
