#include "world.h"
#include "shader.h"
#include "includes.h"
#include "entity.h"
#include <iostream>
using namespace std;

World::World()
{
	//Se crea cielo, isla, etc (paisaje)
	this->terrain = new Terrain("island");
	this->sky = new Sky("sky");
	this->sea = new Entity("sea");
	this->player = new Airplane("Heinkel");
}

World::~World()
{

}

void World::render()
{
	terrain->render();
	sky->render();
	//sea->render();
	player->render();

	/* DISTANCE CULLING - Slides Optimización Render */

}

void World::update()
{
	terrain->update();
	sky->update();
	sea->update();
	player->update();
}