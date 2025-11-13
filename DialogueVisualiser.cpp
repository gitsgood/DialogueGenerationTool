#include "DialogueVisualiser.h"

// A struct to hold the results of our text processing.
struct WrappedTextResult {
	std::string text; // The final text with newlines inserted for drawing.
	Vector2 size;     // The width and height of the resulting text block.
};

/**
 * @brief Measures and wraps text to fit within a maximum width.
 * @param text The original string to process.
 * @param maxWidth The width at which the text should wrap.
 * @param fontSize The size of the font for measurement.
 * @param lineSpacing The vertical space between lines of text.
 * @return A WrappedTextResult struct containing the wrapped text and its final dimensions.
 */
WrappedTextResult MeasureAndWrapText(const std::string& text, float maxWidth, int fontSize, int lineSpacing) {
	if (text.empty()) {
		return { "", {0, 0} };
	}

	std::stringstream lineStream;
	std::string currentLine;
	std::string word;

	// Use a stringstream to iterate through each word in the original text.
	std::stringstream textStream(text);

	while (textStream >> word) {
		// Create a temporary line to test the width if the new word is added.
		std::string tempLine = currentLine.empty() ? word : (currentLine + " " + word);

		float tempWidth = MeasureText(tempLine.c_str(), fontSize);

		if (tempWidth > maxWidth && !currentLine.empty()) {
			// The new word makes the line too long. Commit the previous line.
			lineStream << currentLine << "\n";
			// The new line starts fresh with the current word.
			currentLine = word;
		}
		else {
			// The new word fits, so update the current line.
			currentLine = tempLine;
		}
	}

	// Add the very last line to the result.
	lineStream << currentLine;
	std::string wrappedText = lineStream.str();

	// --- Now, measure the final wrapped block to get its precise dimensions ---
	float widestLine = 0.0f;
	int lineCount = 0;

	std::stringstream resultStream(wrappedText);
	std::string singleLine;

	while (std::getline(resultStream, singleLine, '\n')) {
		float lineWidth = MeasureText(singleLine.c_str(), fontSize);
		if (lineWidth > widestLine) {
			widestLine = lineWidth;
		}
		lineCount++;
	}

	return {
		wrappedText,
		{
			widestLine,
			(float)lineCount * fontSize + (float)std::max(0, lineCount - 1) * lineSpacing
		}
	};
}

DialogueVisualiser::DialogueVisualiser(DialogueTree& tree) : dialogueTree(tree) 
{
	// Calculate the initial layout of the tree
	CalculateLayout();
}

// --- The Core Layout Logic ---
void DialogueVisualiser::CalculateLayout() 
{
	// Clear all previous layout data
	layoutRects.clear();
	subtreeWidths.clear();

	// --- PASS 1: Measurement ---
	// We need a fresh 'visited' map for each pass.
	std::unordered_map<std::string, bool> visitedPass1;
	CalculateSubtreeWidthsRecursive("START", visitedPass1);

	// --- PASS 2: Placement ---
	std::unordered_map<std::string, bool> visitedPass2;
	// Start the placement from the root node at world origin (0,0)
	LayoutElementRecursive("START", { 0.0f, 0.0f }, visitedPass2);
}

float DialogueVisualiser::CalculateSubtreeWidthsRecursive(const std::string& elementId, std::unordered_map<std::string, bool>& visited)
{
	if (visited[elementId]) return 0;
	visited[elementId] = true;

	// --- Step 1: Calculate THIS element's specific size using the wrapping helper ---
	std::string textToProcess;
	if (dialogueTree.NodeMap.count(elementId)) {
		textToProcess = dialogueTree.NodeMap.at(elementId).NPCDialogue;
	}
	else if (dialogueTree.EdgeMap.count(elementId)) {
		textToProcess = dialogueTree.EdgeMap.at(elementId).PlayerDialogue;
	}
	else {
		textToProcess = "[UNKNOWN ID]";
	}

	// Use the wrapping function to get the final wrapped text and its dimensions.
	WrappedTextResult wrapResult = MeasureAndWrapText(textToProcess, MAX_NODE_WIDTH, TEXT_FONT_SIZE, LINE_SPACING);

	// Cache the wrapped text for the Draw function.
	wrappedText[elementId] = wrapResult.text;

	// Calculate the final size of the box including padding.
	Vector2 finalSize = {
		wrapResult.size.x + (TEXT_PADDING * 2),
		wrapResult.size.y + (TEXT_PADDING * 2)
	};

	// Cache this element's final calculated size.
	elementSizes[elementId] = finalSize;

	// --- Step 2: Calculate the TOTAL width of the subtree rooted at this element ---
	// (This part of the logic remains identical to the previous version)
	float totalWidthOfSubtree = 0;
	if (dialogueTree.NodeMap.count(elementId)) {
		DialogueNode& node = dialogueTree.NodeMap.at(elementId);
		if (node.OutgoingEdgeIds.empty()) {
			totalWidthOfSubtree = finalSize.x;
		}
		else {
			for (const auto& edgeId : node.OutgoingEdgeIds) {
				totalWidthOfSubtree += CalculateSubtreeWidthsRecursive(edgeId, visited);
			}
			totalWidthOfSubtree += (node.OutgoingEdgeIds.size() - 1) * HORIZONTAL_SPACING;
		}
	}
	else if (dialogueTree.EdgeMap.count(elementId)) {
		DialogueEdge& edge = dialogueTree.EdgeMap.at(elementId);
		if (edge.NextNodeId.empty()) {
			totalWidthOfSubtree = finalSize.x;
		}
		else {
			totalWidthOfSubtree = CalculateSubtreeWidthsRecursive(edge.NextNodeId, visited);
		}
	}

	totalWidthOfSubtree = std::max(totalWidthOfSubtree, finalSize.x);
	subtreeWidths[elementId] = totalWidthOfSubtree;
	return totalWidthOfSubtree;
}

void DialogueVisualiser::LayoutElementRecursive(const std::string& elementId, Vector2 position, std::unordered_map<std::string, bool>& visited) 
{
	if (visited[elementId] || elementSizes.find(elementId) == elementSizes.end()) return;
	visited[elementId] = true;

	// Get the pre-calculated dynamic size for this element
	Vector2 mySize = elementSizes.at(elementId);

	if (dialogueTree.NodeMap.count(elementId)) {
		DialogueNode& node = dialogueTree.NodeMap.at(elementId);
		// Place this node, centered horizontally on its given position
		layoutRects[elementId] = { position.x - mySize.x / 2.0f, position.y, mySize.x, mySize.y };

		// Get the total width needed for all direct children subtrees
		float totalChildWidth = subtreeWidths.at(elementId);
		// Determine the starting X position for the first child's center point
		float currentX = position.x - totalChildWidth / 2.0f;

		for (const auto& edgeId : node.OutgoingEdgeIds) {
			float childSubtreeWidth = subtreeWidths.at(edgeId);
			// The position for this child is its center
			Vector2 edgePos = {
				currentX + childSubtreeWidth / 2.0f,
				position.y + mySize.y + VERTICAL_SPACING
			};
			LayoutElementRecursive(edgeId, edgePos, visited);
			// Move the "cursor" for the next child
			currentX += childSubtreeWidth + HORIZONTAL_SPACING;
		}

	}
	else if (dialogueTree.EdgeMap.count(elementId)) {
		DialogueEdge& edge = dialogueTree.EdgeMap.at(elementId);
		// Place this edge, centered at its position
		layoutRects[elementId] = { position.x - mySize.x / 2.0f, position.y, mySize.x, mySize.y };

		// Recurse for the next node (if it exists)
		if (!edge.NextNodeId.empty()) {
			Vector2 nextNodePos = { position.x, position.y + mySize.y + VERTICAL_SPACING };
			LayoutElementRecursive(edge.NextNodeId, nextNodePos, visited);
		}
	}
}


// --- The Core Drawing Logic ---
void DialogueVisualiser::Draw() 
{
	//BeginDrawing();
	ClearBackground(DARKGRAY);

	std::unordered_map<std::string, bool> visited; // To avoid drawing things twice
	// Start drawing from the root
	DrawRecursive("START", visited);

	//DrawFPS(10, 10);
	//EndDrawing();
}

void DialogueVisualiser::DrawRecursive(const std::string& elementId, std::unordered_map<std::string, bool>& visited) 
{
	// Safety Guard: If we have already drawn this element, or if it has no layout, stop.
	if (visited[elementId] || layoutRects.find(elementId) == layoutRects.end()) {
		return;
	}
	// Mark this element as drawn for this frame.
	visited[elementId] = true;

	// Fetch the pre-calculated position/size and the pre-wrapped text.
	Rectangle currentRect = layoutRects.at(elementId);
	const char* textToDraw = wrappedText.at(elementId).c_str();

	// Check if the element is a Node.
	if (dialogueTree.NodeMap.count(elementId)) {
		// --- Draw the NODE ---
		DrawRectangleRec(currentRect, BLUE);
		DrawRectangleLinesEx(currentRect, 2, DARKBROWN);
		// Draw the wrapped text. Raylib handles the newlines ('\n') automatically.
		DrawText(textToDraw, currentRect.x + TEXT_PADDING, currentRect.y + TEXT_PADDING, TEXT_FONT_SIZE, WHITE);

		// --- Find, connect, and recurse for all child EDGES ---
		DialogueNode& node = dialogueTree.NodeMap.at(elementId);
		for (const auto& edgeId : node.OutgoingEdgeIds) {
			// Check if the child edge has a valid layout.
			if (layoutRects.count(edgeId)) {
				Rectangle edgeRect = layoutRects.at(edgeId);

				// Calculate line start and end points (bottom-center of parent to top-center of child).
				Vector2 startPos = { currentRect.x + currentRect.width / 2, currentRect.y + currentRect.height };
				Vector2 endPos = { edgeRect.x + edgeRect.width / 2, edgeRect.y };

				DrawLineBezier(startPos, endPos, 2.0f, YELLOW);

				// Continue the recursive drawing process for the child.
				DrawRecursive(edgeId, visited);
			}
		}
	}
	// Check if the element is an Edge.
	else if (dialogueTree.EdgeMap.count(elementId)) {
		// --- Draw the EDGE ---
		DrawRectangleRec(currentRect, RED);
		DrawRectangleLinesEx(currentRect, 2, MAROON);
		DrawText(textToDraw, currentRect.x + TEXT_PADDING, currentRect.y + TEXT_PADDING, TEXT_FONT_SIZE, WHITE);

		// --- Find, connect, and recurse for the child NODE ---
		DialogueEdge& edge = dialogueTree.EdgeMap.at(elementId);
		// Check if the edge leads to another node and if that node has a valid layout.
		if (!edge.NextNodeId.empty() && layoutRects.count(edge.NextNodeId)) {
			Rectangle nextNodeRect = layoutRects.at(edge.NextNodeId);

			// Calculate line start and end points.
			Vector2 startPos = { currentRect.x + currentRect.width / 2, currentRect.y + currentRect.height };
			Vector2 endPos = { nextNodeRect.x + nextNodeRect.width / 2, nextNodeRect.y };

			DrawLineBezier(startPos, endPos, 2.0f, YELLOW);

			// Continue the recursive drawing process for the child.
			DrawRecursive(edge.NextNodeId, visited);
		}
	}
}