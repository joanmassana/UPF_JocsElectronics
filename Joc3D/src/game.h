/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "camera.h"
#include "utils.h"
 
class Entity;
class EntityMesh;

enum GameState { MENU, HELP, GAME, PAUSE, END };

class Game
{
public:
	static Game* instance;
	Entity* sky;

	EntityMesh* menuImage;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	GameState state;
	int enemyPlanesDestroyed;
	int carrierHealth;

	//some vars
	Camera* cameraPlayer; //our global camera
	Camera* cameraFree; //our global camera
	Camera* cameraCurrent;
	Camera* cameraRight;
	Camera* cameraLeft;
	Camera* cameraFront;

	bool mouse_locked; //tells if the mouse is locked (not seen)

	

	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render();
	void renderMenu();
	void renderHelp();
	void renderGameplay();
	void renderGUI();
	void renderPause();
	void renderEndScreen();

	void update( double dt );
	void updateMenu();
	void updateHelp();
	void updateGameplay(double dt);
	void updatePause();
	void updateEndScreen();

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

};


#endif 