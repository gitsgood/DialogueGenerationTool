#include "DialogueContent.h"
#include "JsonFileManager.h"

std::string DialogueNode::StringifyForJson() const
{
	std::ostringstream JsonNodeObject;
	JsonNodeObject
		<< "\t\t{\n\t\t\t\"NodeId\": \"" << JsonFileManager::ParserForJson(NodeId) << "\",\n"
		<< "\t\t\t\"NPCDialogue\": \"" << JsonFileManager::ParserForJson(NPCDialogue) << "\",\n"
		<< "\t\t\t\"OutgoingEdgeIds\": [";
	{
		size_t SizeOfEdgeVector{ OutgoingEdgeIds.size() };
		for (size_t i = 0; i < SizeOfEdgeVector; i++)
		{
			JsonNodeObject
				<< "\"" << JsonFileManager::ParserForJson(OutgoingEdgeIds[i]) << "\"";
			if (i != SizeOfEdgeVector - size_t(1))
			{
				JsonNodeObject << ", ";
			}
		}
	}
	JsonNodeObject << "]\n\t\t}";

	return JsonNodeObject.str();
}

std::string DialogueEdge::StringifyForJson() const
{
	std::ostringstream JsonEdgeObject;
	JsonEdgeObject
		<< "\t\t{\n\t\t\t\"EdgeId\": \"" << JsonFileManager::ParserForJson(EdgeId) << "\","
		<< "\n\t\t\t\"PlayerDialogue\": \"" << JsonFileManager::ParserForJson(PlayerDialogue) << "\","
		<< "\n\t\t\t\"NextNodeId\": \"" << JsonFileManager::ParserForJson(NextNodeId) << "\""
		<< "\n\t\t}";

	return JsonEdgeObject.str();
}

void DialogueTree::PopulateMaps()
{
	auto PopulateLamda = []<typename T>(const std::vector<T>& V, std::unordered_map<std::string, T>& M)
	{
		M.clear();
		for (const T& Member : V)
		{
			M.try_emplace(Member.GetId(), Member);
		}
	};

	PopulateLamda(DialogueEdgeContainer, EdgeMap);

	PopulateLamda(DialogueNodeContainer, NodeMap);
}
