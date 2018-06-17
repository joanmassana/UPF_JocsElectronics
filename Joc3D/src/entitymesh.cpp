#include "entity.h"
#include "entitymesh.h"
#include "shader.h"
#include "input.h"
#include "includes.h"
#include "texture.h"
#include "mesh.h"
#include "world.h"
#include "bass.h"
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
		health = 100;
		isAlive = true;
		crashed = false;
		canShoot = true;
		shootTimer = 0;
		rate_of_fire = 25;
		break;
	case LUFTWAFFE_BOMBER:
		mesh_name = "data/assets/bomber/bomber_axis.ASE";
		texture_name = "data/assets/bomber/bomber_axis.tga";
		speed = 40;
		dirSpeed = 1;
		health = 80;
		shootTimer = 0;
		rate_of_fire = 10;
		isAlive = true;
		crashed = false;
		break;
	}

	this->target = NULL;
	//this->target = new Entity();
	//this->target->model.translate(50000.0, 1000.0, -5000.0);
	
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
	if (crashed) {
		return;
	}	
	
	if (isAlive) {
		if (health <= 0) {
			if (!target) {
				target = new Entity();
			}
			target->model.translate(getGlobalMatrix().getTranslation().x, 0, getGlobalMatrix().getTranslation().z - 500.0);
			isAlive = false;
		}
	}

	//Movimiento hacia delante
	model.translate(0, 0, -speed * dt);

	if (is_player) {	//Camara se mueve con el avión
		checkInput(dt);
		Game::instance->cameraPlayer->lookAt(model*Vector3(0, 1.75, 10), model*Vector3(0, 0, -10), model.rotateVector(Vector3(0, 1, 0)));
	}
	else {
		this->checkIA(dt);
	}

	for (int i = 0; i < children.size(); i++) {
		children[i]->update(dt);
	}
}

void Airplane::checkInput(float dt)
{
	//Controles
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift

	if (Input::isKeyPressed(SDL_SCANCODE_E)) this->model.rotate(dt * dirSpeed / 5, Vector3(0.0f, 1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) this->model.rotate(dt * dirSpeed / 5, Vector3(0.0f, -1.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_UP)) this->model.rotate(dt * dirSpeed / 2, Vector3(1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) this->model.rotate(dt * dirSpeed / 2, Vector3(-1.0f, 0.0f, 0.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_LEFT)) this->model.rotate(dt * dirSpeed / 2, Vector3(0.0f, 0.0f, -1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_RIGHT)) this->model.rotate(dt * dirSpeed / 2, Vector3(0.0f, 0.0f, 1.0f));
	if (Input::isKeyPressed(SDL_SCANCODE_P)) this->speed += 20 * dt;
	if (Input::isKeyPressed(SDL_SCANCODE_O)) this->speed -= 20 * dt;
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
		if (canShoot) {
			this->shootGun();
			canShoot = false;
			shootTimer = 0;
		}
		else {
			shootTimer += dt;
			if (shootTimer > (1 / rate_of_fire)) {
				canShoot = true;
			}
		}
	}
	if (Input::isKeyPressed(SDL_SCANCODE_B)) this->bomb();

}

void Airplane::checkIA(float dt) //BLOQUE IA
{
	if (target) {		
		goToTarget(dt);
	}
	return;
}

void Airplane::goToTarget(float dt) //BLOQUE IA
{
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
	if (abs(dist) < 0.00001) {
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

	Matrix44 im = getGlobalMatrix();
	im.inverse();
	axis = im.rotateVector(axis);
	if (axis.length() > 0.01) {
		axis.normalize();
	}

	if (axis.length() < 0.001) {
		return;
	}

	model.rotate(angle*dt/2, axis*-1);

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
	Vector3 pos_right = getGlobalMatrix() * Vector3(1.9,0,-1.9);
	Vector3 pos_left = getGlobalMatrix() * Vector3(-1.9, 0, -1.9);
	Vector3 vel = getGlobalMatrix().rotateVector(Vector3(0,0,-500));
	BulletManager::instance.createBullet(pos_right,vel, 0, this, 10);
	BulletManager::instance.createBullet(pos_left, vel, 0, this, 10);
	
	

	//Audio
	HSAMPLE hSample;
	HCHANNEL hSampleChannel;

	BASS_Init(1, 44100, 0, 0, NULL);
	hSample = BASS_SampleLoad(false, "data/sounds/gunshot.wav", 0, 0, 1, 0);
	hSampleChannel = BASS_SampleGetChannel(hSample, false);
	BASS_ChannelPlay(hSampleChannel, true);
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


void Sky::update(float dt)
{	
	this->model.setTranslation(Camera::current->eye.x, Camera::current->eye.y, Camera::current->eye.z);
}


Sea::Sea() : EntityMesh()
{
	mesh_name = "data/assets/agua/agua.ASE";
	texture_name = "data/assets/agua/agua.tga";
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
		model.translate(0, -dt * 10, 0);
		time_of_life -= dt;
	}
}

BulletManager::BulletManager()
{
	memset(&bullets, 0, sizeof(bullets));
}

void BulletManager::createBullet(Vector3 pos, Vector3 vel, char type, Airplane* author, float ttl)
{
	Bullet b;
	b.position = pos;
	b.velocity = vel;
	b.type = type;
	b.author = author;
	b.ttl = ttl;
	b.damage = 10;

	for (int i = 0; i < max_bullets; i++) {
		Bullet& bullet = bullets[i];
		if (bullet.ttl > 0) {
			continue;
		}
		bullet = b;
		bulletsLeft--;
		break;
	}
}

void BulletManager::render()
{
	Mesh m;
	for (int i = 0; i < max_bullets; i++) {
		Bullet& bullet = bullets[i];
		if (bullet.ttl <= 0) {
			continue;
		}
		m.vertices.push_back(bullet.position);
	}
	glColor4f(1,1,0.3,1);
	glPointSize(4);
	if(m.vertices.size() > 0) {
		m.renderFixedPipeline(GL_POINTS);
	}
}

void BulletManager::update(float dt)
{	
	for (int i = 0; i < max_bullets; i++) {
		Bullet& bullet = bullets[i];
		bullet.ttl -= dt;
		if (bullet.ttl <= 0) {
			continue;
		}		
		bullet.position = bullet.position + bullet.velocity * dt;
		bullet.velocity = bullet.velocity + Vector3(0,-dt*10,0);

		//Detectar colision bala-avion
		for (auto it = Airplane::planes.begin(); it != Airplane::planes.end(); ++it) {

			Vector3 front = bullet.velocity;
			front.normalize();
			Vector3 col_point;
			Vector3 normal;

			if ((*it)->mesh->testRayCollision((*it)->model, bullet.position, front, col_point, normal, 1, false)) {
				if ((*it)->isAlive) {
					if (!(*it)->is_player) {
						cout << "Impact!" << endl;
						(*it)->health -= bullet.damage;		
					}
				}	
			}
		}
	}
}


