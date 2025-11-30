#pragma once

#ifndef DIALOGUECONTENT_H
#define DIALOGUECONTENT_H

#include "Core.h"

struct DialogueParent
{
	virtual std::string StringifyForJson() const = 0;
	virtual std::string GetId() const = 0;
};

struct DialogueNode : public DialogueParent
{
	std::string NodeId;
	std::string NPCDialogue;
	std::vector<std::string> OutgoingEdgeIds;
	bool bIsConvinced{ false };
	bool bIsAngered{ false };
	bool bIsAntagonistic{ false };
	bool bIsVillagerKilled{ false };

	DialogueNode(const std::string& newNPCDialogue, const std::vector<std::string>& newOutgoingEdgeIds, const std::string& newNodeId = "START") :
		NodeId(newNodeId),
		NPCDialogue(newNPCDialogue),
		OutgoingEdgeIds(newOutgoingEdgeIds)
	{
	};

	virtual std::string StringifyForJson() const override;

	virtual std::string GetId() const override { return NodeId; };
};

struct DialogueEdge : public DialogueParent
{
	std::string EdgeId;
	std::string PlayerDialogue;
	std::string NextNodeId;
	bool bIsConvinced{ false };
	bool bIsAngered{ false };
	bool bIsAntagonistic{ false };
	bool bIsVillagerKilled{ false };

	DialogueEdge(const std::string& newEdgeId = "END", const std::string& newPlayerDialogue = "*end*", const std::string& newNextNodeId = "") :
		EdgeId(newEdgeId),
		PlayerDialogue(newPlayerDialogue),
		NextNodeId(newNextNodeId)
	{
	};

	virtual std::string StringifyForJson() const override;

	virtual std::string GetId() const override { return EdgeId; };
};

struct DialogueTree
{
	inline static std::string NPCId{ "Tommy Wiseau" };

	inline static std::vector<DialogueEdge> DialogueEdgeContainer;
	inline static std::vector<DialogueNode> DialogueNodeContainer;

	inline static std::unordered_map<std::string, DialogueEdge> EdgeMap;
	inline static std::unordered_map<std::string, DialogueNode> NodeMap;

	static void PopulateMaps();
};

#endif // !DIALOGUECONTENT_H