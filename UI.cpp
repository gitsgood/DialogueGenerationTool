#include "UI.h"

UserInterface::UserInterface()
{
	InitWindow(1, 1, "This window exists for the sole purpose of succesfully getting monitor size info.");

	Monitor = GetCurrentMonitor();
	WindowWidth = GetMonitorWidth(Monitor) / 2;
	WindowLength = GetMonitorHeight(Monitor) / 2;

	CloseWindow(); // We close the temporary one. What comes after is what matters.

	InitWindow(WindowWidth, WindowLength, "WindowName");

	PointOfView = { 0 };
	PointOfView.offset = { float(WindowWidth / 2), float(WindowLength / 2) };
	PointOfView.target = { 0.f ,0.f };
	PointOfView.rotation = 0.f;
	PointOfView.zoom = 2.f;

}

void UserInterface::ViewControls()
{
	// Panning with Middle Mouse Button
	if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
		Vector2 delta = GetMouseDelta();
		delta = Vector2Scale(delta, -1.0f / PointOfView.zoom); // Scale by zoom level
		PointOfView.target = Vector2Add(PointOfView.target, delta);
	}

	// Zooming to the Mouse Cursor
	float wheel = GetMouseWheelMove();
	if (wheel == 0) { return; }

	// Get the world point that is under the mouse
	Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), PointOfView);

	// Set the offset to where the mouse is on the screen
	PointOfView.offset = GetMousePosition();

	// Set the target to match the mouse world position
	PointOfView.target = mouseWorldPos;

	// Zoom
	float zoomIncrement = 0.125f;
	PointOfView.zoom += (wheel * zoomIncrement);

	if (PointOfView.zoom < 0.25f) PointOfView.zoom = 0.25f;
	if (PointOfView.zoom > 5.0f) PointOfView.zoom = 5.0f;
	PointOfView.zoom = Clamp(PointOfView.zoom, 0.25f, 5.0f);
}
