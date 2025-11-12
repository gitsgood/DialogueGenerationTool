#pragma once

#ifndef UI_H
#define UI_H

#include "Core.h"
#include "DialogueContent.h"

class UserInterface
{
public:

	enum State
	{
		Input,
		VisualiseTree,
	};

	Camera2D PointOfView;

	int Monitor;
	int WindowWidth;
	int WindowLength;

	UserInterface();

	void ViewControls();
};

#endif // !UI_H