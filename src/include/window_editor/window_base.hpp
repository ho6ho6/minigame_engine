/* Šî’êƒNƒ‰ƒX‚ğì¬‚·‚é */
// std::unique_ptr<n_windowscene::window_scene> ‚ğ std::unique_ptr<n_windowgame::window_game> ‚É•ÏŠ·‚Å‚«‚È‚¢

#pragma once

namespace n_windowbase
{
	class window_base
	{
		public:
			virtual ~window_base() = default;
			virtual void Render() = 0;
	};
}