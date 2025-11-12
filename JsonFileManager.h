#pragma once

#ifndef JSONFILEMANAGER_H
#define JSONFILEMANAGER_H

#include "Core.h"
#include "DialogueContent.h"

class JsonFileManager
{
public:

	inline static std::filesystem::path SearchPath{ "./" };
	inline static std::string SearchString{ "DialogueJSONs" };
	inline static std::filesystem::path DestinationPath{ "" };
	inline static std::string JsonFileName{ "DialogueFile" };

	inline static std::string NPCId{ "Tommy Wiseau" };

	inline static std::vector<DialogueEdge> DialogueEdgeContainer;
	inline static std::vector<DialogueNode> DialogueNodeContainer;

	/**
	* @brief Will try to find the specific directory for which this tool was designed for.
	*/
	static void FindTargetDirectory();

	/**
	* @brief Opens a new file and inputs the contents gathered by this tool.
	*/
	static void WriteTheDialogueJson();

	/**
	* @brief Will put together a string that is fully JSON formatted.
	* 
	* @see WriteTheDialogueJson for where this gets called.
	* 
	* @return Formatted JSON string container the full amount of information gathered by this tool.
	*/
	static std::string GenerateTheFinalJsonString();

	static const DialogueEdge& GetDialogueEdge(int inVectorIndex) { return DialogueEdgeContainer[inVectorIndex]; } 
	static const DialogueNode& GetDialogueNode(int inVectorIndex) { return DialogueNodeContainer[inVectorIndex]; }
};

#endif // !JSONFILEMANAGER_H