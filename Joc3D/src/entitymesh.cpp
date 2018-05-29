#include "entity.h"
#include "entitymesh.h"
#include "shader.h"
#include "input.h"
#include "includes.h"
#include "texture.h"
#include "mesh.h"
#include "world.h"
#include <iostream>
using namespace std;

class World;
vector<Airplane*> Airplane::planes;

EntityMesh::EntityMesh(): Entity()
{
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
		current_shader->setUniform("u_model", this->getGlobalMatrix());

		mesh->render(GL_TRIANGLES, current_shader);

		//disable shader
		current_shader->disable();

		for (int i = 0; i < children.size(); i++) {
			children[i]->render();
		}
	}
}

void EntityMesh::update(float dt)
{

}

Airplane::Airplane(AircraftType type, Vector3 mod, bool isPlayer) : EntityMesh()
{	
	planes.push_back(this);
	switch (type) {
	case RAF_FIGHTER:
		mesh_name = "data/assets/spitfire/spitfire.ASE";
		texture_name = "data/assets/spitfire/spitfire_color_spec.tga";
		speed = 50;
		dirSpeed = 2;
		break;
	case LUFTWAFFE_BOMBER:
		mesh_name = "data/assets/bomber/bomber_axis.ASE";
		texture_name = "data/assets/bomber/bomber_axis.tga";
		speed = 35;
		dirSpeed = 1;
		break;
	}

	this->is_player = isPlayer;
	if (is_player) {
		this->model.translate(0, 1000, 0);
		cout << "Airplane model: " << model.getTranslation().x << ", " << model.getTranslation().y << ", " << model.getTranslation().z << endl;
	}
	else {
		this->model.translate(mod.x, mod.y, mod.z);
	}
	
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
	target = NULL; //BLOQUE IA

	this->torpedo = new Torpedo();
	this->addChild(torpedo);
}


void Airplane::checkIA(float dt) //BLOQUE IA
{
	if (!target)
		return;

	//Rotar hacia el enemigo
	Vector3 pos = target->getGlobalMatrix().getTranslation();
	Vector3 target_pos = target->getGlobalMatrix().getTranslation();
	Vector3 front = getGlobalMatrix().rotateVector(Vector3(0,0,-1));
	if (pos.y < 500) {
		Vector3 newpos = pos + front *  100;
		pos.y = 500;
		target_pos = newpos;
	}
	Vector3 to_target = target_pos - pos;
	float dist = (pos - target_pos).length();
	if (abs(dist) < 0.00001)
		return;

	to_target.normalize();

	front.normalize();

	float FdotT = front.dot(to_target);
	float angle = acos(FdotT);
	if (abs(angle) < 0.00001)
		//Alineado->Disparar?
		return;

	Vector3 axis = front.cross(to_target);
	axis.normalize();

	Matrix44 im = getGlobalMatrix();
	im.inverse();
	axis = im.rotateVector(axis);

	if (axis.length() < 0.001)
		return;

	model.rotate(angle, axis);

}

void Airplane::checkInput(float dt) 
{
	//Controles
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift

	if (Input::isKeyPressed(SDL_SCANCODE_E)) this->model.rotate(dt * dirSpeed/5, Vector3(0.0f, 1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) this->model.rotate(dt * dirSpeed/5, Vector3(0.0f, -1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) this->model.rotate(dt * dirSpeed/2, Vector3(1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) this->model.rotate(dt * dirSpeed/2, Vector3(-1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) this->model.rotate(dt * dirSpeed/2, Vector3(0.0f, 0.0f, -1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) this->model.rotate(dt * dirSpeed/2, Vector3(0.0f, 0.0f, 1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_P)) this->speed += 100 * dt;
	if (Input::isKeyPressed(SDL_SCANCODE_O)) this->speed -= 100 * dt;
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) this->shootTorpedo();
}

void Airplane::shootTorpedo()
{
	if (!torpedo) {
		return;
	}
	torpedo->time_of_life = 10;
	torpedo->is_on = true;

	World::root->addChild(torpedo);	
	this->removeChild(torpedo);
	cout << "Torpedo fired" << endl;
}

void Airplane::applyLookAt(Camera * camera)
{

}

void Airplane::update(float dt)
{
	//Movimiento hacia delante
	model.translate(0, 0, -speed * dt);

	if (is_player) {
		//Camara se mueve con el avión
		if(Game::instance->camera12 == true) {
			checkInput(dt);
			Camera::current->lookAt(model*Vector3(0, 1.5, 10), model*Vector3(0, 0, -10), model.rotateVector(Vector3(0, 1, 0)));			
		}

		if (Game::instance->camera12 == false) {
			std::cout << "camera12: " << Game::instance->camera12 << std::endl;

			if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) Game::instance->camera2->move(Vector3(0.0f, 0.0f, 1.0f));
			if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) Game::instance->camera2->move(Vector3(0.0f, 0.0f, -1.0f));
			if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) Game::instance->camera2->move(Vector3(1.0f, 0.0f, 0.0f));
			if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) Game::instance->camera2->move(Vector3(-1.0f, 0.0f, 0.0f));
		}
	}

	else {
		checkIA(dt);
	}
	for (int i = 0; i < children.size(); i++) {
		children[i]->update(dt);
	}
}

Terrain::Terrain() : EntityMesh()
{
	mesh_name = "data/assets/island/island.ASE";
	texture_name = "data/assets/island/island_color.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
}

void Terrain::update(float dt)
{

}

Sky::Sky() : EntityMesh()
{
	mesh_name = "data/assets/cielo/cielo.ASE";
	texture_name = "data/assets/cielo/cielo.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
}

void Sky::update(float dt)
{	
	this->model.setTranslation(Camera::current->eye.x, Camera::current->eye.y, Camera::current->eye.z);
}

Sea::Sea() : EntityMesh()
{
	mesh_name = "data/assets/island/water_deep.ASE";
	texture_name = "data/assets/island/water_deep.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
}

void Sea::update(float dt)
{

}

Torpedo::Torpedo() : EntityMesh()
{
	mesh_name = "data/assets/torpedo/torpedo.ASE";
	texture_name = "data/assets/torpedo/torpedo.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
	speed = 0.15;
	model.translate(0,-1,0);	
	is_on = false;
}

void Torpedo::update(float dt)
{
	if (is_on && time_of_life > 0) {
		model.translate(0, -dt*10, 0);
		time_of_life -= dt;
	}
}
