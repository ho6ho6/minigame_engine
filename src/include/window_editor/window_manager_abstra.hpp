/*window_manager�p*/

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

			// ���t���[���`��
			virtual void window_manager_RenderAll() = 0;	//�o�^�ς݂̃E�B���h�E�\��
	};
}

#endif