СИСТЕМА РЕСУРСОВ (ResourceService.cpp)
Методы:
	1) void LoadBaseAssets() - при запуске движка загружает в память базовые ресурсы
	каждого типа (Текстура, меш, кубмапа и т.д.)
	2) std::shared_ptr<ResourceBase> LoadResource(uuid) - подгружает необходимый ресурс по 
	заданному uuid. Возвращает ссылку на ResourseBase, которую необходимо кастануть к необходимому
	типу. Например, если загружается меш:
		mesh.model=std::static_pointer_cast<Model>( rs->LoadResource(mesh.modelUUID));
	3) std::string FindFilePathByUUID(uuid,bool getFileName) - находит полный путь к ресурсу с
	заданным uuid (getFileName = false) или название файла (getFileName=true)
	4) uuid GetUUIDFromPath (path) - возвращает uuid, ассоциируемый с файлом по данному пути
	5) vector<uuid> GetAllResourcesOfType (EAssetType assetType) - возвращает вектор uuid всех
	ресурсов, тип которых = assetType
	6) vector<string> GetAllResourcesFilePaths (EAssetType assetType, bool findFullPath) - аналогично предыдущей функции,
	только теперь возвращает вектор путей к файлам.
	7) void LoadSceneFromFile(...)/ void SaveSceneToFile(...) - загружает/сохраняет сцену из/в файл
	8) void GenerateMetaFiles(filePath) - генерирует метафайлы (если необходимо) для всех
	файлов в данной директории и дочерних директориях
