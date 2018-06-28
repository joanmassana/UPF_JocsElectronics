#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "entity.h"
#include "world.h"
#include "bass.h"

#include <cmath>

float angle = 0;

World* world;

Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	state = MENU;
	enemyPlanesDestroyed = 0;
	carrierHealth = 5000;
	glClearColor(0, 0, 0, 1);

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable(GL_CULL_FACE); //render both sides of every triangle
	glEnable(GL_DEPTH_TEST); //check the occlusions using the Z buffer

	
	//create our camera
	cameraPlayer = new Camera();
	cameraPlayer->lookAt(Vector3(0.f, 500.f, 100.f), Vector3(0.f, 400.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	cameraPlayer->setPerspective(70.f, window_width / (float)window_height, 0.1f, 100000.f); //set the projection, we want to be perspective

	cameraFree = new Camera();
	cameraFree->lookAt(Vector3(0.f, 500.f, 100.f), Vector3(0.f, 400.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	cameraFree->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	cameraRight = new Camera();
	cameraRight->lookAt(Vector3(0.f, 500.f, 100.f), Vector3(0.f, 400.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	cameraRight->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	cameraLeft = new Camera();
	cameraLeft->lookAt(Vector3(0.f, 500.f, 100.f), Vector3(0.f, 400.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	cameraLeft->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	cameraFront = new Camera();
	cameraFront->lookAt(Vector3(0.f, 500.f, 100.f), Vector3(0.f, 400.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	cameraFront->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	cameraCurrent = new Camera();
	cameraCurrent = cameraPlayer;

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	//Audio
	HSAMPLE hSample;
	HCHANNEL hSampleChannel;

	BASS_Init(1, 44100, 0, 0, NULL);
	hSample = BASS_SampleLoad(false, "data/sounds/music.mp3", 0, 0, 3, BASS_SAMPLE_LOOP);
	hSampleChannel = BASS_SampleGetChannel(hSample, false);
	BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_VOL, 1);
	BASS_ChannelPlay(hSampleChannel, true);
}

//what to do when the image has to be draw
void Game::render()
{
	switch (state) {
	case MENU:
		renderMenu();
		break;
	case HELP:
		renderHelp();
		break;
	case GAME:
		renderGameplay();
		renderGUI();
		break;
	case PAUSE:
		renderPause();
		break;
	case END:
		renderEndScreen();
		break;
	default:
		break;
	}	
}

void Game::renderMenu() {
	//set the clear color (the background color)
	glClearColor(50.0 / 255.0, 50.0 / 255.0, 50.0 / 255.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawText(450, 450, "MENU", Vector3(1, 1, 1), 2);

	

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);	
	
}
void Game::renderHelp() {
	//set the clear color (the background color)
	glClearColor(50.0 / 255.0, 50.0 / 255.0, 50.0 / 255.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawText(450, 450, "HELP", Vector3(1, 1, 1), 2);



	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);

}


void Game::renderGameplay() {

	//set the clear color (the background color)
	glClearColor(56.0 / 255.0, 89.0 / 255.0, 137.0 / 255.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	cameraCurrent->enable();

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	world->render(elapsed_time);

	int a = world->player->ammo;
	stringstream ss1;
	ss1 << "Ammo: ";
	ss1 << a;
	string ammoStr = ss1.str();

	stringstream ss2;
	ss2 << "Planes shot down: ";
	ss2 << enemyPlanesDestroyed;
	string planesStr = ss2.str();

	stringstream ss3;
	ss3 << "Carrier Health: ";
	ss3 << carrierHealth;
	string carrierStr = ss3.str();

	stringstream ss4;
	ss4 << "ROUND ";
	ss4 << world->round;
	string roundStr = ss4.str();

	stringstream ss5;
	ss5 << "Health ";
	ss5 << world->player->health;
	string healthStr = ss5.str();

	//render the FPS
	drawText(2, 2, roundStr, Vector3(1, 1, 1), 2);
	drawText(150, 2, healthStr, Vector3(1, 1, 1), 2);
	drawText(300, 2, ammoStr, Vector3(1, 1, 1), 2);
	drawText(500, 2, planesStr, Vector3(1, 1, 1), 2);
	drawText(800, 2, carrierStr, Vector3(1, 1, 1), 2);

	//drawText(2, 25, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);

	//renderGUI();	
}



void Game::renderGUI() {

	/*Mesh* m = new Mesh();
	m->createQuad(100, 100, 50, 50, false);
	m->registerMesh("menu_mesh");

	menuImage = new EntityMesh();
	menuImage->mesh_name = "menu_mesh";
	menuImage->texture = Texture::Load("data/assets/gui/crosshair.tga");
	menuImage->render();*/

}

void Game::renderPause()
{
	drawText(450, 450, "PAUSE", Vector3(1, 1, 1), 2);
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::renderEndScreen() {
	//set the clear color (the background color)
	glClearColor(50.0 / 255.0, 50.0 / 255.0, 50.0 / 255.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawText(450, 400, "GAME OVER", Vector3(1, 1, 1), 2);

	stringstream ss;
	ss << "Planes shot down: ";
	ss << enemyPlanesDestroyed;
	string str = ss.str();

	drawText(450, 475, str, Vector3(1, 1, 1), 2);
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed) {
	switch (state) {
	case MENU:
		updateMenu();
		break;
	case HELP:
		updateHelp();
		break;
	case GAME:
		updateGameplay(seconds_elapsed);
		break;
	case PAUSE:
		updatePause();
		break;
	case END:
		updateEndScreen();
		break;
	default:
		break;
	}
}

void Game::updateMenu() {
	if (Input::wasKeyPressed(SDL_SCANCODE_G) || (Input::gamepads[0].button[A_BUTTON])) {
		world = new World();
		enemyPlanesDestroyed = 0;
		state = HELP;
	}
}

void Game::updateHelp() {
	if (Input::wasKeyPressed(SDL_SCANCODE_G) || Input::wasKeyPressed(SDL_SCANCODE_TAB) || (Input::gamepads[0].button[A_BUTTON]) || (Input::gamepads[0].button[Y_BUTTON])) {
		state = GAME;
	}
}

void Game::updateGameplay(double seconds_elapsed)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_M) || (Input::gamepads[0].button[START_BUTTON])) {
		state = PAUSE;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB) || (Input::gamepads[0].button[Y_BUTTON])) {
		state = HELP;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_Z)) {
		state = END;
	}

	world->update(seconds_elapsed);
	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

										 //example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		cameraFree->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		cameraFree->rotate(Input::mouse_delta.y * 0.005f, cameraFree->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	if (Input::isKeyPressed(SDL_SCANCODE_C)) cameraCurrent = cameraPlayer;
	if (Input::isKeyPressed(SDL_SCANCODE_F)) {
		cameraCurrent = cameraFree;
		cameraCurrent->lookAt(cameraPlayer->eye, cameraPlayer->center, cameraPlayer->up);
	}

	if (Input::isKeyPressed(SDL_SCANCODE_W)) cameraFree->move(Vector3(0.0f, 0.0f, 1.0f) * 3);
	if (Input::isKeyPressed(SDL_SCANCODE_S)) cameraFree->move(Vector3(0.0f, 0.0f, -1.0f) * 3);
	if (Input::isKeyPressed(SDL_SCANCODE_A)) cameraFree->move(Vector3(1.0f, 0.0f, 0.0f) * 3);
	if (Input::isKeyPressed(SDL_SCANCODE_D)) cameraFree->move(Vector3(-1.0f, 0.0f, 0.0f) * 3);


	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

void Game::updatePause()
{
	if (Input::wasKeyPressed(SDL_SCANCODE_M) || (Input::gamepads[0].button[A_BUTTON])) {
		state = GAME;
	}
}

void Game::updateEndScreen() {
	
	if (Input::wasKeyPressed(SDL_SCANCODE_G) || (Input::gamepads[0].button[A_BUTTON])) {
		delete world;
		state = MENU;
	}
}

//Keyboard event handler (sync input)
void Game::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	case SDLK_F1: Shader::ReloadAll(); break;
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onResize(int width, int height)
{
	std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport(0, 0, width, height);
	cameraCurrent->aspect = width / (float)height;
	window_width = width;
	window_height = height;
}

