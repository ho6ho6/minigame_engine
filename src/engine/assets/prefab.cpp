/* Assets/prefabsのオブジェクトを制御する */

//struct Prefab {
//	std::string name;	// Prefabの名前
//	std::string icon_path;	// Prefabのパス
//	std::string mesh_path; // Prefabのメッシュパス
//	std::string material_path; // Prefabのマテリアルパス
//	Transform transform; // PrefabのTransform情報
//};
//
//std::vector<Prefab> g_Prefab; // Prefabのリスト
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