#ifndef meshManager_hpp
#define meshManager_hpp
#include"mesh.hpp"
class mesh;
class Material;
class MeshManager {
public:
	static MeshManager* getInstance();
	mesh* imp_mesh_obj_type(UreTechEngine::string filePath, Material _mat, bool useSmooth = true);
private:
	static MeshManager* c_Instance;
	MeshManager();
};


#endif 