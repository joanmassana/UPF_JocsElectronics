/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
This class wraps the behaviour of a camera. A Camera helps to set the point of view from where we will render the scene.
The most important attributes are  eye and center which say where is the camera and where is it pointing.
This class also stores the matrices used to do the transformation and projection of the scene.
*/

#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include "utils.h";

class Shader;

class Entity
{
public:
	
	//ATTRIBUTES
	std::string name;
	Matrix44 model;			//or transform
	std::string mesh_name;
	std::string lowmesh_name;
	Shader* shader;

	std::string texture_name;
	std::string normal_texture_name;
	std::string detail_texture_name;

	float lod_distance_threshold;

	//CONSTRUCTORS
	Entity();
	virtual ~Entity();

	//METHODS
	virtual void render();
	virtual void update(float elapsed_time);

	Vector3 getPosition();

	Entity* parent;		//pointer to my parent entity
	
	//pointers to my children
	std::vector<Entity*> children;		
	
	//methods
	void addChild(Entity* ent);
	void removeChild(Entity* ent);

	Matrix44 getGlobalMatrix(); //returns transform in world coordinates
};


#endif