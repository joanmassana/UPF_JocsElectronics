#include "world.h"
#include "game.h"
#include "shader.h"
#include "includes.h"
#include "entity.h"
#include "mesh.h"
#include "entitymesh.h"
#include "bass.h"
#include <iostream>

class Mesh; 
class EntityMesh;

using namespace std;

Entity* World::root = NULL;

World::World()
{
	round = 1;
	//Root
	root = new Entity();
	//Creamos el mundo
	this->terrain = new Terrain();
	root->addChild(terrain);

	this->sky = new Sky();

	this->carrier = new Carrier();
	root->addChild(carrier);
	
	this->sea = new Sea();
	root->addChild(sea);

	//player
	this->player = new Airplane(RAF_FIGHTER, Vector3(0, 0, 0), true);

	//enemigos
	for (int i = 0; i < planesPerRound[round - 1]; i++) {
		Airplane* airplane = new Airplane(LUFTWAFFE_BOMBER, Vector3(1500 + i * 50, 800, 2000 + i * 100), false);
		planes.push_back(airplane);
	}

	//Audio
	BASS_Init(1, 44100, 0, 0, NULL);
	hSample = BASS_SampleLoad(false, "data/sounds/explosion.wav", 0, 0, 1, 0);
	hSampleChannel = BASS_SampleGetChannel(hSample, false);
}

World::~World()
{
	delete root;
	delete sky;
	delete player;
	planes.clear();
}

void World::render(float dt)
{
	//Render sky without z-buffer
	glDisable(GL_DEPTH_TEST);
	sky->model.setTranslation(Game::instance->cameraCurrent->eye.x, Game::instance->cameraCurrent->eye.y, Game::instance->cameraCurrent->eye.z);
	sky->render();
	glEnable(GL_DEPTH_TEST);

	//World render
	root->render();
	player->render();

	//planes render
	for (auto it = planes.begin(); it != planes.end(); ++it) {
		(*it)->render();
	}
	
	BulletManager::instance.render();
	
	


	/* DISTANCE CULLING - Slides Optimización Render */

}

void World::update(float dt)
{
	root->update(dt);
	sky->update(dt);
	player->update(dt);

	checkIfRoundEnded();



	for (auto it = planes.begin(); it != planes.end(); ++it) {
		(*it)->update(dt);

		if ((*it)->crashed) {
			continue;
		}
		
		//Detectar colision avion-terreno
		Vector3 front = (*it)->getGlobalMatrix().rotateVector(Vector3(0, 0, -1));
		front.normalize();
		Vector3 col_point;
		Vector3 normal;

		if (terrain->mesh->testRayCollision(terrain->model, (*it)->getGlobalMatrix().getTranslation(), front, col_point, normal, 10, false)) {
			if ((*it)->is_player) {
				cout << "Player crashed against terrain" << endl;
				(*it)->crashed = true;
				(*it)->isAlive = false;
			
				BASS_ChannelPlay(hSampleChannel, true);
			}
			else {
				cout << "NPC crashed against terrain" << endl;
				(*it)->crashed = true;
				
				BASS_3DVECTOR* explosionPos = new BASS_3DVECTOR((*it)->getGlobalMatrix().getTranslation().x, (*it)->getGlobalMatrix().getTranslation().y, (*it)->getGlobalMatrix().getTranslation().z);
				BASS_3DVECTOR* listenerPos = new BASS_3DVECTOR(planes[0]->getGlobalMatrix().getTranslation().x, planes[0]->getGlobalMatrix().getTranslation().y, planes[0]->getGlobalMatrix().getTranslation().z);
				BASS_3DVECTOR* listenerFront = new BASS_3DVECTOR(planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 0, -1)).x, planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 0, -1)).y, planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 0, -1)).z);
				BASS_3DVECTOR* listenerTop = new BASS_3DVECTOR(planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)).x, planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)).y, planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)).z);

				BASS_ChannelSet3DPosition(hSampleChannel, explosionPos, NULL, NULL);
				BASS_Set3DPosition(listenerPos, NULL, listenerFront, listenerTop);
				BASS_ChannelSet3DAttributes(hSampleChannel, 1, 10, 100, 360, 360, 1);
				BASS_Apply3D();

				BASS_ChannelPlay(hSampleChannel, false);
			}			
		}

		if (sea->mesh->testRayCollision(sea->model, (*it)->getGlobalMatrix().getTranslation(), front, col_point, normal, 10, false)) {
			if ((*it)->is_player) {
				cout << "Player crashed against sea" << endl;
				(*it)->crashed = true;
				(*it)->isAlive = false;

				BASS_ChannelPlay(hSampleChannel, true);
			}
			else {
				cout << "NPC crashed against sea" << endl;
				(*it)->crashed = true;

				BASS_3DVECTOR* explosionPos = new BASS_3DVECTOR((*it)->getGlobalMatrix().getTranslation().x, (*it)->getGlobalMatrix().getTranslation().y, (*it)->getGlobalMatrix().getTranslation().z);
				BASS_3DVECTOR* listenerPos = new BASS_3DVECTOR(planes[0]->getGlobalMatrix().getTranslation().x, planes[0]->getGlobalMatrix().getTranslation().y, planes[0]->getGlobalMatrix().getTranslation().z);
				BASS_3DVECTOR* listenerFront = new BASS_3DVECTOR(planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 0, -1)).x, planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 0, -1)).y, planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 0, -1)).z);
				BASS_3DVECTOR* listenerTop = new BASS_3DVECTOR(planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)).x, planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)).y, planes[0]->getGlobalMatrix().rotateVector(Vector3(0, 1, 0)).z);

				BASS_ChannelSet3DPosition(hSampleChannel, explosionPos, NULL, NULL);
				BASS_Set3DPosition(listenerPos, NULL, listenerFront, listenerTop);
				BASS_ChannelSet3DAttributes(hSampleChannel, 1, 10, 100, 360, 360, 1);
				BASS_Apply3D();

				BASS_ChannelPlay(hSampleChannel, false);
			}
		}
	}		
	BulletManager::instance.update(dt);
}



void World::checkIfRoundEnded() {
	for (auto it = planes.begin(); it != planes.end(); ++it) {
		if (!(*it)->crashed) {
			return;
		}
	}
	cout << "All enemy planes have been destroyed" << endl;
	planes.clear();
	if (round == ROUNDS) {
		Game::instance->state = END;
		cout << "the END" << endl;
	}
	else {
		this->round++;
		player->ammo += player->ammo / 2;
	}	
	for (int i = 0; i < planesPerRound[round - 1]; i++) {
		int n;
		i % 2 == 0 ? n = 1 : n = -1;
		Airplane* airplane = new Airplane(LUFTWAFFE_BOMBER, Vector3(1500 + i * n * 50, 800, 2000 + i * 100), false);
		planes.push_back(airplane);
	}
	cout << "Round " << round << endl;
}

