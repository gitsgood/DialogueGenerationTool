#include "Core.h"
#include "JsonFileManager.h"
#include "UI.h"


int main()
{
    UserInterface UserView;

    const char* WindowName { "Name Of Your Window" };
    const char* HelloMessage{ "Hello World!" };

    const int TextFontSize{ 25 };

    Vector2 MiddleOfScreenPrintTarget = 
    {
        (UserView.PointOfView.target.x -MeasureText(HelloMessage, TextFontSize))/2,
        (UserView.PointOfView.target.y -TextFontSize)/2
    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        UserView.ViewControls();

        BeginDrawing();
        BeginMode2D(UserView.PointOfView);
        {
            ClearBackground(RED);



            DrawText(HelloMessage, int(MiddleOfScreenPrintTarget.x), int(MiddleOfScreenPrintTarget.y), TextFontSize, BLACK);
        }
        EndMode2D();
        EndDrawing();
    }

    CloseWindow();
}