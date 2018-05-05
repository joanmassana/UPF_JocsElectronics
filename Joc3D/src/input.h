#ifndef INPUT_H
#define INPUT_H

#include "includes.h"
#include "utils.h"
#include "framework.h"

//mapped as in SDL
enum Gamepad
{
	//axis
	LEFT_ANALOG_X = 0,
	LEFT_ANALOG_Y = 1,
	RIGHT_ANALOG_X = 4,
	RIGHT_ANALOG_Y = 3,
	TRIGGERS = 2, //both triggers share an axis (positive is right, negative is left trigger)

	//buttons
	A_BUTTON = 0,
	B_BUTTON = 1,
	X_BUTTON = 2,
	Y_BUTTON = 3,
	LB_BUTTON = 4,
	RB_BUTTON = 5,
	BACK_BUTTON = 6,
	START_BUTTON = 7,
	LEFT_ANALOG_BUTTON = 8,
	RIGHT_ANALOG_BUTTON = 9
};

enum HATState
{
	PAD_CENTERED = 0x00,
	PAD_UP = 0x01,
	PAD_RIGHT = 0x02,
	PAD_DOWN = 0x04,
	PAD_LEFT = 0x08,
	PAD_RIGHTUP = (PAD_RIGHT | PAD_UP),
	PAD_RIGHTDOWN = (PAD_RIGHT | PAD_DOWN),
	PAD_LEFTUP = (PAD_LEFT | PAD_UP),
	PAD_LEFTDOWN = (PAD_LEFT | PAD_DOWN)
};

struct GamepadState
{
	int num_axis;	//num analog sticks
	int num_buttons; //num buttons
	float axis[8]; //analog stick
	char button[16]; //buttons
	char prev_button[16]; //buttons in the previous frame
	char direction; //which direction is the left stick pointing at
	char prev_direction; //which direction was the left stick before
	HATState hat; //digital pad

	bool isButtonPressed(int num) { return button[num] != 0; }
	bool wasButtonPressed(int num) { return (button[num] & !prev_button[num]) != 0; }
	bool didDirectionChanged(char dir) { return direction != prev_direction && (direction & dir) != 0; }
};


class Input {
public:
	//keyboard state
	static const Uint8* keystate;
	static Uint8 prev_keystate[SDL_NUM_SCANCODES]; //previous before

	//mouse state
	static int mouse_state; //tells which buttons are pressed
	static Vector2 mouse_position; //last mouse position
	static Vector2 mouse_delta; //mouse movement in the last frame

	//keyboard
	static bool isKeyPressed(int key_code) { return keystate[key_code] != 0; }
	static bool wasKeyPressed(int key_code) { return keystate[key_code] != 0 && prev_keystate[key_code] == 0; }

	//gamepad state
	static GamepadState gamepads[4];

	//gamepad
	static bool isButtonPressed(int button, int pad = 0) { return gamepads[pad].isButtonPressed(button); }
	static bool wasButtonPressed(int button, int pad = 0) { return gamepads[pad].wasButtonPressed(button); }

	//mouse
	static void centerMouse();

	static void init( SDL_Window* window );
	static void update();

	static SDL_Joystick* openGamepad(int index);
	static void updateGamepadState(SDL_Joystick* joystick, GamepadState& state);
};



#endif