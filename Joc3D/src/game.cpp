#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "entity.h"
#include "world.h"

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

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	world = new World();

	//create our camera
	cameraPlayer = new Camera();
	cameraPlayer->lookAt(Vector3(0.f, 500.f, 100.f),Vector3(0.f,400.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	cameraPlayer->setPerspective(70.f,window_width/(float)window_height,0.1f,100000.f); //set the projection, we want to be perspective

	cameraFree = new Camera();
	cameraFree->lookAt(Vector3(0.f, 500.f, 100.f), Vector3(0.f, 400.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	cameraFree->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	cameraCurrent = new Camera();
	cameraCurrent = cameraPlayer;

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(31.0/255.0, 51.0/255.0, 61.0/255.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	cameraCurrent->enable();

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
   
	world->render(elapsed_time);
   
	//Draw out world
	//drawGrid();

	//render the FPS
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//Render GUI   AQUII

void Game::update(double seconds_elapsed)
{
	world->update(seconds_elapsed);
	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
		cameraFree->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
		cameraFree->rotate(Input::mouse_delta.y * 0.005f, cameraFree->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
	}

	if (Input::isKeyPressed(SDL_SCANCODE_C)) cameraCurrent = cameraPlayer;
	if (Input::isKeyPressed(SDL_SCANCODE_F)) {
		cameraCurrent = cameraFree;
		cameraCurrent->lookAt(cameraPlayer->eye, cameraPlayer->center, cameraPlayer->up);
	}

	if (Input::isKeyPressed(SDL_SCANCODE_W)) cameraFree->move(Vector3(0.0f, 0.0f, 1.0f)*3);
	if (Input::isKeyPressed(SDL_SCANCODE_S)) cameraFree->move(Vector3(0.0f, 0.0f, -1.0f)*3);
	if (Input::isKeyPressed(SDL_SCANCODE_A)) cameraFree->move(Vector3(1.0f, 0.0f, 0.0f)*3);
	if (Input::isKeyPressed(SDL_SCANCODE_D)) cameraFree->move(Vector3(-1.0f, 0.0f, 0.0f)*3);

	
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
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

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
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
	glViewport( 0,0, width, height );
	cameraCurrent->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

