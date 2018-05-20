#include "entity.h"
#include "shader.h"
#include "input.h"
#include "includes.h"
#include "texture.h"
#include "mesh.h"
#include <iostream>
using namespace std;

Entity::Entity(string name)
{
	this->name = name;
}

Entity::~Entity()
{

}

void Entity::destroy()
{
	to_destroy.push_back(this);
}

void Entity::render() 
{
	for (int i = 0; i < children.size(); i++) {
		children[i]->render();
	}
}

void Entity::update()
{

}

void Entity::addChild(Entity * ent)
{
}

void Entity::removeChild(Entity * ent)
{
}

Matrix44 Entity::getGlobalMatrix()
{
	if (parent) {
		return model * parent->getGlobalMatrix();
	}
	return model;
}

