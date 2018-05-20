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
	shader = Shader::Load("data/shaders/basic.vs", "data/shaders/texture.fs");
}

Entity::~Entity()
{

}

void Entity::render() 
{
	Shader* current_shader = shader;

	if (current_shader)
	{
		//enable shader
		current_shader->enable();

		//upload uniforms
		current_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		current_shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
		current_shader->setUniform("u_texture", this->texture);
		current_shader->setUniform("u_model", this->model);

		mesh->render(GL_TRIANGLES, current_shader);

		//disable shader
		current_shader->disable();
	}

	for (int i = 0; i < children.size(); i++)
		children[i]->render(); //repeat for every child
}

void Entity::update()
{

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

Airplane::Airplane(string name) : Entity(name) 
{
	mesh_name = "data/assets/bomber/bomber_axis.ASE";
	texture_name = "data/assets/bomber/bomber_axis.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
	speed = 0.15;
	dirSpeed = 0.2;
	this->model.translate(0, 450, 0);
}

void Airplane::applyLookAt(Camera * camera)
{

}

void Airplane::update()
{
	//Movimiento hacia delante
	model.translate(0,0, -speed);
	
	//Camara se mueve con el avión
	Camera::current->lookAt(model*Vector3(0, 0, 20), model*Vector3(0, -10, -10), model.rotateVector(Vector3(0, 1, 0)));

	//Controles
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_E)) this->model.rotate((float)(dirSpeed/2 * DEG2RAD), Vector3(0.0f, 1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) this->model.rotate((float)(dirSpeed/2 * DEG2RAD), Vector3(0.0f, -1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(-1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(0.0f, 0.0f, -1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(0.0f, 0.0f, 1.0f));
}

Terrain::Terrain(string name): Entity(name)
{
	mesh_name = "data/assets/island/island.ASE";
	texture_name = "data/assets/island/island_color.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
}

void Terrain::update()
{

}

Sky::Sky(string name) : Entity(name)
{
	mesh_name = "data/assets/cielo/cielo.ASE";
	texture_name = "data/assets/cielo/cielo.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
}

void Sky::update()
{
	this->model.setTranslation(Camera::current->eye.x, Camera::current->eye.y, Camera::current->eye.z);
}
