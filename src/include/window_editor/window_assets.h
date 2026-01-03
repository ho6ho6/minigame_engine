/*Assetsフォルダにあるアセットを取得し表示、window_sceneに受け渡しも行う*/

#ifndef WINDOW_ASSETS_H
#define WINDOW_ASSETS_H

#include <imgui.h>
#include "window_base.h"
// パスを修正: assets_manager ディレクトリが include 配下にある場合

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