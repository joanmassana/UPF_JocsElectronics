#include "world.h"
#include "shader.h"
#include "includes.h"
#include "entity.h"
#include <iostream>
using namespace std;

World::World()
{
	//Se crea cielo, isla, etc (paisaje)
	this->terrain = new Entity("island");
	this->sky = new Entity("sky");
}

World::~World()
{

}

void World::render()
{

}

void World::update(float elapsed_time)
{

}