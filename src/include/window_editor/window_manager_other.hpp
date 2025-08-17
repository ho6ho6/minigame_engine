/*サブウィンドウ用 window_i*/
/*Scene,Game,Inputが必ず実装する必要があるインタフェース*/

#ifndef WINDOW_MANGAER_OTHER
#define WINDOW_MANGAER_OTHER

#pragma once

namespace engine::window {

    class window_manager_other {
    public:
        virtual      ~window_manager_other() = default;
        virtual void window_manager_other_Render() = 0;          // 毎フレーム呼び出される描画関数
        virtual bool window_manager_other_IsVisible() const = 0;  // 表示／非表示管理
    };

} // namespace engine::window

#endif // !WINDOW_MANGAER_OTHER