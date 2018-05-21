#include "world.h"
#include "shader.h"
#include "includes.h"
#include "entity.h"
#include <iostream>
using namespace std;

World::World()
{
	//Creamos el mundo
	this->terrain = new Terrain("island");
	this->sky = new Sky("sky");
	this->sea = new Entity("sea");
	//Creamos el avion Player
	this->player = new Airplane("Spitfire", RAF_FIGHTER, Vector3(0,0,0), true);
	//Creamos otros aviones
	Airplane* g1 = new Airplane("Heinkel", LUFTWAFFE_BOMBER, Vector3(-100, 700, -160), false);
	Airplane* g2 = new Airplane("Heinkel", LUFTWAFFE_BOMBER, Vector3(-50, 700, -180), false);
	Airplane* g3 = new Airplane("Heinkel", LUFTWAFFE_BOMBER, Vector3(0, 700, -200), false);
	Airplane* g4 = new Airplane("Heinkel", LUFTWAFFE_BOMBER, Vector3(50, 700, -180), false);
	Airplane* g5 = new Airplane("Heinkel", LUFTWAFFE_BOMBER, Vector3(100, 700, -160), false);
	this->planes.push_back(g1);
	this->planes.push_back(g2);
	this->planes.push_back(g3);
	this->planes.push_back(g4);
	this->planes.push_back(g5);
}

World::~World()
{

}

void World::render()
{
	//World render
	terrain->render();
	sky->render();
	//sea->render();

	//Player render
	player->render();

	//Enenmies render
	for (vector<Entity*>::iterator it = planes.begin(); it != planes.end(); ++it) {
		(*it)->render();
	}
	
	/* DISTANCE CULLING - Slides Optimización Render */

}

void World::update()
{
	terrain->update();
	sky->update();
	sea->update();
	player->update();

	for (vector<Entity*>::iterator it = planes.begin(); it != planes.end(); ++it) {
		(*it)->update();
	}
}