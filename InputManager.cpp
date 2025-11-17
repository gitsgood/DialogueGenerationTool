#include "InputManager.h"
#include "DialogueVisualiser.h"

InputManager::InputManager(DialogueTree& tree)
    : dialogueTree(tree), currentState(State::IDLE), dialogueVisualiser(nullptr)
{
    // Ensure the text buffer is clean when we start.
    memset(textInputBuffer, 0, sizeof(textInputBuffer));
}

void InputManager::Update() {
    // The main state machine logic.
    switch (currentState) {
    case State::IDLE:
        // In IDLE state, we wait for a command to begin.
        // Pressing 'A' will clear the tree and start a new creation process.
        if (IsKeyPressed(KEY_A)) {
            dialogueTree.NodeMap.clear();
            dialogueTree.EdgeMap.clear();
            creationQueue.clear();

            if (dialogueVisualiser) dialogueVisualiser->RebuildLayout();

            // Kick off the process by adding the root node to the queue.
            creationQueue.push_back("START");
            ProcessNextInQueue();
        }
        break;

    case State::AWAITING_NODE_DIALOGUE:
        // Handle text input for the NPC dialogue.
        if (HandleTextInput()) { // Returns true when Enter is pressed
            tempNode.NPCDialogue = textInputBuffer;

            // Transition to the next state: asking for the number of edges.
            memset(textInputBuffer, 0, sizeof(textInputBuffer));
            currentState = State::AWAITING_NODE_EDGE_COUNT;
        }
        break;

    case State::AWAITING_NODE_EDGE_COUNT:
        if (HandleTextInput()) {
            int edgeCount = 0;
            try {
                edgeCount = std::stoi(textInputBuffer); // Safely convert text to integer
            }
            catch (...) {
                // Handle cases where the user types non-numbers.
                edgeCount = 0;
            }

            if (edgeCount > 0) {
                // We now know how many edges to create. Populate the queue.
                for (int i = 0; i < edgeCount; ++i) {
                    // Generate a unique ID for each new edge.
                    std::string newEdgeId = "Edge" + std::to_string(dialogueTree.EdgeMap.size() + i);
                    tempNode.OutgoingEdgeIds.push_back(newEdgeId);
                    creationQueue.push_back(newEdgeId);
                }
            }

            // The temporary node is complete. Finalize it by adding it to the real tree.
            dialogueTree.NodeMap[tempNode.NodeId] = tempNode;
            // NOTE: We need a way to tell the visualiser to update. We'll add this later.

            // Process the next item in the queue (which will be the first edge).
            ProcessNextInQueue();
        }
        break;

    case State::AWAITING_EDGE_DIALOGUE:
        if (HandleTextInput()) {
            tempEdge.PlayerDialogue = textInputBuffer;
            currentState = State::AWAITING_EDGE_CONFIRM_END;
        }
        break;

    case State::AWAITING_EDGE_CONFIRM_END:
        if (IsKeyPressed(KEY_Y)) {
            // The branch CONTINUES. We need to create a new node.
            std::string newNodeId = "Node" + std::to_string(dialogueTree.NodeMap.size());
            tempEdge.NextNodeId = newNodeId;
            dialogueTree.EdgeMap[tempEdge.EdgeId] = tempEdge;

            // IMPORTANT: Add the new node to the FRONT of the queue.
            // This makes the system process it immediately, mimicking depth-first traversal.
            creationQueue.insert(creationQueue.begin(), newNodeId);

            ProcessNextInQueue();

        }
        else if (IsKeyPressed(KEY_N)) {
            // The branch ENDS here.
            tempEdge.NextNodeId = ""; // An empty string signifies the end.
            dialogueTree.EdgeMap[tempEdge.EdgeId] = tempEdge;

            ProcessNextInQueue();
        }
        break;
    }
}

void InputManager::Draw() {
    // For now, let's just draw the text buffer at the bottom of the screen
    // so we can see what we're typing.
    DrawText("INPUT MODE", 20, GetScreenHeight() - 60, 20, WHITE);

    DrawRectangle(18, GetScreenHeight() - 38, GetScreenWidth() - 36, 30, Fade(BLACK, 0.5f));
    DrawText(textInputBuffer, 20, GetScreenHeight() - 35, 20, YELLOW);

    // Blinking cursor to show we're in input mode
    if (fmod(GetTime(), 1.0) > 0.5) {
        int textWidth = MeasureText(textInputBuffer, 20);
        DrawText("_", 20 + textWidth, GetScreenHeight() - 35, 20, YELLOW);
    }
}

void InputManager::SetVisualiser(DialogueVisualiser* visualiser)
{
    dialogueVisualiser = visualiser;
}

// This helper function encapsulates Raylib's character-by-character input logic.
bool InputManager::HandleTextInput() {
    // Get all characters pressed since the last frame
    int key = GetCharPressed();
    while (key > 0) {
        int len = strlen(textInputBuffer);
        // Ensure we don't write past the end of our buffer
        if (len < sizeof(textInputBuffer) - 1) {
            textInputBuffer[len] = (char)key;
            textInputBuffer[len + 1] = '\0'; // Null-terminate the string
        }
        key = GetCharPressed();
    }

    // Handle Backspace
    if (IsKeyPressed(KEY_BACKSPACE)) {
        int len = strlen(textInputBuffer);
        if (len > 0) {
            textInputBuffer[len - 1] = '\0';
        }
    }

    // Return true if the user confirms their input
    return IsKeyPressed(KEY_ENTER);
}

/**
 * @brief Processes the next item in the creation queue. This function acts
 * as our non-blocking "recursive" step.
 */
void InputManager::ProcessNextInQueue() {
    // If the queue is empty, the entire creation process is finished.
    if (creationQueue.empty()) {
        currentState = State::IDLE;
        activeElementId = "";
        printf("Dialogue creation complete!\n");
        return;
    }

    // Get the next ID from the front of the queue and remove it.
    std::string nextId = creationQueue.front();
    creationQueue.erase(creationQueue.begin());

    activeElementId = nextId;

    // Clear the text buffer for the next input.
    memset(textInputBuffer, 0, sizeof(textInputBuffer));

    // Determine what kind of element we need to create and set the state.
    if (nextId.find("Node") != std::string::npos || nextId == "START") {
        // We are creating a node.
        tempNode = DialogueNode("", {}, nextId); // Prepare a blank temporary node.
        currentState = State::AWAITING_NODE_DIALOGUE;
    }
    else if (nextId.find("Edge") != std::string::npos) {
        // We are creating an edge.
        tempEdge = DialogueEdge(nextId, "", ""); // Prepare a blank temporary edge.
        currentState = State::AWAITING_EDGE_DIALOGUE;
    }
}