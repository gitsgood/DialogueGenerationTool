#include "Core.h"
#include "JsonFileManager.h"	// Also includes DialogueContent.h
#include "UI.h"
#include "DialogueVisualiser.h"

#define RAYGUI_IMPLEMENTATION
#include "src/raygui.h"

// --- Helper function to create some test data ---
void CreateTestData(DialogueTree& tree) {
	tree.NodeMap.clear();
	tree.EdgeMap.clear();

	tree.NodeMap.emplace("START", DialogueNode("Hello, traveler! Are you brave? I for one am extremely brave, would you like to hear my stories? I have many, let me tell you more.", { "Edge1", "Edge2", "Edge3"}, "START"));
	tree.NodeMap.emplace("Node_Brave", DialogueNode("Excellent! A true hero.", { "Edge_End", "Edge_FuckYou"}, "Node_Brave"));
	tree.NodeMap.emplace("Node_Coward", DialogueNode("Then run away, coward!", { "Edge_End2", "Edge_FuckYou2"}, "Node_Coward"));
	tree.NodeMap.emplace("Node_Harumph", DialogueNode("Hrphm!", { }, "Node_Harumph"));

	tree.EdgeMap.emplace("Edge1", DialogueEdge("Edge1", "Yes, I am brave!", "Node_Brave"));
	tree.EdgeMap.emplace("Edge2", DialogueEdge("Edge2", "No, I am a coward.", "Node_Coward"));
	tree.EdgeMap.emplace("Edge3", DialogueEdge("Edge3", "Please, no.", "Node_Harumph"));
	tree.EdgeMap.emplace("Edge_End", DialogueEdge("Edge_End", "I see.", ""));
	tree.EdgeMap.emplace("Edge_FuckYou", DialogueEdge("Edge_FuckYou", "Fuck off.", ""));
	tree.EdgeMap.emplace("Edge_End2", DialogueEdge("Edge_End2", "That's rough buddy", ""));
	tree.EdgeMap.emplace("Edge_FuckYou2", DialogueEdge("Edge_FuckYou2", "*clench your fist*", ""));
}

int main()
{
	UserInterface UserView;

	// Create and populate our data
	DialogueTree myTree;
	CreateTestData(myTree);

	// Create the visualiser, which will automatically calculate the layout
	DialogueVisualiser visualiser(myTree);

	const char* HelloMessage{ "Hello World!" };

	const int TextFontSize{ 25 };

	Vector2 InputMiddleOfScreenPrintTarget =
	{
		(GetScreenWidth() - MeasureText(HelloMessage, TextFontSize)) / 2,
		(GetScreenHeight() - TextFontSize) / 2
	};

	SetTargetFPS(60);

	// We need to keep the data outside the loop so it persists
	char nameBuffer[1028] = "Raylib User"; // Buffer to hold text
	bool nameEditMode = false;           // Tracks if we are currently typing in this box

	// Toggle States
	bool showCircle = true;              // Checkbox state
	bool darkMode = false;               // Toggle button state

	// Slider State (Bonus: just to show how easy it is)
	float circleRadius = 50.0f;

	while (!WindowShouldClose()) {

		//UserView.ViewControls();

		BeginDrawing();
		switch (UserView.CurrentState)
		{
		case(0):
			UserView.InputControls();
			//ClearBackground(RED);
			//DrawText(HelloMessage, int(InputMiddleOfScreenPrintTarget.x), int(InputMiddleOfScreenPrintTarget.y), TextFontSize, BLACK);

#pragma region UI
						// Change background based on our "Dark Mode" toggle
			ClearBackground(darkMode ? DARKGRAY : RAYWHITE);

			// --------------------------------------------------------------------------
			// 1. TEXT INPUT (GuiTextBox)
			// --------------------------------------------------------------------------
			// Syntax: GuiTextBox(bounds, buffer, bufferSize, editMode)
			// Returns true if the user presses ENTER or clicks outside (changing focus)
			if (GuiTextBox(Rectangle { 100, 40, 200, 30 }, nameBuffer, 1028, nameEditMode)) {
				// Toggle edit mode when clicked or enter pressed
				nameEditMode = !nameEditMode;
			}
			DrawText("Name:", 30, 45, 20, darkMode ? WHITE : GRAY);


			// --------------------------------------------------------------------------
			// 2. CHECKBOX (GuiCheckBox)
			// --------------------------------------------------------------------------
			// Syntax: GuiCheckBox(bounds, text, &bool_variable)
			// It automatically updates the boolean variable you pass by address.
			GuiCheckBox(Rectangle { 100, 90, 20, 20 }, "Show Circle", & showCircle);


			// --------------------------------------------------------------------------
			// 3. TOGGLE BUTTON (GuiToggle)
			// --------------------------------------------------------------------------
			// Syntax: GuiToggle(bounds, text, &bool_variable)
			// Useful for modes (On/Off). Looks like a pushed button when active.
			GuiToggle(Rectangle { 100, 130, 120, 30 }, "Dark Mode", & darkMode);


			// --------------------------------------------------------------------------
			// BONUS: SLIDER (GuiSlider)
			// --------------------------------------------------------------------------
			// Syntax: GuiSlider(bounds, textLeft, textRight, &value, min, max)
			GuiSlider(Rectangle { 100, 180, 200, 20 }, "Radius ", NULL, & circleRadius, 10.0f, 100.0f);


			// --------------------------------------------------------------------------
			// LOGIC VISUALIZATION
			// --------------------------------------------------------------------------
			// Let's use the variables we just set with the GUI to draw something
			if (showCircle) {
				DrawCircle(500, 225, circleRadius, darkMode ? LIGHTGRAY : RED);

				// Draw the name inside the circle
				int textWidth = MeasureText(nameBuffer, 20);
				DrawText(nameBuffer, 500 - textWidth / 2, 215, 20, darkMode ? BLACK : WHITE);
			}

#pragma endregion
			break;
		case(1):
			UserView.ViewControls();
			BeginMode2D(UserView.PointOfView);
			{
				//visualiser.Update(); // Handle camera
				visualiser.Draw();   // Draw the tree
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