#ifndef _NODEEDIT_BLUEPRINTS_WIDGETS_H_
#define _NODEEDIT_BLUEPRINTS_WIDGETS_H_

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <imgui_node_editor.h>
#include <ax/Math2D.h>
#include <ax/Builders.h>
#include <ax/Widgets.h>
#include "nodesdef.h"

using namespace node_edit;

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

struct Node;

struct Pin
{
	ax::NodeEditor::PinId   ID;
	::Node*     Node;
	std::string Name;
	PinUIType     Type;
	PinKind     Kind;

	std::shared_ptr<NEGraphBasePin>    PinData;

	Pin(int id, const char* name, PinUIType type) :
		ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input), PinData(nullptr)
	{
	}
};

struct Node
{
	ax::NodeEditor::NodeId ID;
	std::string Name;
	std::vector<Pin> Inputs;
	std::vector<Pin> Outputs;
	ImColor Color;
	NodeUIType Type;
	ImVec2 Size;

	std::string State;
	std::string SavedState;

	std::shared_ptr<NEGraphBaseNode>    NodeData;

	Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
		ID(id), Name(name), Color(color), Type(NodeUIType::Blueprint), Size(0, 0), NodeData(nullptr)
	{
	}
};

struct Link
{
	ax::NodeEditor::LinkId ID;

	ax::NodeEditor::PinId StartPinID;
	ax::NodeEditor::PinId EndPinID;

	ImColor Color;

	std::shared_ptr<NEGraphBaseLink> LinkData;

	Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId) :
		ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255), LinkData(nullptr)
	{
	}
};

struct NodeIdLess
{
	bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
	{
		return lhs.AsPointer() < rhs.AsPointer();
	}
};

ImColor GetIconColor(PinUIType type);

#endif