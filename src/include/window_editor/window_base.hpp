/* ���N���X���쐬���� */
// std::unique_ptr<n_windowscene::window_scene> �� std::unique_ptr<n_windowgame::window_game> �ɕϊ��ł��Ȃ�

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