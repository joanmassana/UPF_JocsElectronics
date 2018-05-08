#include "entity.h"

#include "includes.h"
#include <iostream>


Entity::Entity()
{
	
}

Entity::~Entity()
{

}

void Entity::render() 
{
	//if this was an EntityMesh...
	//Matrix44 global_matrix = getGlobalMatrix();
	//renderMesh( global_matrix );
	for (int i = 0; i < children.size(); i++)
		children[i]->render(); //repeat for every child
}
void Entity::update(float elapsed_time)
{

}

Vector3 Entity::getPosition()
{
	Vector3 pos;
	return pos;
}

void Entity::addChild(Entity* ent)
{

}

void Entity::removeChild(Entity* ent)
{

}

//get the global transformation of this object (not the relative to the parent)
//this function uses recursivity to crawl the tree upwards
Matrix44 Entity::getGlobalMatrix()
{
	if (parent) //if I have a parent, ask his global and concatenate
		return model * parent->getGlobalMatrix();
	return model; //otherwise just return my model as global
}