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
	switch (type) {
	case RAF_FIGHTER:
		mesh_name = "data/assets/spitfire/spitfire.ASE";
		texture_name = "data/assets/spitfire/spitfire_color_spec.tga";
		speed = 55;
		dirSpeed = 2;
		health = 500;
		isAlive = true;
		crashed = false;
		canShoot = true;
		shootTimer = 0;
		rate_of_fire = 25;
		ammo = 1000;
		BASS_Init(1, 44100, 0, 0, NULL);
		hSample = BASS_SampleLoad(false, "data/sounds/gunshot.wav", 0, 0, 1, 0);
		hSampleChannel = BASS_SampleGetChannel(hSample, false);
		payload = NULL;
		break;
	case LUFTWAFFE_BOMBER:
		planes.push_back(this);
		mesh_name = "data/assets/bomber/bomber_axis.ASE";
		texture_name = "data/assets/bomber/bomber_axis.tga";
		speed = 50;
		dirSpeed = 1;
		health = 50;
		canShoot = true;
		shootTimer = 0;
		rate_of_fire = 10;
		ammo = 1000;
		isAlive = true;
		crashed = false;
		target = new Entity();
		target->model.translate(1900.0, 500, 2000.0);
		float rx = rand() % 2000 - 1000;
		float ry = rand() % 2000 - 1000;
		finish = new Entity();
		finish->model.translate(target->getGlobalMatrix().getTranslation().x + (target->getGlobalMatrix().getTranslation().x - mod.x)*0.75 + rx, 2000, target->getGlobalMatrix().getTranslation().z + (target->getGlobalMatrix().getTranslation().z - mod.z) * 0.75 + ry);
		payload = new Payload();
		payload->model.translate(0,-1,0);
		addChild(payload);
		break;
	}
	
	this->is_player = isPlayer;
	if (is_player) {
		this->model.translate(1900, 800, 1900);
	}
	else {
		this->model.translate(mod.x, mod.y, mod.z);
	}
	
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
	
}

void Airplane::update(float dt)
{
	
	if (isAlive && !is_player) {
		if (health <= 0) {
			Game::instance->enemyPlanesDestroyed++;			
			float rx = rand() % 2000 - 1000;
			float rz = rand() % 2000 - 1000;
			//cout << "Target: " << target->getGlobalMatrix().getTranslation().x << ", " << target->getGlobalMatrix().getTranslation().y << ", " << target->getGlobalMatrix().getTranslation().z << ", " << endl;
			target->model.setTranslation(getGlobalMatrix().getTranslation().x + rx, 0, getGlobalMatrix().getTranslation().z + rz);
			cout << "New target: " << target->getGlobalMatrix().getTranslation().x << ", " << target->getGlobalMatrix().getTranslation().y << ", " << target->getGlobalMatrix().getTranslation().z << ", " << endl;
			isAlive = false;
		}
	}	
	//Movimiento hacia delante
	model.translate(0, 0, -speed * dt);

	if (is_player) {	//Camara se mueve con el avi�n
		if (!isAlive || health < 0) {
			Game::instance->state = END;
		}
		
		checkInput(dt);
		Game::instance->cameraPlayer->lookAt(model*Vector3(0, 1.75, 10), model*Vector3(0, 0, -10), model.rotateVector(Vector3(0, 1, 0)));
		Game::instance->cameraRight->lookAt(model*Vector3(-10, 1.75, 0), model*Vector3(0, 0, 0), model.rotateVector(Vector3(0, 1, 0)));
		Game::instance->cameraLeft->lookAt(model*Vector3(10, 1.75, 0), model*Vector3(0, 0, 0), model.rotateVector(Vector3(0, 1, 0)));
		Game::instance->cameraFront->lookAt(model*Vector3(0, 1.75, -10), model*Vector3(0, 0, 10), model.rotateVector(Vector3(0, 1, 0)));
	}
	else {
		if (!isAlive && speed < 60) {
			speed = speed + 10 * dt;
		}	
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

	//Movimiento lateral - Timon de cola
	if (Input::isKeyPressed(SDL_SCANCODE_E)) this->model.rotate(dt * dirSpeed / 5, Vector3(0.0f, 1.0f, 0.0f));
	if (Input::gamepads[0].axis[TRIGGERS] < -0.2) this->model.rotate(dt * dirSpeed / 2 * Input::gamepads[0].axis[TRIGGERS], Vector3(0.0f, 1.0f, 0.0f));

	if (Input::isKeyPressed(SDL_SCANCODE_Q)) this->model.rotate(dt * dirSpeed / 5, Vector3(0.0f, -1.0f, 0.0f));
	if (Input::gamepads[0].axis[TRIGGERS] > 0.2) this->model.rotate(dt * dirSpeed / 2 * Input::gamepads[0].axis[TRIGGERS], Vector3(0.0f, -1.0f, 0.0f));

	//ARRIBA
	if (Input::isKeyPressed(SDL_SCANCODE_UP)) this->model.rotate(dt * dirSpeed / 2, Vector3(1.0f, 0.0f, 0.0f));
	if (Input::gamepads[0].axis[LEFT_ANALOG_Y] > 0.2) this->model.rotate(dt * dirSpeed / 2 * Input::gamepads[0].axis[LEFT_ANALOG_Y], Vector3(1.0f, 0.0f, 0.0f));
	//ABAJO
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN)) this->model.rotate(dt * dirSpeed / 2, Vector3(-1.0f, 0.0f, 0.0f));
	if (Input::gamepads[0].axis[LEFT_ANALOG_Y] < 0.2) this->model.rotate(dt * dirSpeed / 2 * Input::gamepads[0].axis[LEFT_ANALOG_Y], Vector3(-1.0f, 0.0f, 0.0f));
	//IZQ
	if (Input::isKeyPressed(SDL_SCANCODE_LEFT)) this->model.rotate(dt * dirSpeed / 2, Vector3(0.0f, 0.0f, -1.0f));
	if (Input::gamepads[0].axis[LEFT_ANALOG_X] < 0.2) this->model.rotate(dt * dirSpeed / 2 * Input::gamepads[0].axis[LEFT_ANALOG_Y], Vector3(0.0f, 0.0f, -1.0f));
	//DER
	if (Input::isKeyPressed(SDL_SCANCODE_RIGHT)) this->model.rotate(dt * dirSpeed / 2, Vector3(0.0f, 0.0f, 1.0f));
	if (Input::gamepads[0].axis[LEFT_ANALOG_X] > 0.2) this->model.rotate(dt * dirSpeed / 2 * Input::gamepads[0].axis[LEFT_ANALOG_Y], Vector3(0.0f, 0.0f, 1.0f));
	//Acelerar
	if (Input::isKeyPressed(SDL_SCANCODE_0)) {
		if(this->speed < 80) this->speed += 20 * dt;
		cout << this->speed << endl;
	}
	if (Input::gamepads[0].button[A_BUTTON]) {
		if (this->speed < 80) this->speed += 20 * dt;
	}
	//Frenar
	if (Input::isKeyPressed(SDL_SCANCODE_9)) {
		if (this->speed > 40) this->speed -= 20 * dt;
		cout << this->speed << endl;
	}
	if (Input::gamepads[0].button[X_BUTTON]) {
		if (this->speed > 40) this->speed -= 20 * dt;
	}

	//Disparar
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) || (Input::gamepads[0].axis[TRIGGERS] > 0.4)) {		//Trigger derecho
		if (canShoot) {
			this->shootGun_player();
			canShoot = false;
			shootTimer = 0;
		}
		else {
			shootTimer += dt;
			if (shootTimer > (1 / rate_of_fire) && ammo > 0) {
				canShoot = true;
			}
		}
	}


	//Camaras
	if (Input::isKeyPressed(SDL_SCANCODE_1) || Input::gamepads[0].button[PAD_DOWN]) {
		Game::instance->cameraCurrent = Game::instance->cameraFront;
	}	else if(Input::isKeyPressed(SDL_SCANCODE_2) || Input::gamepads[0].button[PAD_LEFT]){
		Game::instance->cameraCurrent = Game::instance->cameraLeft;
	}	else if (Input::isKeyPressed(SDL_SCANCODE_3) || Input::gamepads[0].button[PAD_RIGHT]) {
		Game::instance->cameraCurrent = Game::instance->cameraRight;
	}	else {
		Game::instance->cameraCurrent = Game::instance->cameraPlayer;
	}
}

void Airplane::checkIA(float dt) //BLOQUE IA
{	
	if (!isAlive) {
		goToTarget(dt, target);
	}
	else {
		payload ? goToTarget(dt, target) : goToTarget(dt, finish);
		if (targetReached(target)) bomb();		
		if (targetReached(finish)) {
			cout << "Finish Reached" << endl;
			isAlive = false;
			crashed = true;
		}
		
	}	
	if ((World::instance->player->getGlobalMatrix().getTranslation()-this->getGlobalMatrix().getTranslation()).length() < 300 && isAlive) {
		if (canShoot) {
			this->shootGun_enemy();
			this->canShoot = false;
			this->shootTimer = 0;
		}
		else {
			this->shootTimer += dt;
			if (this->shootTimer > (1 / this->rate_of_fire) && this->ammo > 0) {
				this->canShoot = true;
			}
		}
	}
	return;
}

void Airplane::goToTarget(float dt, Entity* t) //BLOQUE IA
{
	if (!t) return;
	//Rotar hacia el enemigo
	Vector3 pos = getGlobalMatrix().getTranslation();
	Vector3 target_pos = t->getGlobalMatrix().getTranslation();
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

bool Airplane::targetReached(Entity* t) {
	return ((getGlobalMatrix().getTranslation() - t->getGlobalMatrix().getTranslation()).length() < 50);
}

void Airplane::bomb()
{
	if (!payload) {
		return;
	}
	payload->time_of_life = 10;
	payload->is_on = true;
		
	Matrix44 glob = payload->getGlobalMatrix();
	this->removeChild(payload);
	World::root->addChild(payload);
	payload->model = glob;
	this->payload = NULL;
	cout << "Payload fired" << endl;
}

void Airplane::shootGun_player()
{
	Vector3 pos_right = getGlobalMatrix() * Vector3(1.9,0,-1.9);
	Vector3 pos_left = getGlobalMatrix() * Vector3(-1.9, 0, -1.9);
	Vector3 vel = getGlobalMatrix().rotateVector(Vector3(0,0,-500));
	BulletManager::instance.createBullet(pos_right,vel, 0, this, 10, 15);
	BulletManager::instance.createBullet(pos_left, vel, 0, this, 10, 15);
	this->ammo--;
	
	//Audio	
	BASS_ChannelSetAttribute(this->hSampleChannel, BASS_ATTRIB_VOL, 0.1);
	BASS_ChannelPlay(this->hSampleChannel, true);
}

void Airplane::shootGun_enemy()
{
	Vector3 pos = getGlobalMatrix() * Vector3(0, 1, 0);
	float rx = rand() % 20 - 10;
	float ry = rand() % 20 - 10;
	float rz = rand() % 20 - 10;
	Vector3 ran = Vector3(rx, ry, rz);
	Vector3 dir = ((World::instance->player->getGlobalMatrix() * World::instance->player->mesh->box.center) + ran) - this->getGlobalMatrix().getTranslation();
	dir.normalize();
	Vector3 vel = dir*500;
	BulletManager::instance.createBullet(pos, vel, 0, this, 10, 10);

	//Audio	
	BASS_ChannelSetAttribute(this->hSampleChannel, BASS_ATTRIB_VOL, 0.1);
	BASS_ChannelPlay(this->hSampleChannel, true);
}

void Airplane::applyLookAt(Camera * camera)
{

}

void Airplane::renderPlaneFinder()
{
	Mesh m;
	for (int i = 0; i < planes.size(); i++) {	
		if (planes[i]->isAlive) {
			m.vertices.push_back(planes[i]->getGlobalMatrix().getTranslation());
		}		
	}
	glColor4f(0.65, 0, 0, 1);
	glPointSize(14);
	if (m.vertices.size() > 0) {
		m.renderFixedPipeline(GL_POINTS);
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
	Mesh* sub_div_plane = new Mesh();
	sub_div_plane->createSubdividedPlane(100000, 10, true);
	sub_div_plane->registerMesh("sub_div_mesh");

	//mesh_name = "data/assets/agua/agua.ASE";
	texture_name = "data/assets/agua/agua.tga";
	texture = Texture::Load(texture_name.c_str());
	//mesh = Mesh::Load(mesh_name.c_str());

	mesh_name = "sub_div_mesh";
	mesh = Mesh::Load(mesh_name.c_str());
	shader = Shader::Load("data/shaders/basic.vs", "data/shaders/water.fs");

}

void Sea::update(float dt)
{
	this->model.setTranslation(Camera::current->eye.x, 0, Camera::current->eye.z);
}

Payload::Payload() : EntityMesh()
{
	mesh_name = "data/assets/torpedo/bomb.ASE";
	texture_name = "data/assets/torpedo/bomb.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
	speed = 0.15;
	model.translate(0,-1,0);	
	is_on = false;
}

void Payload::update(float dt)
{
	if (is_on && time_of_life > 0) {
		Matrix44 gl = getGlobalMatrix();
		gl.translate(0, -dt * 50, 0);
		model.setTranslation(gl.getTranslation().x, gl.getTranslation().y, gl.getTranslation().z);

		if (this->getGlobalMatrix().getTranslation().y < 0) {
			Game::instance->carrierHealth -= 250;
			cout << "Carier Hit" << endl;
			time_of_life = 0;
		}

	}
}

Carrier::Carrier() : EntityMesh()
{
	mesh_name = "data/assets/carrier/aircarrier.ASE";
	texture_name = "data/assets/carrier/aircarrier_metal.tga";
	texture = Texture::Load(texture_name.c_str());
	mesh = Mesh::Load(mesh_name.c_str());
	model.translate(1900, 0, 2000);
	health = 5000;
}

void Carrier::update(float dt)
{
	if (Game::instance->carrierHealth <= 0) {
		Game::instance->state = END;
	}
}

BulletManager::BulletManager()
{
	memset(&bullets, 0, sizeof(bullets));
}

void BulletManager::createBullet(Vector3 pos, Vector3 vel, char type, Airplane* author, float ttl, float damage)
{
	Bullet b;
	b.position = pos;
	b.velocity = vel;
	b.type = type;
	b.author = author;
	b.ttl = 2;
	b.damage = damage;

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
	Mesh m;
	for (int i = 0; i < max_bullets; i++) {
		Bullet& bullet = bullets[i];
		if (bullet.ttl <= 0) {
			continue;
		}
		m.vertices.push_back(bullet.position);
	}
	glColor4f(1,1,0.3,1);
	glPointSize(3);
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

		//Detectar colision bala-avion enemigo
		for (auto it = Airplane::planes.begin(); it != Airplane::planes.end(); ++it) {

			Vector3 front = bullet.velocity;
			front.normalize();
			Vector3 col_point;
			Vector3 normal;

			if ((*it)->mesh->testRayCollision((*it)->model, bullet.position, front, col_point, normal, 1, false)) {
				if ((*it)->isAlive) {
					if (!(*it)->is_player && bullet.author == World::instance->player) {
						cout << "Impact!" << endl;
						(*it)->health -= bullet.damage;		
						bullet.ttl = 0;
					}
				}	
			}
		}

		//Detectar colision bala-avion player	

		Vector3 front = bullet.velocity;
		front.normalize();
		Vector3 col_point;
		Vector3 normal;

		if (World::instance->player->mesh->testRayCollision(World::instance->player->model, bullet.position, front, col_point, normal, 1, false)) {
			if (World::instance->player->isAlive) {
				if (bullet.author != World::instance->player) {
					cout << "You're hit!" << endl;
					World::instance->player->health -= bullet.damage;
					bullet.ttl = 0;
				}
			}
		}
		
	}
}


