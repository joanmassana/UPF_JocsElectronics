#include "entity.h"
#include "shader.h"
#include "input.h"
#include "includes.h"
#include "texture.h"
#include "mesh.h"
#include <iostream>
using namespace std;


std::vector<Entity*> Entity::to_destroy;

Entity::Entity()
{
	parent = NULL;
}

Entity::~Entity()
{
	if (!parent)
		return;
	for (vector<Entity*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it) {
		if (it != parent->children.end()) {
			parent->children.erase(it);
		}
	}
}

void Entity::destroy()
{
	to_destroy.push_back(this);
	//destroy
}

void Entity::render() 
{
	for (int i = 0; i < children.size(); i++) {
		children[i]->render();
	}
}

void Entity::update(float dt)
{
	for (int i = 0; i < children.size(); i++) {
		children[i]->update(dt);
	}
}

void Entity::addChild(Entity * ent)
{
	ent->parent = this;
	children.push_back(ent);
}

void Entity::removeChild(Entity * ent)
{
	//children.pop(ent);
}

Matrix44 Entity::getGlobalMatrix()
{
	if (parent) {
		return this->model * parent->getGlobalMatrix();
	}
	return this->model;
}

