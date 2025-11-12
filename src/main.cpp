#include "Core.h"
#include "JsonFileManager.h"	// Also includes DialogueContent.h
#include "UI.h"


int main()
{
	UserInterface UserView;

	const char* WindowName{ "Name Of Your Window" };
	const char* HelloMessage{ "Hello World!" };

	const char* VisualiserMessage{ "Your tree goes here" };

	const int TextFontSize{ 25 };

	Vector2 VisualiserMiddleOfScreenPrintTarget =
	{
		UserView.PointOfView.target.x - (MeasureText(VisualiserMessage, TextFontSize) / 2),
		UserView.PointOfView.target.y - (TextFontSize / 2)
	};

	Vector2 InputMiddleOfScreenPrintTarget =
	{
		(GetScreenWidth() - MeasureText(HelloMessage, TextFontSize)) / 2,
		(GetScreenHeight() - TextFontSize) / 2
	};

	SetTargetFPS(60);

	while (!WindowShouldClose()) {

		UserView.ViewControls();

		BeginDrawing();
		switch (UserView.CurrentState)
		{
		case(0):
			ClearBackground(RED);
			DrawText(HelloMessage, int(InputMiddleOfScreenPrintTarget.x), int(InputMiddleOfScreenPrintTarget.y), TextFontSize, BLACK);
			break;
		case(1):
			BeginMode2D(UserView.PointOfView);
			{
				ClearBackground(BLACK);
				DrawText(VisualiserMessage, int(VisualiserMiddleOfScreenPrintTarget.x), int(VisualiserMiddleOfScreenPrintTarget.y), TextFontSize, YELLOW);
			}
			EndMode2D();
			break;
		default:
			break;
		}
		EndDrawing();
	}

	CloseWindow();
}