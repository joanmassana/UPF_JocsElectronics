#include "entity.h"
#include "entitymesh.h"
#include "shader.h"
#include "input.h"
#include "includes.h"
#include "texture.h"
#include "mesh.h"
#include "world.h"
#include <iostream>
#include <algorithm>
using namespace std;

class World;
vector<Airplane*> Airplane::planes;
BulletManager BulletManager::instance;

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
		current_shader->setUniform("u_viewprojection", Game::instance->cameraCurrent->viewprojection_matrix);
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
		speed = 40;
		dirSpeed = 1;
		break;
	}

	this->target = new Entity();
	this->target->model.translate(5000.0, 5000.0, -5000.0);
	cout << "Target is: " << target->getGlobalMatrix().getTranslation().x << ", " << target->getGlobalMatrix().getTranslation().y << ", " << target->getGlobalMatrix().getTranslation().z << endl;

	this->is_player = isPlayer;
	if (is_player) {
		this->model.translate(0, 1000, 0);
	}
	else {
		this->model.translate(mod.x, mod.y, mod.z);
	}
	
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
	
	this->torpedo = new Torpedo();
	this->addChild(torpedo);
}

void Airplane::update(float dt)
{
	//Movimiento hacia delante
	model.translate(0, 0, -speed * dt);

	if (is_player) {	//Camara se mueve con el avi�n
		
		checkInput(dt);
		Game::instance->cameraPlayer->lookAt(model*Vector3(0, 1.5, 10), model*Vector3(0, 0, -10), model.rotateVector(Vector3(0, 1, 0)));			
		
	}
	else {
		this->checkIA(dt);
	}
	for (int i = 0; i < children.size(); i++) {
		children[i]->update(dt);
	}
}

void Airplane::checkIA(float dt) //BLOQUE IA
{
	if (target == NULL) {
		//cout << "No target" << endl;
		return;
	}
	//cout << "Target acquired" << endl;

	//Rotar hacia el enemigo
	Vector3 pos = getGlobalMatrix().getTranslation();
	Vector3 target_pos = target->getGlobalMatrix().getTranslation();
	Vector3 front = getGlobalMatrix().rotateVector(Vector3(0, 0, -1));

	if (pos.y < 500) {
		Vector3 newpos = pos + front * 100;
		pos.y = 500;
		target_pos = newpos;
	}
	Vector3 to_target = target_pos - pos;

	float dist = (pos - target_pos).length();
	if (abs(dist) < 0.00001){
		return;
	}

	to_target.normalize();
	if (front.length() > 0.01) {
		front.normalize();
	}

	float FdotT = front.dot(to_target);
	if (FdotT < -1) {
		FdotT = -1;
	}
	else if (FdotT > 1) {
		FdotT = 1;
	}

	float angle = acos(FdotT);

	if (abs(angle) < 0.00001) {
		//Alineado->Disparar?
		return;
	}

	Vector3 axis = front.cross(to_target);
	//if (axis.length() > 0.01) {
	//	axis.normalize();
	//}
	
	Matrix44 im = getGlobalMatrix();
	im.inverse();
	axis = im.rotateVector(axis);
	axis.normalize();

	if (axis.length() < 0.001) {
		return;
	}

	model.rotate(angle, axis*-1);

}

void Airplane::checkInput(float dt) 
{
	//Controles
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift

	if (Input::isKeyPressed(SDL_SCANCODE_E)) this->model.rotate(dt * dirSpeed/5, Vector3(0.0f, 1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) this->model.rotate(dt * dirSpeed/5, Vector3(0.0f, -1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_UP)) this->model.rotate(dt * dirSpeed/2, Vector3(1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) this->model.rotate(dt * dirSpeed/2, Vector3(-1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_LEFT)) this->model.rotate(dt * dirSpeed/2, Vector3(0.0f, 0.0f, -1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_RIGHT)) this->model.rotate(dt * dirSpeed/2, Vector3(0.0f, 0.0f, 1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_P)) this->speed += 100 * dt;
	if (Input::isKeyPressed(SDL_SCANCODE_O)) this->speed -= 100 * dt;
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) this->shootGun();
	if (Input::isKeyPressed(SDL_SCANCODE_B)) this->bomb();
}

void Airplane::bomb()
{
	if (!torpedo) {
		return;
	}
	torpedo->time_of_life = 10;
	torpedo->is_on = true;
		
	Matrix44 glob = torpedo->getGlobalMatrix();
	this->removeChild(torpedo);
	World::root->addChild(torpedo);
	torpedo->model = glob;
	this->torpedo = NULL;
	cout << "Torpedo fired" << endl;
}

void Airplane::shootGun()
{
	Vector3 pos = getGlobalMatrix() * Vector3(0,0,-10);
	Vector3 vel = getGlobalMatrix().rotateVector(Vector3(0,0,-500));
	BulletManager::instance.createBullet(pos,vel, 0, this, 10);
}

void Airplane::applyLookAt(Camera * camera)
{

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

/*void Sky::render()
{
	glDisable(GL_DEPTH_TEST); //check the occlusions using the Z buffer
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

}*/

void Sky::update(float dt)
{	
	this->model.setTranslation(Game::instance->cameraCurrent->eye.x, Game::instance->cameraCurrent->eye.y, Game::instance->cameraCurrent->eye.z);
}



Sea::Sea() : EntityMesh()
{
	mesh_name = "data/assets/agua/agua.ASE";
	texture_name = "data/assets/agua/agua.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
}

/*
void Sea::render()
{
	glDisable(GL_DEPTH_TEST); //check the occlusions using the Z buffer
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer
}*/

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
	//cout << "Position is: " << getGlobalMatrix().getTranslation().x << ", " << getGlobalMatrix().getTranslation().y << ", " << getGlobalMatrix().getTranslation().z << endl;
}

BulletManager::BulletManager()
{
	memset(&bullets, 0, sizeof(bullets));
}

void BulletManager::createBullet(Vector3 pos, Vector3 vel, char type, Airplane* author, float ttl)
{
	//cout << "Creating bullet" << endl;
	Bullet b;
	b.position = pos;
	b.velocity = vel;
	b.type = type;
	b.author = author;
	b.ttl = ttl;

	for (int i = 0; i < max_bullets; i++) {
		Bullet& bullet = bullets[i];
		if (bullet.ttl > 0) {
			continue;
		}
		bullet = b;
		break;
	}
}

void BulletManager::render()
{
	//cout << "Rendering bullet" << endl;
	Mesh m;
	for (int i = 0; i < max_bullets; i++) {
		Bullet& bullet = bullets[i];
		if (bullet.ttl <= 0) {
			continue;
		}
		m.vertices.push_back(bullet.position);
	}
	glColor4f(1,1,0.3,1);
	glPointSize(2);
	if(m.vertices.size() > 0) {
		m.renderFixedPipeline(GL_POINTS);
	}
}

void BulletManager::update(float dt)
{
	//cout << "Updating bullet" << endl;
	for (int i = 0; i < max_bullets; i++) {
		Bullet& bullet = bullets[i];
		bullet.ttl -= dt;
		if (bullet.ttl <= 0) {
			continue;
		}
		
		bullet.position = bullet.position + bullet.velocity * dt;
		bullet.velocity = bullet.velocity + Vector3(0,-dt,0);
	}
}
