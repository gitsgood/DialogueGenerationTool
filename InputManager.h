#pragma once

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "Core.h"
#include "DialogueContent.h"

class DialogueVisualiser; 

class InputManager {
public:
    /**
     * @brief Defines the different stages of the dialogue creation process.
     */
    enum class State {
        IDLE,                      // Not currently creating anything.
        AWAITING_NODE_DIALOGUE,    // Waiting for the user to type the NPC's dialogue.
        AWAITING_NODE_EDGE_COUNT,  // Waiting for the user to type the number of player options.
        AWAITING_EDGE_DIALOGUE,    // Waiting for the user to type the player's response.
        AWAITING_EDGE_CONFIRM_END  // Waiting for the user to press 'Y' or 'N' to end the branch.
    };

    InputManager(DialogueTree& tree);

    /**
     * @brief The main update function, called every frame from main().
     * It processes user input based on the current state.
     */
    void Update();

    /**
     * @brief The main draw function, called every frame from main().
     * It displays the relevant UI prompts and text buffer.
     */
    void Draw();

    void SetVisualiser(DialogueVisualiser* visualiser);

private:
    // --- Data & State ---
    DialogueTree& dialogueTree; // A reference to the data we are modifying.
    State currentState;

    // A buffer to hold text as the user is actively typing it.
    // A C-style char array is easiest for simple Raylib text input.
    char textInputBuffer[512];

    // --- Private Helper Methods ---
    /**
     * @brief A generic text input handler to capture keyboard input.
     * @return True if the user has pressed the Enter key, otherwise false.
     */
    bool HandleTextInput();

    void ProcessNextInQueue();

    // A queue to manage the creation process. This replaces recursion.
    // It will hold the IDs of the elements that we still need to create.
    std::vector<std::string> creationQueue;

    // The ID of the node or edge we are currently creating or focusing on.
    std::string activeElementId;

    // Temporary storage for the data we are building.
    // We use these to gather info before adding it to the main DialogueTree.
    DialogueNode tempNode = DialogueNode(); // Initialize to avoid issues
    DialogueEdge tempEdge = DialogueEdge();

    DialogueVisualiser* dialogueVisualiser;
};

#endif // !INPUTMANAGER_H