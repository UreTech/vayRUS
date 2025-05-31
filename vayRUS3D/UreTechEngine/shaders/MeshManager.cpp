#include"MeshManager.hpp"

#include<vector>
#include<string>

#include<../EngineCore.h>

MeshManager* MeshManager::c_Instance = nullptr;
	
MeshManager::MeshManager() {

}
MeshManager* MeshManager::getInstance()
{
	if (c_Instance != nullptr) {
		return c_Instance;
	}
	else {
		c_Instance = new MeshManager();
		return c_Instance;
	}
}

glm::vec3 computeFaceNormal(const triangleFace& face) {
	glm::vec3 edge1 = face.v1.pos - face.v0.pos;
	glm::vec3 edge2 = face.v2.pos - face.v0.pos;
	return glm::normalize(glm::cross(edge1, edge2));
}

void calculateSmoothNormals(std::vector<triangleFace>& faces) {
	struct SmoothVertex {
		glm::vec3 position;
		glm::vec3 normalSum = glm::vec3(0.0f);
		uint64_t smoothGroup;
		int count = 0;
	};

	std::vector<SmoothVertex> smoothVertices;

	for (auto& face : faces) {
		glm::vec3 faceNormal = computeFaceNormal(face);

		for (Vertex* v : { &face.v0, &face.v1, &face.v2 }) {
			bool found = false;

			for (auto& sv : smoothVertices) {
				if (sv.smoothGroup == face.smoothGroup && sv.position == v->pos) {
					sv.normalSum += faceNormal;
					sv.count++;
					found = true;
					break;
				}
			}

			if (!found) {
				smoothVertices.push_back({ v->pos, faceNormal, face.smoothGroup, 1 });
			}
		}
	}

	for (auto& sv : smoothVertices) {
		sv.normalSum = glm::normalize(sv.normalSum);
	}

	for (auto& face : faces) {
		for (Vertex* v : { &face.v0, &face.v1, &face.v2 }) {
			for (auto& sv : smoothVertices) {
				if (sv.smoothGroup == face.smoothGroup && sv.position == v->pos) {
					v->norm = sv.normalSum;
					break;
				}
			}
		}
	}
}

mesh* MeshManager::imp_mesh_obj_type(UreTechEngine::string filePath,Material _mat, bool useSmooth)
{
	bool isUsedSmooth = false;
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices,finalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
	std::vector< uint64_t > temp_face_smoothGroups;
	std::vector< int > tmp_face_texture_index;
	std::string file;

	if (UPK_ENABLE_PACKAGE_SYSTEM) {
		Buffer objImpBuf = UreTechEngineClass::getEngine()->package->get(filePath);
		file = string((char*)objImpBuf.pointer, objImpBuf.size).std_str();
		if (objImpBuf.pointer == nullptr) {
			UreTechEngine::EngineConsole::log("(Mesh Loader): Can not open source file! (UPK) Path:" + filePath, UreTechEngine::EngineConsole::ERROR_NORMAL);
			return nullptr;
		}
	}
	else {
		UreTechEngine::EngineConsole::log("(Mesh Loader): Can not open source file! (NOT UPK) Path:" + filePath, UreTechEngine::EngineConsole::ERROR_NORMAL);
		return nullptr;
	}

	std::vector<triangleFace> faces;
	int textureCount = -1;
	uint64_t smoothGroup = 0;

	dArray <string> lines = parseWith(string::stdStrToUStr(file), '\n');

	for (uint64_t i = 0; i < lines.size(); i++) {
		if (!lines[i].empty()) {
			if (lines[i].substr(2) == "v ") {// vertex (3 input x, y, z)
				dArray <string> vReadStr = parseWith(lines[i], ' ');
				if (vReadStr.size() == 4) {
					float x = std::atof(vReadStr[1].c_str());
					float y = std::atof(vReadStr[2].c_str());
					float z = std::atof(vReadStr[3].c_str());
					temp_vertices.push_back(glm::vec3(x, y, z));
				}
				else {
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid vertex data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::ERROR_NORMAL);
				}
			}
			else if (lines[i].substr(3) == "vn ") {// vertex normal
				dArray <string> vnReadStr = parseWith(lines[i], ' ');
				if (vnReadStr.size() == 4) {
					float x = std::atof(vnReadStr[1].c_str());
					float y = std::atof(vnReadStr[2].c_str());
					float z = std::atof(vnReadStr[3].c_str());
					temp_normals.push_back(glm::vec3(x, y, z));
				}
				else {
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid vertex normal data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::ERROR_NORMAL);
				}
			}
			else if (lines[i].substr(3) == "vt ") {// texture cord
				dArray <string> vtReadStr = parseWith(lines[i], ' ');
				if (vtReadStr.size() == 3) {
					float x = std::atof(vtReadStr[1].c_str());
					float y = std::atof(vtReadStr[2].c_str());
					temp_uvs.push_back(glm::vec2(x, y));
				}
				else {
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid vertex uv data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::ERROR_NORMAL);
				}
			}
			else if (lines[i].substr(2) == "s ") {// smooth group
				try {
					smoothGroup = strToU64(lines[i].substr(INT64_MAX, 2));
					if (smoothGroup != 0) {
						isUsedSmooth = true;
					}
				}
				catch (std::exception) {
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid smooth group data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::ERROR_NORMAL);
				}
			}
			else if (lines[i].substr(2) == "f ") {// face (must be 3 at each line)
				if (textureCount == -1) {
					textureCount = 0;
				}
				dArray <string> f0ReadStr = parseWith(lines[i], ' ');
				if (f0ReadStr.size() == 4) {
					// read verts
					for (uint64_t j = 1; j < 4; j++) {
						dArray <string> f1ReadStr = parseWith(f0ReadStr[j], '/');
						if (f1ReadStr.size() == 3) {
							vertexIndices.push_back(strToU64(f1ReadStr[0])-1);
							uvIndices.push_back(strToU64(f1ReadStr[1])-1);
							normalIndices.push_back(strToU64(f1ReadStr[2])-1);
							temp_face_smoothGroups.push_back(smoothGroup);
							tmp_face_texture_index.push_back(textureCount);
						}
						else {
							UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid f1 data! " + filePath + "-i>" + UreTechEngine::u64ToDecStr(i) + "-j>" + UreTechEngine::u64ToDecStr(j), UreTechEngine::EngineConsole::ERROR_NORMAL);
						}
					}
				}
				else {
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid f0 data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::ERROR_NORMAL);
				}
			}
			else if (lines[i].substr(6) == "usemtl") {
				textureCount++;
			}
		}
	}
	
	for (unsigned int a = 0; a < vertexIndices.size(); a+=3) {
		triangleFace face;
		face.v0 = Vertex(temp_vertices[vertexIndices[(a)]], temp_uvs[uvIndices[(a)]], temp_normals[normalIndices[(a)]], tmp_face_texture_index[(a)]);
		face.v1 = Vertex(temp_vertices[vertexIndices[(a)+1]], temp_uvs[uvIndices[(a) + 1]], temp_normals[normalIndices[(a) + 1]], tmp_face_texture_index[(a+1)]);
		face.v2 = Vertex(temp_vertices[vertexIndices[(a)+2]], temp_uvs[uvIndices[(a) + 2]], temp_normals[normalIndices[(a) + 2]], tmp_face_texture_index[(a+2)]);
		face.smoothGroup = temp_face_smoothGroups[a];// div by 3 because 3 vertices
		faces.push_back(face);
		finalIndices.push_back((a));
		finalIndices.push_back((a) + 1);
		finalIndices.push_back((a) + 2);
	}
	if (useSmooth && isUsedSmooth) {
		UreTechEngine::EngineConsole::log("(Mesh Loader): Using smooth! Path:" + filePath, UreTechEngine::EngineConsole::DEBUG);
		std::vector<std::vector<triangleFace>> smoothTempFaces;
		uint64_t csf = 0;
		smoothTempFaces.resize(1, std::vector<triangleFace>());
		for (uint64_t i = 0; i < faces.size(); i++) {
			if (csf != faces[i].smoothGroup) {
				csf = faces[i].smoothGroup;
				smoothTempFaces.push_back(std::vector<triangleFace>());
				smoothTempFaces[csf - 1].push_back(faces[i]);
			}
			else {
				smoothTempFaces[csf].push_back(faces[i]);
			}
		}
		for (uint64_t i = 0; i < smoothTempFaces.size(); i++) {
			calculateSmoothNormals(smoothTempFaces[i]);
		}
		faces.clear();
		for (uint64_t i = 0; i < smoothTempFaces.size(); i++) {
			for (uint64_t j = 0; j < smoothTempFaces[i].size(); j++) {
				faces.push_back(smoothTempFaces[i][j]);
			}
		}
	}
	std::vector<Vertex>tmpVert;

	vertexArrayObject* _vao = new vertexArrayObject();
		for (unsigned int i = 0; i < faces.size(); i++) {
			tmpVert.push_back(faces[i].v0);
			tmpVert.push_back(faces[i].v1);
			tmpVert.push_back(faces[i].v2);
			//UreTechEngine::EngineERROR::consoleError(std::to_string(faces[i].v0.textureIndex), UreTechEngine::EngineERROR::INFO_NORMAL);
			//UreTechEngine::EngineERROR::consoleError(std::to_string(faces[i].v1.textureIndex), UreTechEngine::EngineERROR::INFO_NORMAL);
			//UreTechEngine::EngineERROR::consoleError(std::to_string(faces[i].v2.textureIndex), UreTechEngine::EngineERROR::INFO_NORMAL);
		}

	unsigned int vertCount = temp_vertices.size();
	_vao->createObject(tmpVert[0], tmpVert.size(), finalIndices[0], finalIndices.size());
	mesh* result = new mesh(_vao, _mat);
	UreTechEngine::EngineConsole::log("(Mesh Loader): Obj loaded! Path:" + filePath, UreTechEngine::EngineConsole::INFO_NORMAL);
	return result;
}
