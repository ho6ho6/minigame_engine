/*Assets�̊Ǘ�*/

#ifndef ASSETS_MANAGER
#define ASSETS_MANAGER

#pragma once
#include "../../../include/assets/assets_manager/texture_manager.hpp"
#include "../../../include/assets/texture.hpp"


/*Assets�E�B���h�E�ɕ\�������摜�̃T���l�����\��*/
namespace n_assetsmanager
{
	class assets_manager
	{
	public:
		assets_manager(n_texturemanager::texture_manager& tex_Mgr) : m_TextureManager(tex_Mgr) {}

		void assets_Show();

	private:
		n_texturemanager::texture_manager& m_TextureManager;
	};

}
#endif // !ASSETS_MANAGER

