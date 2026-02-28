#include"MeshManager.hpp"

#include<vector>
#include<string>

#include<UreTechEngine/EngineCore.h>

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

mesh* MeshManager::imp_mesh_obj_type(UreTechEngine::string filePath, Material _mat, bool useSmooth)
{
	for (uint64_t i = 0; i < loadedVaos.size(); i++) {
		if (loadedVaos[i].vaoName == filePath) {
			UreTechEngine::EngineConsole::log("(Mesh Loader): Mesh already loaded! Path:" + filePath, UreTechEngine::EngineConsole::INFO);
			return new mesh(loadedVaos[i].vao, _mat);
		}
	}
	bool isUsedSmooth = false;
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices,finalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;
	std::vector< uint64_t > temp_face_smoothGroups;
	std::vector< int > tmp_face_texture_index;
	std::string file;

	if (UPK_ENABLE_PACKAGE_SYSTEM) {
		// read
		Buffer objImpBuf = UreTechEngineClass::getEngine()->package->get(filePath);

		//asign
		if (objImpBuf.pointer == nullptr) {
			UreTechEngine::EngineConsole::log("(Mesh Loader): Can not open source file! (UPK) Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
			return nullptr;
		}
		else {
			file = string((char*)objImpBuf.pointer, objImpBuf.size).std_str();
			free(objImpBuf.pointer);
		}
	}
	else {
		UreTechEngine::EngineConsole::log("(Mesh Loader): Can not open source file! (NOT UPK) Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
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
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid vertex data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::LOG_ERROR);
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
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid vertex normal data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::LOG_ERROR);
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
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid vertex uv data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::LOG_ERROR);
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
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid smooth group data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::LOG_ERROR);
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
							UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid f1 data! " + filePath + "-i>" + UreTechEngine::u64ToDecStr(i) + "-j>" + UreTechEngine::u64ToDecStr(j), UreTechEngine::EngineConsole::LOG_ERROR);
						}
					}
				}
				else {
					UreTechEngine::EngineConsole::log("(Mesh Loader): Invalid f0 data! " + filePath + "-->" + UreTechEngine::u64ToDecStr(i), UreTechEngine::EngineConsole::LOG_ERROR);
				}
			}
			else if (lines[i].substr(6) == "usemtl") {
				textureCount++;
			}
			else if (lines[i].substr(2) == "g ") {
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
	loadedVaos.push_back({ filePath,_vao });

	mesh* result = new mesh(loadedVaos[loadedVaos.size() - 1].vao, _mat);

	UreTechEngine::EngineConsole::log("(Mesh Loader): Obj loaded! Path:" + filePath, UreTechEngine::EngineConsole::INFO);
	return result;
}

struct GLB_HEADER {
	char magic[4];
	uint32_t version;
	uint32_t totalLenght;

	void* first_chunk_header() {
		return ((char*)this) + sizeof(GLB_HEADER);
	}
};

struct GLB_CHUNK_HEADER {
	uint32_t chunkLenght;
	char chunkType[4];

	void* data_begin() {
		return ((char*)this) + sizeof(GLB_CHUNK_HEADER);
	}

	void* data_offset(uint32_t _offset) {
		return ((char*)this) + sizeof(GLB_CHUNK_HEADER) + _offset;
	}

	GLB_CHUNK_HEADER* get_next() {
		return (GLB_CHUNK_HEADER*)(((char*)this) + sizeof(GLB_CHUNK_HEADER) + chunkLenght);

	}
};

// helper macro
#define GLB_HSTRING_TO_USTRING(buf) string(buf, 4)

#define GLB_JSON_TYPE_NAME "JSON"
#define GLB_BIN_TYPE_NAME "BIN\0"

#define GLB_COMPONENT_TYPE_BYTE_ID 5120
#define GLB_COMPONENT_TYPE_UBYTE_ID 5121
#define GLB_COMPONENT_TYPE_SHORT_ID 5122
#define GLB_COMPONENT_TYPE_USHORT_ID 5123
#define GLB_COMPONENT_TYPE_UINT_ID 5125
#define GLB_COMPONENT_TYPE_FLOAT32_ID 5126

#define GLB_TYPE_SCALAR "SCALAR"
#define GLB_TYPE_VEC2 "VEC2"
#define GLB_TYPE_VEC3 "VEC3"

mesh* MeshManager::imp_mesh_glb_type(UreTechEngine::string filePath, bool useSmooth)
{


	Buffer glBinary;
	glBinary.pointer = nullptr;
	glBinary.size = 0;

	if (UPK_ENABLE_PACKAGE_SYSTEM) {
		// read
		glBinary = UreTechEngineClass::getEngine()->package->get(filePath);

		//asign
		if (glBinary.pointer == nullptr) {
			UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Can not open source file! (UPK) Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
			return nullptr;
		}
	}
	else {
		UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Can not open source file! (NOT UPK) Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
		return nullptr;
	}

	GLB_HEADER* glb_header = (GLB_HEADER*)glBinary.pointer;

	if (GLB_HSTRING_TO_USTRING(glb_header->magic) != "glTF") {
		UreTechEngine::EngineConsole::log("(GLB Mesh Loader): File is corrupted or invalid! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
		free(glBinary.pointer);
		return nullptr;
	}

	if (glb_header->version != 2) {
		UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only glTF 2.0 allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
		free(glBinary.pointer);
		return nullptr;
	}

	if (glb_header->totalLenght != glBinary.size) {
		UreTechEngine::EngineConsole::log("(GLB Mesh Loader): File size is not matching with real size! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
		free(glBinary.pointer);
		return nullptr;
	}

	dArray<GLB_CHUNK_HEADER*> chunks;

	GLB_CHUNK_HEADER* jsonChunk = nullptr;

	GLB_CHUNK_HEADER* last = (GLB_CHUNK_HEADER*)glb_header->first_chunk_header();

	while (true) {

		UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Found chunk with type: " + GLB_HSTRING_TO_USTRING(last->chunkType), UreTechEngine::EngineConsole::DEBUG);

		if (GLB_HSTRING_TO_USTRING(last->chunkType) == GLB_JSON_TYPE_NAME) {
			jsonChunk = last;
		}
		else {
			// ignore json chunk
			chunks.push_back(last);
		}

		last = last->get_next();

		if ((char*)last >= (char*)(glBinary.pointer + glBinary.size)) {
			break; // no more chunks available
		}
	}

	if (jsonChunk == nullptr) {
		UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Json chunk not found! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
		free(glBinary.pointer);
		return nullptr;
	}

	nlohmann::json json = nlohmann::json::parse((char*)(jsonChunk->data_begin()), ((char*)(jsonChunk->data_begin()) + jsonChunk->chunkLenght));

	if (json.contains("meshes")) {
		if (json["meshes"].size() > 0) {
			if (json["meshes"][0].contains("primitives")) {

				// primitives (submesh)
				// we need to hold pointer because actual vao is must not have a copy
				// copies could cause problems because of the pointer nature of vaos
				dArray<vertexArrayObject*> vaos;

				// load primitives
				int primitivesCount = json["meshes"][0]["primitives"].size();
				for (int i = 0; i < primitivesCount; i++) {
					if (json["meshes"][0]["primitives"][i].contains("attributes")) {
						int POSITION_accessor_index = json["meshes"][0]["primitives"][i]["attributes"]["POSITION"];
						int NORMAL_accessor_index = json["meshes"][0]["primitives"][i]["attributes"]["NORMAL"];
						int TEXCOORD_0_accessor_index = json["meshes"][0]["primitives"][i]["attributes"]["TEXCOORD_0"];
						int indices_accessor_index = json["meshes"][0]["primitives"][i]["indices"];

						// Check Position accessor
						if (json["accessors"][POSITION_accessor_index]["componentType"] != GLB_COMPONENT_TYPE_FLOAT32_ID) {
							UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only float32 position allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
							free(glBinary.pointer);
							return nullptr;
						}
						if (json["accessors"][POSITION_accessor_index]["type"] != GLB_TYPE_VEC3) {
							UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only vec3 position allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
							free(glBinary.pointer);
							return nullptr;
						}

						// Check Normal accessor
						if (json["accessors"][NORMAL_accessor_index]["componentType"] != GLB_COMPONENT_TYPE_FLOAT32_ID) {
							UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only float32 normal allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
							free(glBinary.pointer);
							return nullptr;
						}
						if (json["accessors"][NORMAL_accessor_index]["type"] != GLB_TYPE_VEC3) {
							UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only vec3 normal allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
							free(glBinary.pointer);
							return nullptr;
						}

						// Check UV accessor
						if (json["accessors"][TEXCOORD_0_accessor_index]["componentType"] != GLB_COMPONENT_TYPE_FLOAT32_ID) {
							UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only float32 UV allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
							free(glBinary.pointer);
							return nullptr;
						}
						if (json["accessors"][TEXCOORD_0_accessor_index]["type"] != GLB_TYPE_VEC2) {
							UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only vec2 UV allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
							free(glBinary.pointer);
							return nullptr;
						}

						// Check Index accessor
						int indexComponentType = json["accessors"][indices_accessor_index]["componentType"];
						if (indexComponentType != GLB_COMPONENT_TYPE_USHORT_ID && indexComponentType != GLB_COMPONENT_TYPE_UINT_ID) {
							UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only USHORT or UINT indices allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
							free(glBinary.pointer);
							return nullptr;
						}
						if (json["accessors"][indices_accessor_index]["type"] != GLB_TYPE_SCALAR) {
							UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Only scalar indices allowed! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
							free(glBinary.pointer);
							return nullptr;
						}

						// If we are here, all data is valid, we can load it
						vertexArrayObject* temp_vao = new vertexArrayObject();

						// meta datas
						uint32_t vertexCount = json["accessors"][POSITION_accessor_index]["count"];
						uint32_t indexCount = json["accessors"][indices_accessor_index]["count"];

						bool isIndexUINT = (indexComponentType == GLB_COMPONENT_TYPE_UINT_ID);

						uint32_t positionBufferViewIndex = json["accessors"][POSITION_accessor_index]["bufferView"];
						uint32_t positionBufferViewOffset = json["bufferViews"][positionBufferViewIndex]["byteOffset"];
						uint32_t positionBuffer = json["bufferViews"][positionBufferViewIndex]["buffer"];
						float* positionData = (float*)chunks[positionBuffer]->data_offset(positionBufferViewOffset);

						uint32_t normalBufferViewIndex = json["accessors"][NORMAL_accessor_index]["bufferView"];
						uint32_t normalBufferViewOffset = json["bufferViews"][normalBufferViewIndex]["byteOffset"];
						uint32_t normalBuffer = json["bufferViews"][normalBufferViewIndex]["buffer"];
						float* normalData = (float*)chunks[normalBuffer]->data_offset(normalBufferViewOffset);

						uint32_t uvBufferViewIndex = json["accessors"][TEXCOORD_0_accessor_index]["bufferView"];
						uint32_t uvBufferViewOffset = json["bufferViews"][uvBufferViewIndex]["byteOffset"];
						uint32_t uvBuffer = json["bufferViews"][uvBufferViewIndex]["buffer"];
						float* uvData = (float*)chunks[uvBuffer]->data_offset(uvBufferViewOffset);

						// temporal buffer
						dArray<Vertex> vertices;

						for (uint32_t j = 0; j < vertexCount; j++) {
							Vertex newVertex;
							newVertex.pos = glm::vec3(positionData[j * 3], positionData[j * 3 + 1], positionData[j * 3 + 2]);
							newVertex.norm = glm::vec3(normalData[j * 3], normalData[j * 3 + 1], normalData[j * 3 + 2]);
							newVertex.tex = glm::vec2(uvData[j * 2], uvData[j * 2 + 1]);
							newVertex.textureIndex = i;
							vertices.push_back(newVertex);
						}

						uint32_t indexBufferViewIndex = json["accessors"][indices_accessor_index]["bufferView"];
						uint32_t indexBufferViewOffset = json["bufferViews"][indexBufferViewIndex]["byteOffset"];
						uint32_t indexBuffer = json["bufferViews"][indexBufferViewIndex]["buffer"];

						// index data is same we just  need to check if its uint or ushort
						if (isIndexUINT) {
							indexData_32* indexData = (indexData_32*)chunks[indexBuffer]->data_offset(indexBufferViewOffset);

							temp_vao->createObject(*vertices.data(), vertexCount, *indexData, indexCount);
						}
						else {
							indexData_16* indexData = (indexData_16*)chunks[indexBuffer]->data_offset(indexBufferViewOffset);

							temp_vao->createObject(*vertices.data(), vertexCount, *indexData, indexCount);
						}
						loadMutex.lock();
						this->loadedVaos.push_back({ filePath + "@vao" + u64ToDecStr(i), temp_vao});
						loadMutex.unlock();
						vaos.push_back((this->loadedVaos[this->loadedVaos.size() - 1].vao));
					}
				}

				// all primitives loaded, we can create mesh now
				mesh* result = new mesh(vaos.data(), vaos.size());
				UreTechEngine::EngineConsole::log("(GLB Mesh Loader): GLB loaded! Path:" + filePath, UreTechEngine::EngineConsole::INFO);
				free(glBinary.pointer); // free binary data after we loaded everything, we dont need it anymore
				return result;
			}
			else {
				UreTechEngine::EngineConsole::log("(GLB Mesh Loader): Mesh dont have primitives! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
				free(glBinary.pointer);
				return nullptr;
			}
		}
		else {
			UreTechEngine::EngineConsole::log("(GLB Mesh Loader): No any mesh found in meshes! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
			free(glBinary.pointer);
			return nullptr;
		}
	}
	else {
		UreTechEngine::EngineConsole::log("(GLB Mesh Loader): No any mesh found! Path:" + filePath, UreTechEngine::EngineConsole::LOG_ERROR);
		free(glBinary.pointer);
		return nullptr;
	}
}
