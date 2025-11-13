#include "DialogueVisualiser.h"

struct WrappedTextResult 
{
	std::string text; // The text with newlines inserted
	Vector2 size;     // The final width and height
};

WrappedTextResult MeasureAndWrapText(const std::string& text, float maxWidth, int fontSize, int lineSpacing) 
{
	if (text.empty()) {
		return { "", {0, 0} };
	}

	std::stringstream lineStream;
	std::string currentLine;
	std::string word;

	// Use a stringstream to iterate through each word in the original text
	std::stringstream textStream(text);

	while (textStream >> word) {
		// Create a temporary line to test the width
		std::string tempLine = currentLine.empty() ? word : (currentLine + " " + word);

		float tempWidth = MeasureText(tempLine.c_str(), fontSize);

		if (tempWidth > maxWidth) {
			// The new word makes the line too long.
			// So, commit the PREVIOUS line to our result.
			lineStream << currentLine << "\n";

			// The new line starts with the current word.
			currentLine = word;
		}
		else {
			// The new word fits, so update the current line.
			currentLine = tempLine;
		}
	}

	// Add the very last line to the result
	lineStream << currentLine;

	std::string wrappedText = lineStream.str();

	// --- Now, measure the final block to get its dimensions ---
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

	float totalWidth = 0;
	std::string textToWrap;
	float minWidth = MIN_NODE_WIDTH;

	if (dialogueTree.NodeMap.count(elementId)) {
		textToWrap = dialogueTree.NodeMap.at(elementId).NPCDialogue;
	}
	else if (dialogueTree.EdgeMap.count(elementId)) {
		textToWrap = dialogueTree.EdgeMap.at(elementId).PlayerDialogue;
	}

	// --- New Core Logic: Calculate and cache this element's size ---
	WrappedTextResult wrapResult = MeasureAndWrapText(textToWrap, MAX_NODE_WIDTH, TEXT_FONT_SIZE, LINE_SPACING);
	wrappedText[elementId] = wrapResult.text; // Cache the wrapped text for drawing

	Vector2 finalSize = {
		std::max(minWidth, wrapResult.size.x) + TEXT_PADDING * 2,
		wrapResult.size.y + TEXT_PADDING * 2
	};
	elementSizes[elementId] = finalSize; // Cache the final size

	// --- The rest of the function is similar, but uses the dynamic size ---
	if (dialogueTree.NodeMap.count(elementId)) {
		DialogueNode& node = dialogueTree.NodeMap.at(elementId);
		if (node.OutgoingEdgeIds.empty()) {
			totalWidth = finalSize.x;
		}
		else {
			for (const auto& edgeId : node.OutgoingEdgeIds) {
				totalWidth += CalculateSubtreeWidthsRecursive(edgeId, visited);
			}
			totalWidth += (node.OutgoingEdgeIds.size() - 1) * HORIZONTAL_SPACING;
		}
	}
	else if (dialogueTree.EdgeMap.count(elementId)) {
		DialogueEdge& edge = dialogueTree.EdgeMap.at(elementId);
		if (edge.NextNodeId.empty()) {
			totalWidth = finalSize.x;
		}
		else {
			totalWidth = CalculateSubtreeWidthsRecursive(edge.NextNodeId, visited);
		}
	}

	// The total width of a subtree must be at least the width of the root of that subtree.
	totalWidth = std::max(totalWidth, finalSize.x);
	subtreeWidths[elementId] = totalWidth;
	return totalWidth;
}

void DialogueVisualiser::LayoutElementRecursive(const std::string& elementId, Vector2 position, std::unordered_map<std::string, bool>& visited) 
{
	if (visited[elementId] || elementSizes.find(elementId) == elementSizes.end()) return;
	visited[elementId] = true;

	// Get the pre-calculated size for this element
	Vector2 mySize = elementSizes.at(elementId);

	if (dialogueTree.NodeMap.count(elementId)) {
		DialogueNode& node = dialogueTree.NodeMap.at(elementId);
		layoutRects[elementId] = { position.x - mySize.x / 2.0f, position.y, mySize.x, mySize.y };

		float totalChildWidth = subtreeWidths.at(elementId);
		float currentX = position.x - totalChildWidth / 2.0f;

		for (const auto& edgeId : node.OutgoingEdgeIds) {
			float childSubtreeWidth = subtreeWidths.at(edgeId);
			Vector2 edgePos = {
				currentX + childSubtreeWidth / 2.0f,
				position.y + mySize.y + VERTICAL_SPACING
			};
			LayoutElementRecursive(edgeId, edgePos, visited);
			currentX += childSubtreeWidth + HORIZONTAL_SPACING;
		}

	}
	else if (dialogueTree.EdgeMap.count(elementId)) {
		DialogueEdge& edge = dialogueTree.EdgeMap.at(elementId);
		layoutRects[elementId] = { position.x - mySize.x / 2.0f, position.y, mySize.x, mySize.y };

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
	if (visited[elementId] || layoutRects.find(elementId) == layoutRects.end()) return;
	visited[elementId] = true;

	Rectangle currentRect = layoutRects.at(elementId);

	// Get the pre-wrapped text
	const char* textToDraw = wrappedText.at(elementId).c_str();

	// Is it a Node or an Edge?
	if (dialogueTree.NodeMap.count(elementId)) {
		// --- Draw the NODE ---
		DialogueNode& node = dialogueTree.NodeMap.at(elementId);

		DrawRectangleRec(currentRect, BLUE);
		DrawRectangleLinesEx(currentRect, 2, DARKBROWN);
		// Use the wrapped text
		DrawText(textToDraw, currentRect.x + TEXT_PADDING, currentRect.y + TEXT_PADDING, TEXT_FONT_SIZE, WHITE);

		// Draw lines to its children and recurse
		for (const auto& edgeId : node.OutgoingEdgeIds) {
			if (layoutRects.count(edgeId)) {
				Rectangle edgeRect = layoutRects.at(edgeId);
				Vector2 startPos = { currentRect.x + currentRect.width / 2, currentRect.y + currentRect.height };
				Vector2 endPos = { edgeRect.x + edgeRect.width / 2, edgeRect.y };
				DrawLineBezier(startPos, endPos, 2.0f, YELLOW);
				DrawRecursive(edgeId, visited);
			}
		}
	}
	else if (dialogueTree.EdgeMap.count(elementId)) {
		// --- Draw the EDGE ---
		DialogueEdge& edge = dialogueTree.EdgeMap.at(elementId);

		DrawRectangleRec(currentRect, RED);
		DrawRectangleLinesEx(currentRect, 2, MAROON);
		// Use the wrapped text
		DrawText(textToDraw, currentRect.x + TEXT_PADDING, currentRect.y + TEXT_PADDING, TEXT_FONT_SIZE, WHITE);

		// Draw line to its child and recurse
		if (!edge.NextNodeId.empty() && layoutRects.count(edge.NextNodeId)) {
			Rectangle nextNodeRect = layoutRects.at(edge.NextNodeId);
			Vector2 startPos = { currentRect.x + currentRect.width / 2, currentRect.y + currentRect.height };
			Vector2 endPos = { nextNodeRect.x + nextNodeRect.width / 2, nextNodeRect.y };
			DrawLineBezier(startPos, endPos, 2.0f, YELLOW);
			DrawRecursive(edge.NextNodeId, visited);
		}
	}
}