#include<../EngineCore.h>

#define entity_cpp

void UreTechEngine::entity::updateVisual()
{
	if (entityMesh != nullptr) {
		//UreTechEngine::EngineConsole::log("Ent addr: " + intToHex((uint64_t)entityMesh), UreTechEngine::EngineConsole::DEBUG);
		entityMesh->draw(transform);
	}
}

void UreTechEngine::entity::init(UreTechEngineClass* _ptr)
{
	engineRef = _ptr;
}

void UreTechEngine::entity::print(string message)
{
	EngineConsole::log(this->entClassName + ":" + this->entName + "(" +intToHex(this->entityID) + ")-->" + message, EngineConsole::DEBUG);
}

void UreTechEngine::entity::begin()
{
}

void UreTechEngine::entity::tick()
{
}

UreTechEngine::entity::entity()
{
	entClassName = "entity";
	entityMesh = nullptr;
}

UreTechEngine::entity::entity(UreTechEngine::vector3& location,UreTechEngine::Rotation& rot,UreTechEngine::vector3& scale, mesh* _mesh, std::string _entName)
	{
		transform.Location = location;
		transform.Rotation = rot;
		transform.Scale = scale;
		entityMesh = _mesh;
		entName = _entName;
	}

UreTechEngine::entity::entity(UreTechEngine::vector3& location, mesh* _mesh, std::string _entName)
{
	transform.Location = location;
	entityMesh = _mesh;
	entName = _entName;
}

UreTechEngine::entity::entity(mesh* _mesh, std::string _entName, UreTechEngine::Transform3D trnsfrm)
{
	transform = trnsfrm;
	entityMesh = _mesh;
	entName = _entName;
}
