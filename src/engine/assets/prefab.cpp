/* Assets/prefabs�̃I�u�W�F�N�g�𐧌䂷�� */

//struct Prefab {
//	std::string name;	// Prefab�̖��O
//	std::string icon_path;	// Prefab�̃p�X
//	std::string mesh_path; // Prefab�̃��b�V���p�X
//	std::string material_path; // Prefab�̃}�e���A���p�X
//	Transform transform; // Prefab��Transform���
//};
//
//std::vector<Prefab> g_Prefab; // Prefab�̃��X�g
//
//void LoadAllPrefabs()
//{
//	for (auto& entry : std::filesystem::directory_iterator("Assets/prefab"))
//	{
//		if (entry.paht().extension() != ".json") continue;
//		Prefab prefab = ParsePrefabJson(entry.path().string());
//		g_Prefab.push_back(prefab);
//	}
//}