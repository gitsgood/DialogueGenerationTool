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

	/**
	* @brief Makes sure the input string doesn't hold any characters that mess with JSON formatting.
	*
	* @see DialogueParent::StringifyForJson and its implementations for info on how this gets called.
	* 
	* @param[in] inStringToParse The string that needs double checking.
	*
	* @return A string with any singular " or \ characters preceded with a backslash( \ ).
	*/
	static std::string ParserForJson(const std::string& inStringToParse);
};

#endif // !JSONFILEMANAGER_H