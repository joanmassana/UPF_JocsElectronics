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
#include "entitymesh.h"

const int ROUNDS = 2;

using namespace std;

class Shader;
class Texture;

class World
{
public:

	//ATTRIBUTES
	int round;
	int planesPerRound[ROUNDS] = { 1,2 };
	//int planesPerRound[ROUNDS] = { 1,1,2,2,2,3,3,3,4,5 };
	static Entity* root;
	EntityMesh* terrain;
	EntityMesh* sky;
	EntityMesh* sea;
	EntityMesh* carrier;
	Airplane* player;
	std::vector<Airplane*> planes;

	HSAMPLE hSample;
	HCHANNEL hSampleChannel;

	//float lod_distance_threshold;

	//CONSTRUCTORS
	World();
	virtual ~World();

	//METHODS
	virtual void render(float dt);
	void checkIfRoundEnded();
	virtual void update(float dt);

};

#endif