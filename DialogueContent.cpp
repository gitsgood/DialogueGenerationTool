#include "DialogueContent.h"

std::string DialogueNode::StringifyForJson() const
{
	std::ostringstream JsonNodeObject;
	JsonNodeObject
		<< "\t\t{\n\t\t\t\"NodeId\": \"" << ParserForJson(NodeId) << "\",\n"
		<< "\t\t\t\"NPCDialogue\": \"" << ParserForJson(NPCDialogue) << "\",\n"
		<< "\t\t\t\"OutgoingEdgeIds\": [";
	{
		size_t SizeOfEdgeVector{ OutgoingEdgeIds.size() };
		for (size_t i = 0; i < SizeOfEdgeVector; i++)
		{
			JsonNodeObject
				<< "\"" << ParserForJson(OutgoingEdgeIds[i]) << "\"";
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
		<< "\t\t{\n\t\t\t\"EdgeId\": \"" << ParserForJson(EdgeId) << "\","
		<< "\n\t\t\t\"PlayerDialogue\": \"" << ParserForJson(PlayerDialogue) << "\","
		<< "\n\t\t\t\"NextNodeId\": \"" << ParserForJson(NextNodeId) << "\""
		<< "\n\t\t}";

	return JsonEdgeObject.str();
}

std::string DialogueParent::ParserForJson(const std::string& inStringToParse) const
{
	std::string ParsedString;

	for (const char& VectorLetter : inStringToParse)
	{
		switch (VectorLetter)
		{
		case '"':
		case '\\':
			ParsedString += '\\';
			ParsedString += VectorLetter;
			break;
		default:
			ParsedString += VectorLetter;
			break;
		}
	}

	return ParsedString;
}
