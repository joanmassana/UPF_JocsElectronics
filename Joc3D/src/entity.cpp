#include "entity.h"
#include "shader.h"
#include "input.h"
#include "includes.h"
#include <iostream>
using namespace std;

Entity::Entity(string name)
{
	this->name = name;
	//shader = Shader::Load("data/shaders/basic.vs", "data/shaders/texture.fs");

}

Entity::~Entity()
{

}

void Entity::render() 
{
	//Shader* current_shader = shader;

	//if (current_shader)
	//{
	//	//enable shader
	//	current_shader->enable();

	//	//upload uniforms
	//	current_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	//	current_shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
	//	current_shader->setUniform("u_texture", this->texture_bomber);
	//	current_shader->setUniform("u_model", this->model);

	//	mesh_bomber->render(GL_TRIANGLES, current_shader);

	//	//disable shader
	//	current_shader->disable();
	//}

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

Airplane::Airplane(string name) : Entity(name) {
	mesh_name = "data/assets/bomber/bomber_axis.ASE";
	texture_name = "data/assets/bomber/bomber_axis.tga";
	speed = 0.15;
	dirSpeed = 0.2;
}

void Airplane::applyLookAt(Camera * camera)
{

}

void Airplane::update()
{
	model.translate(0,0, -speed);
	Game::instance->sky->model.translate(0, 0, -speed);
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_E)) this->model.rotate((float)(dirSpeed/2 * DEG2RAD), Vector3(0.0f, 1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) this->model.rotate((float)(dirSpeed/2 * DEG2RAD), Vector3(0.0f, -1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(-1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(0.0f, 0.0f, -1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(0.0f, 0.0f, 1.0f));
}
