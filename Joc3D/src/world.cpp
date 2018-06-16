#include "world.h"
#include "shader.h"
#include "includes.h"
#include "entity.h"
#include "mesh.h"
#include "entitymesh.h"
#include <iostream>

class Mesh; 
class EntityMesh;

using namespace std;

Entity* World::root = NULL;

World::World()
{
	//Root
	root = new Entity();
	//Creamos el mundo
	this->terrain = new Terrain();
	root->addChild(terrain);

	this->sky = new Sky();
	root->addChild(sky);

	this->sea = new Sea();
	root->addChild(sea);

	


	//Creamos el avion Player
	this->player = new Airplane(RAF_FIGHTER, Vector3(0,0,0), true);
	root->addChild(player);
	//Creamos otros aviones
	for (int i = 0; i < 2; i++) {
		Airplane* airplane = new Airplane(LUFTWAFFE_BOMBER, Vector3(-100 + i*50, 700, -160), false);
		planes.push_back(airplane);
	}
}

World::~World()
{

}

void World::render(float dt)
{
	//World render
	root->render();

	//Enenmies render
	for (auto it = planes.begin(); it != planes.end(); ++it) {
		(*it)->render();
	}
	
	BulletManager::instance.render();
	
	


	/* DISTANCE CULLING - Slides Optimización Render */

}

void World::update(float dt)
{
	root->update(dt);

	for (auto it = planes.begin(); it != planes.end(); ++it) {
		(*it)->update(dt);
	}	

	//Detectar colision avion-terreno
	//Vector3 front = Camera::current->center - Camera::current->eye;
	Vector3 front = Game::instance->cameraCurrent->center - Game::instance->cameraCurrent->eye;
	front.normalize();
	Vector3 col_point;
	Vector3 normal;

	if (terrain->mesh->testRayCollision(terrain->model, Game::instance->cameraCurrent->eye, front, col_point, normal, 1, false)) {
		cout << "Col" << endl;
	}

	BulletManager::instance.update(dt);
}