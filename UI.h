#pragma once

#ifndef UI_H
#define UI_H

#include "Core.h"

class UserInterface
{
public:

	enum State
	{
		Input,
		VisualiseTree,
	};

	State CurrentState;

	int Monitor;
	int WindowWidth;
	int WindowLength;
	const char* WindowName{ "'Burn The Village' dialogue asset assistant" };

	/**
	* @brief Ensures Raylib can get monitor and window dimension info by initiating and closing a window.
	*/
	void GetScreenInfo();

	Camera2D PointOfView;

	/**
	* @brief Abstracts the camera setup boilerplate in this function.
	*/
	void TreeVisualiserCameraSetUp();

	UserInterface();

	void ViewControls();
	void InputControls();
};

#endif // !UI_H