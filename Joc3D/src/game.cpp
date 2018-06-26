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
	//BASS_ChannelPlay(hSampleChannel, true);
}

//what to do when the image has to be draw
void Game::render()
{
	switch (state) {
	case MENU:
		renderMenu();
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

	Mesh* m = new Mesh();
	m->createQuad(100, 100, 50, 50, false);
	menuImage = new EntityMesh();
	menuImage->mesh = m;

	Shader* shader = Shader::Load("data/shaders/basic.vs", "data/shaders/texture.fs");
	shader->enable();
	shader->setTexture("u_texture", Texture::Load("data/assets/gui/crosshair.tga"));
	shader->setUniform4("u_color", Vector4(1, 1, 0, 1));
	menuImage->mesh->render(GL_TRIANGLES, shader);
	shader->disable();

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
	stringstream ss;
	ss << "Ammo: ";
	ss << a;
	string str = ss.str();

	//render the FPS
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	drawText(600, 2, str, Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);

	//renderGUI();
	
}

void Game::renderGUI() {

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Mesh quad;

	quad.vertices.push_back(Vector3(0, 0, 0));
	quad.uvs.push_back(Vector2(0, 0));
	quad.vertices.push_back(Vector3(1, 0, 0));
	quad.uvs.push_back(Vector2(1, 0));
	quad.vertices.push_back(Vector3(0, 1, 0));
	quad.uvs.push_back(Vector2(0, 1));

	quad.vertices.push_back(Vector3(1, 0, 0));
	quad.uvs.push_back(Vector2(1, 0));
	quad.vertices.push_back(Vector3(0, 1, 0));
	quad.uvs.push_back(Vector2(0, 1));
	quad.vertices.push_back(Vector3(1, 1, 0));
	quad.uvs.push_back(Vector2(1, 1));

	Shader* shader = Shader::Load("data/shaders/gui.vs", "data/shaders/gui.fs");
	shader->enable();
	shader->setTexture("u_texture", Texture::Load("data/assets/gui/crosshair.tga"));
	shader->setUniform4("u_color", Vector4(1, 1, 0, 1));
	quad.render(GL_TRIANGLES, shader);
	shader->disable();

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
	drawText(450, 450, "GAME OVER", Vector3(1, 1, 1), 2);
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed) {
	switch (state) {
	case MENU:
		updateMenu();
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
	if (Input::wasKeyPressed(SDL_SCANCODE_G)) {
		world = new World();
		state = GAME;
	}
}

void Game::updateGameplay(double seconds_elapsed)
{
	if (Input::wasKeyPressed(SDL_SCANCODE_M)) {
		state = PAUSE;
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
	if (Input::wasKeyPressed(SDL_SCANCODE_M)) {
		state = GAME;
	}
}

void Game::updateEndScreen() {
	
	if (Input::wasKeyPressed(SDL_SCANCODE_G)) {
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

