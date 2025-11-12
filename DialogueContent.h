#pragma once

#ifndef DIALOGUECONTENT_H
#define DIALOGUECONTENT_H

#include "Core.h"

struct DialogueParent
{
	virtual std::string StringifyForJson() const = 0;
};

struct DialogueNode : public DialogueParent
{
	std::string NodeId;
	std::string NPCDialogue;
	std::vector<std::string> OutgoingEdgeIds;

	DialogueNode(const std::string& newNPCDialogue, const std::vector<std::string>& newOutgoingEdgeIds, const std::string& newNodeId = "START") :
		NodeId(newNodeId),
		NPCDialogue(newNPCDialogue),
		OutgoingEdgeIds(newOutgoingEdgeIds)
	{
	};

	virtual std::string StringifyForJson() const override;
};

struct DialogueEdge : public DialogueParent
{
	std::string EdgeId;
	std::string PlayerDialogue;
	std::string NextNodeId;

	DialogueEdge(const std::string& newEdgeId = "END", const std::string& newPlayerDialogue = "*end*", const std::string& newNextNodeId = "") :
		EdgeId(newEdgeId),
		PlayerDialogue(newPlayerDialogue),
		NextNodeId(newNextNodeId)
	{
	};

	virtual std::string StringifyForJson() const override;
};

#endif // !DIALOGUECONTENT_H