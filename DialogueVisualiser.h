#pragma once

#ifndef DIALOGUEVISUALISER_H
#define DIALOGUEVISUALISER_H

#include "Core.h"
#include "DialogueContent.h"

class DialogueVisualiser
{
public:
    // --- Public Methods ---
    DialogueVisualiser(DialogueTree& tree);
    void Draw();   // Draws the entire tree

private:
    // --- Data ---
    DialogueTree& dialogueTree; // A reference to the data

    // Stores the calculated position and size of each element
    std::unordered_map<std::string, Rectangle> layoutRects;

    // Stores the widths of every subtree.
    std::unordered_map<std::string, float> subtreeWidths;

    // Caches the calculated size (W, H) of each individual element's box
    std::unordered_map<std::string, Vector2> elementSizes;

    // Caches the text with newlines inserted for drawing
    std::unordered_map<std::string, std::string> wrappedText;

    // --- Constants for Layout and Drawing ---
    //const float NODE_WIDTH = 250.0f;
    //const float NODE_HEIGHT = 80.0f;
    //const float EDGE_WIDTH = 200.0f;
    //const float EDGE_HEIGHT = 60.0f;
    const float MIN_NODE_WIDTH = 200.0f;
    const float MAX_NODE_WIDTH = 350.0f;
    const float VERTICAL_SPACING = 50.0f;
    const float HORIZONTAL_SPACING = 40.0f;
    const int TEXT_FONT_SIZE = 16;
    const int TEXT_PADDING = 10;
    const int LINE_SPACING = 5;

    // --- Private Helper Methods ---
    void CalculateLayout();

    float CalculateSubtreeWidthsRecursive(const std::string& elementId, std::unordered_map<std::string, bool>& visited);

    void LayoutElementRecursive(const std::string& elementId, Vector2 position, std::unordered_map<std::string, bool>& visited);

    void DrawRecursive(const std::string& elementId, std::unordered_map<std::string, bool>& visited);

};

#endif // !DIALOGUEVISUALISER_H