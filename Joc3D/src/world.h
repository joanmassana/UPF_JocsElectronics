/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
This class wraps the behaviour of a camera. A Camera helps to set the point of view from where we will render the scene.
The most important attributes are  eye and center which say where is the camera and where is it pointing.
This class also stores the matrices used to do the transformation and projection of the scene.
*/

#ifndef WORLD_H
#define WORLD_H

#include "framework.h"
#include "utils.h"
#include "camera.h"
#include "game.h"
#include "entity.h"

using namespace std;

class Shader;
class Texture;

class World
{
public:

	//ATTRIBUTES
	Entity* terrain;
	Entity* sky;
	Entity* sea;

	//float lod_distance_threshold;

	//CONSTRUCTORS
	World();
	virtual ~World();

	//METHODS
	virtual void render();
	virtual void update(float elapsed_time);

};

#endif