
#ifndef ENTITYMESH_H
#define ENTITYMESH_H

#include "framework.h"
#include "utils.h"
#include "camera.h"
#include "game.h"
#include "entity.h"

using namespace std;

enum AircraftType { RAF_FIGHTER, LUFTWAFFE_BOMBER };

class Shader;
class Texture;
class Mesh;

class EntityMesh : public Entity
{
public:

	//ATTRIBUTES
	Shader* shader;
	Mesh* mesh;
	Texture* texture;

	std::string mesh_name;
	std::string lowmesh_name;

	std::string texture_name;
	std::string normal_texture_name;
	std::string detail_texture_name;

	//float lod_distance_threshold;

	//CONSTRUCTORS
	EntityMesh(string name);
	virtual ~EntityMesh();

	//METHODS
	virtual void render();
	virtual void update();

};

class Torpedo;

class Airplane : public EntityMesh {
public:

	float speed;
	float dirSpeed;
	bool is_player;
	Torpedo* torpedo;

	Airplane(string name, AircraftType type, Vector3 mod, bool isPlayer);
	void applyLookAt(Camera* camera);
	void update();
	void shootTorpedo();
};

class Torpedo : public EntityMesh {
public:
	float speed;
	bool time_of_life;
	bool is_on = true;
	Torpedo(string name);
	void update();
};

class Terrain : public EntityMesh {
public:

	Terrain(string name);
	void update();
};

class Sky : public EntityMesh {
public:

	Sky(string name);
	void update();
};

class Sea : public EntityMesh {
public:

	Sea(string name);
	void update();
};


#endif