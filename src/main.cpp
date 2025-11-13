#include "Core.h"
#include "JsonFileManager.h"	// Also includes DialogueContent.h
#include "UI.h"
#include "DialogueVisualiser.h"

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

	// Create the editor, which will automatically calculate the layout
	DialogueVisualiser visualiser(myTree);

	const char* HelloMessage{ "Hello World!" };

	const int TextFontSize{ 25 };

	Vector2 InputMiddleOfScreenPrintTarget =
	{
		(GetScreenWidth() - MeasureText(HelloMessage, TextFontSize)) / 2,
		(GetScreenHeight() - TextFontSize) / 2
	};

	SetTargetFPS(60);

	while (!WindowShouldClose()) {

		//UserView.ViewControls();

		BeginDrawing();
		switch (UserView.CurrentState)
		{
		case(0):
			UserView.InputControls();
			ClearBackground(RED);
			DrawText(HelloMessage, int(InputMiddleOfScreenPrintTarget.x), int(InputMiddleOfScreenPrintTarget.y), TextFontSize, BLACK);
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