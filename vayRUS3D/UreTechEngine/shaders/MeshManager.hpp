#ifndef meshManager_hpp
#define meshManager_hpp
#include"mesh.hpp"
class mesh;
class Material;
class MeshManager {
public:
	static MeshManager* getInstance();
	mesh* importMeshFbx(std::string filePath, Material _mat);
private:
	static MeshManager* c_Instance;
	MeshManager();
};


#endif 