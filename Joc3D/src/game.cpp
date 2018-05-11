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

//Todas las meshes que usamos
Mesh* mesh_bomber = NULL;
Mesh* mesh_island = NULL;
Mesh* mesh_sky = NULL;

//Todas las texturas que usamos
Texture* texture_bomber = NULL;
Texture* texture_island = NULL;
Texture* texture_sky = NULL;

//Shaders
Shader* shader = NULL;

float angle = 0;

World* world = new World();

Airplane* bomber = new Airplane("Heinkel");
Entity* island = new Entity("Island");


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

	sky = new Entity("sky");

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 500.f, 100.f),Vector3(0.f,400.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,10000.f); //set the projection, we want to be perspective

	//Load meshes
	mesh_bomber = Mesh::Load("data/assets/bomber/bomber_axis.ASE");
	mesh_island = Mesh::Load("data/assets/island/island.ASE");
	mesh_sky = Mesh::Load("data/assets/cielo/cielo.ASE");

	//Load textures
	texture_bomber = new Texture();
 	texture_bomber->load("data/assets/bomber/bomber_axis.tga");
	texture_island = new Texture();
	texture_island->load("data/assets/island/island_color_luz.tga");
	texture_sky = new Texture();
	texture_sky->load("data/assets/cielo/cielo.tga");

	// example of shader loading
	shader = Shader::Load("data/shaders/basic.vs", "data/shaders/texture.fs");

	bomber->model.translate(0, 450, 0);

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
   
	Shader* current_shader = shader;

	if(current_shader)
	{
		//enable shader
		current_shader->enable();

		//upload uniforms
		current_shader->setUniform("u_color", Vector4(1,1,1,1));
		current_shader->setUniform("u_viewprojection", camera->viewprojection_matrix );
		current_shader->setUniform("u_texture", texture_bomber);
		current_shader->setUniform("u_model", bomber->model);
		current_shader->setUniform("u_time", time);

		mesh_bomber->render(GL_TRIANGLES, current_shader);

		current_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		current_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		current_shader->setUniform("u_texture", texture_island);
		current_shader->setUniform("u_model", island->model);
		current_shader->setUniform("u_time", time);

		mesh_island->render(GL_TRIANGLES, current_shader);

		current_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		current_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		current_shader->setUniform("u_texture", texture_sky);
		current_shader->setUniform("u_model", sky->model);
		current_shader->setUniform("u_time", time);

		mesh_sky->render(GL_TRIANGLES, current_shader);

		//disable shader
		current_shader->disable();
	}
   
	//Draw out world
	drawGrid();

	//render the FPS
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	bomber->update();
	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
	}

	//async input to move the camera around
	//if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
	//if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	//if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
	//if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	//if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f,0.0f, 0.0f) * speed);
	if (Input::wasKeyPressed(SDL_SCANCODE_C)) {
		cout << "This should change camera" << endl;
		// Change Camera::current to another
	}

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
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

