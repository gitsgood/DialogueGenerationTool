#pragma once

#ifndef DIALOGUEVISUALISER_H
#define DIALOGUEVISUALISER_H

#include "Core.h"
#include "DialogueContent.h"

class DialogueVisualiser
{
public:

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
    const float MAX_NODE_WIDTH = 350.0f; // The key control variable!
    const int TEXT_PADDING = 10;
    const int TEXT_FONT_SIZE = 16;
    const int LINE_SPACING = 4; // Vertical space between wrapped lines
    const float HORIZONTAL_SPACING = 40.0f;
    const float VERTICAL_SPACING = 50.0f;

    void CalculateLayout();

    float CalculateSubtreeWidthsRecursive(const std::string& elementId, std::unordered_map<std::string, bool>& visited);

    void LayoutElementRecursive(const std::string& elementId, Vector2 position, std::unordered_map<std::string, bool>& visited);

    /**
    * @brief Recursively draws the dialogue tree elements and their connections.
    *
    * This function uses the pre-calculated data from the layout phase (layoutRects, wrappedText)
    * to perform the actual drawing operations.
    *
    * @param elementId The ID of the node or edge to start drawing from.
    * @param visited A map to track which elements have already been drawn this frame to prevent cycles.
    */
    void DrawRecursive(const std::string& elementId, std::unordered_map<std::string, bool>& visited);

};

#endif // !DIALOGUEVISUALISER_H