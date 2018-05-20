/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
This class wraps the behaviour of a camera. A Camera helps to set the point of view from where we will render the scene.
The most important attributes are  eye and center which say where is the camera and where is it pointing.
This class also stores the matrices used to do the transformation and projection of the scene.
*/

#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include "utils.h"
#include "camera.h"
#include "game.h"

using namespace std;

class Shader;
class Texture;
class Mesh;

class Entity
{
public:

	//ATTRIBUTES
	std::string name;
	Matrix44 model;			//or transform

	//float lod_distance_threshold;

	//CONSTRUCTORS
	Entity(string name);
	virtual ~Entity();

	//METHODS
	virtual void render();
	virtual void update();
};



#endif