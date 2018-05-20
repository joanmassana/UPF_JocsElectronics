#include "entity.h"
#include "entitymesh.h"
#include "shader.h"
#include "input.h"
#include "includes.h"
#include "texture.h"
#include "mesh.h"
#include <iostream>
using namespace std;

EntityMesh::EntityMesh(string name): Entity(name)
{
	this->name = name;
	shader = Shader::Load("data/shaders/basic.vs", "data/shaders/texture.fs");
}

EntityMesh::~EntityMesh()
{

}

void EntityMesh::render()
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

}

void EntityMesh::update()
{

}

Airplane::Airplane(string name) : EntityMesh(name)
{
	mesh_name = "data/assets/bomber/bomber_axis.ASE";
	texture_name = "data/assets/bomber/bomber_axis.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
	speed = 0.15;
	dirSpeed = 0.12;
	this->model.translate(0, 450, 0);
}

void Airplane::applyLookAt(Camera * camera)
{

}

void Airplane::update()
{
	//Movimiento hacia delante
	model.translate(0, 0, -speed);

	//Camara se mueve con el avión
	Camera::current->lookAt(model*Vector3(0, 0, 20), model*Vector3(0, -10, -10), model.rotateVector(Vector3(0, 1, 0)));

	//Controles
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_E)) this->model.rotate((float)(dirSpeed / 4 * DEG2RAD), Vector3(0.0f, 1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) this->model.rotate((float)(dirSpeed / 4 * DEG2RAD), Vector3(0.0f, -1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(-1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(0.0f, 0.0f, -1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) this->model.rotate((float)(dirSpeed * DEG2RAD), Vector3(0.0f, 0.0f, 1.0f));
}

Terrain::Terrain(string name) : EntityMesh(name)
{
	mesh_name = "data/assets/island/island.ASE";
	texture_name = "data/assets/island/island_color.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
}

void Terrain::update()
{

}

Sky::Sky(string name) : EntityMesh(name)
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

Sea::Sea(string name) : EntityMesh(name)
{
	mesh_name = "data/assets/island/water_deep.ASE";
	texture_name = "data/assets/island/water_deep.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
}

void Sea::update()
{

}
