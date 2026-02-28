#ifndef meshManager_hpp
#define meshManager_hpp
#include"mesh.hpp"
#include<UreTechEngine/VisualEngine/vertexArray.hpp>

class mesh;
class Material;
class vertexArrayObject;

struct loadedVaoData {
	UreTechEngine::string vaoName = "NULL";
	vertexArrayObject* vao;
};

class ENGINE_DEFINE MeshManager {
public:
	static MeshManager* getInstance();
	mesh* imp_mesh_obj_type(UreTechEngine::string filePath, Material _mat, bool useSmooth = true);
	mesh* imp_mesh_glb_type(UreTechEngine::string filePath, bool useSmooth = true);
private:
	static MeshManager* c_Instance;

	// asset management must be better here
	// its works for now :|
	std::mutex loadMutex;
	UreTechEngine::dArray<loadedVaoData> loadedVaos;

	MeshManager();
};


#endif 