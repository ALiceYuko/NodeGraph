#include "Blueprints/Blueprints.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include "nodemgr.h"

using namespace ax;
using ax::Widgets::IconType;

Blueprint::Blueprint()
{

}

Blueprint::~Blueprint()
{

}

ImColor GetIconColor(PinUIType type)
{
	switch (type)
	{
	default:
	case PinUIType::Flow:     return ImColor(255, 255, 255);
	case PinUIType::Bool:     return ImColor(220, 48, 48);
	case PinUIType::Int:      return ImColor(68, 201, 156);
	case PinUIType::Float:    return ImColor(147, 226, 74);
	case PinUIType::String:   return ImColor(124, 21, 153);
	case PinUIType::Object:   return ImColor(51, 150, 215);
	case PinUIType::Function: return ImColor(218, 0, 183);
	case PinUIType::Delegate: return ImColor(255, 48, 48);
	}
}

static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Splitter");
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}


void Blueprint::TouchNode(ed::NodeId id)
{
    s_NodeTouchTime[id] = s_TouchTime;
}

float Blueprint::GetTouchProgress(ed::NodeId id)
{
    auto it = s_NodeTouchTime.find(id);
    if (it != s_NodeTouchTime.end() && it->second > 0.0f)
        return (s_TouchTime - it->second) / s_TouchTime;
    else
        return 0.0f;
}

void Blueprint::UpdateTouch()
{
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for (auto& entry : s_NodeTouchTime)
    {
        if (entry.second > 0.0f)
            entry.second -= deltaTime;
    }
}


void Blueprint::DrawPinIcon(const Pin& pin, bool connected, int alpha)
{
    IconType iconType;
    ImColor  color = GetIconColor(pin.Type);
    color.Value.w = alpha / 255.0f;
    switch (pin.Type)
    {
        case PinUIType::Flow:     iconType = IconType::Flow;   break;
        case PinUIType::Bool:     iconType = IconType::Circle; break;
        case PinUIType::Int:      iconType = IconType::Circle; break;
        case PinUIType::Float:    iconType = IconType::Circle; break;
        case PinUIType::String:   iconType = IconType::Circle; break;
        case PinUIType::Object:   iconType = IconType::Circle; break;
        case PinUIType::Function: iconType = IconType::Circle; break;
        case PinUIType::Delegate: iconType = IconType::Square; break;
        default:
            return;
    }

    ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
};


void Blueprint::DrawNodeType4State(Pin* newLinkPin)
{
	auto funDrawNodes = [&](Node & node) ->void
	{
		if (node.Type != NodeUIType::State)
			return;

		const float rounding = 5.0f;
		const float padding = 12.0f;

		const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

		ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
		ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
		ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
		ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

		ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
		ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
		ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
		ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
		ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
		ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
		ed::PushStyleVar(ed::StyleVar_PinRadius, 5.0f);
		ed::BeginNode(node.ID);

		bool bActive = m_pNodeDataMgr->IsNodeActive(node.ID);
		ed::NodeSetActive(bActive);
		ImGui::BeginVertical(node.ID.AsPointer());
		ImGui::BeginHorizontal("inputs");
		ImGui::Spring(0, padding * 2);

		ImRect inputsRect;
		int inputAlpha = 200;
		if (!node.Inputs.empty())
		{
			auto& pin = node.Inputs[0];
			ImGui::Dummy(ImVec2(0, padding));
			ImGui::Spring(1, 0);
			inputsRect = ImGui_GetItemRect();

			ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
			ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
			ed::PushStyleVar(ed::StyleVar_PinCorners, 12);
			ed::BeginPin(pin.ID, ed::PinKind::Input);
			ed::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
			ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
			ed::EndPin();
			ed::PopStyleVar(3);

			if (newLinkPin && !m_pNodeDataMgr->CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
				inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
		}
		else
			ImGui::Dummy(ImVec2(0, padding));

		ImGui::Spring(0, padding * 2);
		ImGui::EndHorizontal();

		ImGui::BeginHorizontal("content_frame");
		ImGui::Spring(1, padding);

		ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
		ImGui::Dummy(ImVec2(160, 0));
		ImGui::Spring(1);

		node.Name = node.NodeData->GetName();
		ImGui::TextUnformatted(node.Name.c_str());
		ImGui::Spring(1);
		ImGui::EndVertical();
		auto contentRect = ImGui_GetItemRect();

		ImGui::Spring(1, padding);
		ImGui::EndHorizontal();

		ImGui::BeginHorizontal("outputs");
		ImGui::Spring(0, padding * 2);

		ImRect outputsRect;
		int outputAlpha = 200;
		if (!node.Outputs.empty())
		{
			auto& pin = node.Outputs[0];
			ImGui::Dummy(ImVec2(0, padding));
			ImGui::Spring(1, 0);
			outputsRect = ImGui_GetItemRect();

			ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
			ed::BeginPin(pin.ID, ed::PinKind::Output);
			ed::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
			ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
			ed::EndPin();
			ed::PopStyleVar();

			if (newLinkPin && !m_pNodeDataMgr->CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
				outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
		}
		else
			ImGui::Dummy(ImVec2(0, padding));

		ImGui::Spring(0, padding * 2);
		ImGui::EndHorizontal();

		ImGui::EndVertical();

		ed::EndNode();
		ed::PopStyleVar(7);
		ed::PopStyleColor(4);

		auto drawList = ed::GetNodeBackgroundDrawList(node.ID);

		drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
			IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
		//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
		drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
			IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), inputAlpha), 4.0f, 12);
		//ImGui::PopStyleVar();
		drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
			IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
		//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
		drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
			IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y), (int)(255 * pinBackground.z), outputAlpha), 4.0f, 3);
		//ImGui::PopStyleVar();
		drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
		drawList->AddRect(
			contentRect.GetTL(),
			contentRect.GetBR(),
			IM_COL32(48, 128, 255, 100), 0.0f);
		//ImGui::PopStyleVar();
	};

	//Hide Detail By MapNode
	m_pNodeDataMgr->MapNode(funDrawNodes);
}

//Draw UI like Blueprints
void Blueprint::DrawNodeMain(util::BlueprintNodeBuilder & builder, Pin* newLinkPin)
{
	auto funDrawNodes = [&](Node & node) ->void
	{
		if (node.Type != NodeUIType::Blueprint && node.Type != NodeUIType::Simple)
			return;

		const auto isSimple = node.Type == NodeUIType::Simple;

		bool hasOutputDelegates = false;
		for (auto& output : node.Outputs)
			if (output.Type == PinUIType::Delegate)
				hasOutputDelegates = true;

		builder.Begin(node.ID);
		if (!isSimple)
		{
			builder.Header(node.Color);
			ImGui::Spring(0);
			ImGui::TextUnformatted(node.Name.c_str());
			ImGui::Spring(1);
			ImGui::Dummy(ImVec2(0, 28));
			if (hasOutputDelegates)
			{
				ImGui::BeginVertical("delegates", ImVec2(0, 28));
				ImGui::Spring(1, 0);
				for (auto& output : node.Outputs)
				{
					if (output.Type != PinUIType::Delegate)
						continue;

					auto alpha = ImGui::GetStyle().Alpha;
					if (newLinkPin && !m_pNodeDataMgr->CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
						alpha = alpha * (48.0f / 255.0f);

					ed::BeginPin(output.ID, ed::PinKind::Output);
					ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
					ed::PinPivotSize(ImVec2(0, 0));
					ImGui::BeginHorizontal(output.ID.AsPointer());
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
					if (!output.Name.empty())
					{
						ImGui::TextUnformatted(output.Name.c_str());
						ImGui::Spring(0);
					}
					DrawPinIcon(output, m_pNodeDataMgr->IsPinLinked(output.ID), (int)(alpha * 255));
					ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
					ImGui::EndHorizontal();
					ImGui::PopStyleVar();
					ed::EndPin();

					//DrawItemRect(ImColor(255, 0, 0));
				}
				ImGui::Spring(1, 0);
				ImGui::EndVertical();
				ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
			}
			else
				ImGui::Spring(0);
			builder.EndHeader();
		}

		for (auto& input : node.Inputs)
		{
			auto alpha = ImGui::GetStyle().Alpha;
			if (newLinkPin && !m_pNodeDataMgr->CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
				alpha = alpha * (48.0f / 255.0f);

			builder.Input(input.ID);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			DrawPinIcon(input, m_pNodeDataMgr->IsPinLinked(input.ID), (int)(alpha * 255));
			ImGui::Spring(0);
			if (!input.Name.empty())
			{
				ImGui::TextUnformatted(input.Name.c_str());
				ImGui::Spring(0);
			}
			if (input.Type == PinUIType::Bool)
			{
				ImGui::Button("Hello");
				ImGui::Spring(0);
			}
			ImGui::PopStyleVar();
			builder.EndInput();
		}

		if (isSimple)
		{
			builder.Middle();

			ImGui::Spring(1, 0);
			ImGui::TextUnformatted(node.Name.c_str());
			ImGui::Spring(1, 0);
		}

		for (auto& output : node.Outputs)
		{
			if (!isSimple && output.Type == PinUIType::Delegate)
				continue;

			auto alpha = ImGui::GetStyle().Alpha;
			if (newLinkPin && !m_pNodeDataMgr->CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
				alpha = alpha * (48.0f / 255.0f);

			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			builder.Output(output.ID);
			if (output.Type == PinUIType::String)
			{
				static char buffer[128] = "Edit Me\nMultiline!";
				static bool wasActive = false;

				ImGui::PushItemWidth(100.0f);
				ImGui::InputText("##edit", buffer, 127);
				ImGui::PopItemWidth();
				if (ImGui::IsItemActive() && !wasActive)
				{
					ed::EnableShortcuts(false);
					wasActive = true;
				}
				else if (!ImGui::IsItemActive() && wasActive) 
				{
					ed::EnableShortcuts(true);
					wasActive = false;
				}
				ImGui::Spring(0);
			}
			if (!output.Name.empty())
			{
				ImGui::Spring(0);
				ImGui::TextUnformatted(output.Name.c_str());
			}
			ImGui::Spring(0);
			DrawPinIcon(output, m_pNodeDataMgr->IsPinLinked(output.ID), (int)(alpha * 255));
			ImGui::PopStyleVar();
			builder.EndOutput();
		}

		builder.End();
	};

	m_pNodeDataMgr->MapNode(funDrawNodes);
}

void Blueprint::DrawLinks()
{
	//draw Links
	auto funDrawLinks = [&](Link & link) ->void
	{
		ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);
	};
		
	m_pNodeDataMgr->MapLink(funDrawLinks);
}

void Blueprint::PopupMenuOnNodeElem(ed::NodeId & nodeID, ed::LinkId & linkID, ed::PinId & pinID,Pin * & newNodeLinkPin)
{
	ed::Suspend();
	if (ed::ShowNodeContextMenu(&nodeID))
		ImGui::OpenPopup("Node Context Menu");
	else if (ed::ShowPinContextMenu(&pinID))
		ImGui::OpenPopup("Pin Context Menu");
	else if (ed::ShowLinkContextMenu(&linkID))
		ImGui::OpenPopup("Link Context Menu");
	else if (ed::ShowBackgroundContextMenu())
	{
		ImGui::OpenPopup("Create New Node");
		newNodeLinkPin = nullptr;
	}
	ed::Resume();
}

void Blueprint::ShowSpawnMenu(Pin * & newNodeLinkPin, bool & IsCreatedNewNode,ImVec2 openPopupPosition)
{
	auto newNodePostion = openPopupPosition;
	//ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

	//auto drawList = ImGui::GetWindowDrawList();
	//drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

	Node* node = nullptr;
	/*if (ImGui::MenuItem("Input Action"))
		node = m_pNodeDataMgr->SpawnInputActionNode();
	if (ImGui::MenuItem("Output Action"))
		node = m_pNodeDataMgr->SpawnOutputActionNode();
	if (ImGui::MenuItem("Branch"))
		node = m_pNodeDataMgr->SpawnBranchNode();*/
	if (ImGui::MenuItem("Add New State"))
		node = m_pNodeDataMgr->SpawnStateNodeMakeDefData();

	ImGui::Separator();

	if (node)
	{
		IsCreatedNewNode = false;

		ed::SetNodePosition(node->ID, newNodePostion);

		if (auto startPin = newNodeLinkPin)
		{
			auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

			for (auto& pin : pins)
			{
				if (m_pNodeDataMgr->CanCreateLink(startPin, &pin))
				{
					auto endPin = &pin;
					if (startPin->Kind == PinKind::Input)
						std::swap(startPin, endPin);

					m_pNodeDataMgr->SpawnLinkMakeDefData(startPin->ID,endPin->ID, GetIconColor(startPin->Type));

					break;
				}
			}
		}
	}
}

void Blueprint::ShowNodeMenu(ed::NodeId nodeID)
{
	auto node = m_pNodeDataMgr->FindNode(nodeID);

	ImGui::TextUnformatted("Node Context Menu");
	ImGui::Separator();
	if (node)
	{
		ImGui::Text("ID: %p", node->ID.AsPointer());
		ImGui::Text("Type: %s", node->Type == NodeUIType::Blueprint ? "Blueprint" : (node->Type == NodeUIType::State ? "State" : "Comment"));
		ImGui::Text("Inputs: %d", (int)node->Inputs.size());
		ImGui::Text("Outputs: %d", (int)node->Outputs.size());

		/*if (node->pOperationData)
		{
			node->pOperationData->OnRightCliked();
		}*/
	}
	else
		ImGui::Text("Unknown node: %p", nodeID.AsPointer());
	ImGui::Separator();
	if (ImGui::MenuItem("Delete"))
	{
		if (m_pNodeDataMgr->DelNodeBefore(nodeID) == RETCODE_DELTEBEFORE_PASS)
		{
			ed::DeleteNode(nodeID);
		}
	}
}

void Blueprint::ShowPinMenu(ed::PinId pinID)
{
	auto pin = m_pNodeDataMgr->FindPin(pinID);

	ImGui::TextUnformatted("Pin Context Menu");
	ImGui::Separator();
	if (pin)
	{
		ImGui::Text("ID: %p", pin->ID.AsPointer());
		if (pin->Node)
			ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
		else
			ImGui::Text("Node: %s", "<none>");
	}
	else
		ImGui::Text("Unknown pin: %p", pinID.AsPointer());
}


void Blueprint::ShowLinkMenu(ed::LinkId linkID)
{
	auto link = m_pNodeDataMgr->FindLink(linkID);

	ImGui::TextUnformatted("Link Context Menu");
	ImGui::Separator();
	if (link)
	{
		ImGui::Text("ID: %p", link->ID.AsPointer());
		ImGui::Text("From: %p", link->StartPinID.AsPointer());
		ImGui::Text("To: %p", link->EndPinID.AsPointer());
	}
	else
		ImGui::Text("Unknown link: %p", linkID.AsPointer());
	ImGui::Separator();
	if (ImGui::MenuItem("Delete"))
	{
		if (m_pNodeDataMgr->DelLinkBefore(linkID) == RETCODE_DELTEBEFORE_PASS)
		{
			ed::DeleteLink(linkID);
		}
	}
}

void Blueprint::MakeMenuOnNodeElem(ed::NodeId nodeID, ed::LinkId linkID, ed::PinId pinID,
	Pin * & newNodeLinkPin,bool & IsCreatedNewNode, ImVec2 popPos)
{
	ed::Suspend();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("Node Context Menu"))
	{
		ShowNodeMenu(nodeID);
		
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Pin Context Menu"))
	{
		ShowPinMenu(pinID);

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Link Context Menu"))
	{
		ShowLinkMenu(linkID);
		
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("Create New Node"))
	{
		ShowSpawnMenu(newNodeLinkPin,IsCreatedNewNode, popPos);

		ImGui::EndPopup();
	}
	else
		IsCreatedNewNode = false;

	ImGui::PopStyleVar();
	ed::Resume();
}

void Blueprint::LinkOperate(Pin * & newNodeLinkPin,Pin * & newLinkPin, bool & IsCreateNewNode)
{
	if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		auto FunShowLabel = [](const char* label, ImColor color)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
			auto size = ImGui::CalcTextSize(label);

			auto padding = ImGui::GetStyle().FramePadding;
			auto spacing = ImGui::GetStyle().ItemSpacing;

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

			auto rectMin = ImGui::GetCursorScreenPos() - padding;
			auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
			ImGui::TextUnformatted(label);
		};

		ed::PinId startPinId = 0, endPinId = 0;
		if (ed::QueryNewLink(&startPinId, &endPinId))
		{
			auto startPin = m_pNodeDataMgr->FindPin(startPinId);
			auto endPin = m_pNodeDataMgr->FindPin(endPinId);

			newLinkPin = startPin ? startPin : endPin;

			if (startPin->Kind == PinKind::Input)
			{
				std::swap(startPin, endPin);
				std::swap(startPinId, endPinId);
			}

			if (startPin && endPin)
			{
				if (endPin == startPin)
				{
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (endPin->Kind == startPin->Kind)
				{
					FunShowLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				//else if (endPin->Node == startPin->Node)
				//{
				//    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
				//    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
				//}
				else if (endPin->Type != startPin->Type)
				{
					FunShowLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else
				{
					FunShowLabel("+ Create Link", ImColor(32, 45, 32, 180));
					if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
					{
						m_pNodeDataMgr->SpawnLinkMakeDefData(startPinId, endPinId, GetIconColor(startPin->Type));
					}
				}
			}
		}

		ed::PinId pinId = 0;
		if (ed::QueryNewNode(&pinId))
		{
			newLinkPin = m_pNodeDataMgr->FindPin(pinId);
			if (newLinkPin)
				FunShowLabel("+ Create Node", ImColor(32, 45, 32, 180));

			if (ed::AcceptNewItem())
			{
				IsCreateNewNode = true;
				newNodeLinkPin = m_pNodeDataMgr->FindPin(pinId);
				newLinkPin = nullptr;
				ed::Suspend();
				ImGui::OpenPopup("Create New Node");
				ed::Resume();
			}
		}
	}
	else
		newLinkPin = nullptr;

	ed::EndCreate();
}


void Blueprint::DeleteLinkAndNode()
{
	if (ed::BeginDelete())
	{
		ed::LinkId linkId = 0;
		while (ed::QueryDeletedLink(&linkId))
		{
			if (ed::AcceptDeletedItem())
			{
				m_pNodeDataMgr->DelLink(linkId);
			}
		}

		ed::NodeId nodeId = 0;
		while (ed::QueryDeletedNode(&nodeId))
		{
			if (ed::AcceptDeletedItem())
			{
				m_pNodeDataMgr->DelNode(nodeId);
			}
		}
	}
	ed::EndDelete();
}

bool Blueprint::SetBPFun(BPFunTexturePlat fnSets)
{
	m_stTexFunSets = fnSets;
	return (fnSets.funDestroy && fnSets.funGetHeight && fnSets.funGetWidth && fnSets.funLoadFromFile);
}

enum class TYPE_WITCH_TO_SHOW
{
	TYPE_SHOW_NONE,
	TYPE_SHOW_NODE,
	TYPE_SHOW_LINK
};

void Blueprint::ShowInspector()
{
	float paneWidth = ImGui::GetContentRegionAvailWidth();

	ImGui::GetWindowDrawList()->AddRectFilled(
		ImGui::GetCursorScreenPos(),
		ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
		ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);


	ImGui::Spacing(); 
	ImGui::SameLine();

	ImGui::TextUnformatted("Inspector");

	std::vector<ed::NodeId> selectedNodes;
	std::vector<ed::LinkId> selectedLinks;
	selectedNodes.resize(ed::GetSelectedObjectCount());
	selectedLinks.resize(ed::GetSelectedObjectCount());

	int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
	int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

	selectedNodes.resize(nodeCount);
	selectedLinks.resize(linkCount);

	auto FunGetShowWhitch = [&]() ->TYPE_WITCH_TO_SHOW {
		if (nodeCount > 1 || linkCount > 1)
		{
			return TYPE_WITCH_TO_SHOW::TYPE_SHOW_NONE;
		}

		if (nodeCount == 1)
		{
			return TYPE_WITCH_TO_SHOW::TYPE_SHOW_NODE;
		}

		if (linkCount == 1)
		{
			return TYPE_WITCH_TO_SHOW::TYPE_SHOW_LINK;
		}

		return TYPE_WITCH_TO_SHOW::TYPE_SHOW_NONE;
	};

	auto showType = FunGetShowWhitch();

	switch (showType)
	{
	case TYPE_WITCH_TO_SHOW::TYPE_SHOW_NONE:
		break;
	case TYPE_WITCH_TO_SHOW::TYPE_SHOW_NODE:
	{
		m_pNodeDataMgr->ShowNodeInspector(selectedNodes[0]);
	}
	break;
	case TYPE_WITCH_TO_SHOW::TYPE_SHOW_LINK:
	{
		m_pNodeDataMgr->ShowLinkInspector(selectedLinks[0]);
	}
	break;
	default:
		break;
	}
}

void Blueprint::ShowLeftPane(float paneWidth)
{
	auto& io = ImGui::GetIO();

	ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

	ImGui::BeginVertical("PaneList");
	
	ShowInspector();

	ImGui::EndVertical();

	ImGui::EndChild();
}


void Blueprint::UpdateFrame()
{
	if (!m_Editor)
	{
		return;
	}

	UpdateTouch();

	auto& io = ImGui::GetIO();

	//ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

	ed::SetCurrentEditor(m_Editor);

	static float leftPaneWidth = 400.0f;
	static float rightPaneWidth = 800.0f;
	Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

	if (m_pNodeDataMgr)
	{
		m_pNodeDataMgr->CleanExpired();
	}

	ShowLeftPane(leftPaneWidth - 4.0f);

	ImGui::SameLine(0.0f, 12.0f);


	ed::Begin("Node editor");
	{
		auto cursorTopLeft = ImGui::GetCursorScreenPos();

		util::BlueprintNodeBuilder builder(s_HeaderBackground, m_stTexFunSets.funGetWidth? m_stTexFunSets.funGetWidth(s_HeaderBackground):0,
			m_stTexFunSets.funGetHeight ? m_stTexFunSets.funGetHeight(s_HeaderBackground):0);

		DrawNodeType4State(m_newLinkPin);

		DrawNodeMain(builder, m_newLinkPin);

		DrawLinks();

		if (!m_IsCreateNewNode)
		{
			LinkOperate(m_newNodeLinkPin, m_newLinkPin, m_IsCreateNewNode);

			DeleteLinkAndNode();
		}

		ImGui::SetCursorScreenPos(cursorTopLeft);
	}

	auto openPopupPosition = ImGui::GetMousePos();

	PopupMenuOnNodeElem(m_contextNodeId, m_contextLinkId, m_contextPinId, m_newNodeLinkPin);

	MakeMenuOnNodeElem(m_contextNodeId, m_contextLinkId, m_contextPinId, m_newNodeLinkPin, m_IsCreateNewNode, openPopupPosition);


	ed::End();

}

#define  BP_SHOW_TEST (0)

Node* Blueprint::FindNode(ed::NodeId id)
{
	Node * pRet = NULL;
	if (m_pNodeDataMgr)
	{
		pRet = m_pNodeDataMgr->FindNode(id);
	}

	return pRet;
}

void Blueprint::Initialize(NENodeMgr * pNodeDataMgr, std::string strUIConfigFileName)
{
	ed::Config config;

	m_contextNodeId = 0;
	m_contextLinkId = 0;
	m_contextPinId = 0;
	m_IsCreateNewNode = false;
	m_newNodeLinkPin = nullptr;
	m_newLinkPin = nullptr;
	m_strUIFileName = strUIConfigFileName;

	config.UserPointer = this;
	config.SettingsFile = m_strUIFileName.c_str();

	config.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
	{
		Blueprint * pBuue = (Blueprint *)userPointer;
		auto node = pBuue->FindNode(nodeId);
		if (!node)
			return 0;

		if (data != nullptr)
			memcpy(data, node->State.data(), node->State.size());
		return node->State.size();
	};

	config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
	{
		Blueprint * pBuue = (Blueprint *)userPointer;
		auto node = pBuue->FindNode(nodeId);
		if (!node)
			return false;

		node->State.assign(data, size);

		pBuue->TouchNode(nodeId);

		return true;
	};

	m_Editor = ed::CreateEditor(&config);
	ed::SetCurrentEditor(m_Editor);
	m_pNodeDataMgr = pNodeDataMgr;

#if BP_SHOW_TEST

	Node* node;
	node = m_pNodeDataMgr->SpawnInputActionNode();      ed::SetNodePosition(node->ID, ImVec2(-252, 220));
	node = m_pNodeDataMgr->SpawnBranchNode();           ed::SetNodePosition(node->ID, ImVec2(-300, 351));
	node = m_pNodeDataMgr->SpawnOutputActionNode();     ed::SetNodePosition(node->ID, ImVec2(71, 80));


	ed::NavigateToContent();

	m_pNodeDataMgr->BuildNodes();

	//s_Links.push_back(Link(GetNextLinkId(), s_Nodes[5].Outputs[0].ID, s_Nodes[6].Inputs[0].ID));
	//s_Links.push_back(Link(GetNextLinkId(), s_Nodes[5].Outputs[0].ID, s_Nodes[7].Inputs[0].ID));

#endif

	if (m_stTexFunSets.funLoadFromFile)
	{
		s_HeaderBackground = m_stTexFunSets.funLoadFromFile("Data/BlueprintBackground.png");
		s_SaveIcon = m_stTexFunSets.funLoadFromFile("Data/ic_save_white_24dp.png");
		s_RestoreIcon = m_stTexFunSets.funLoadFromFile("Data/ic_restore_white_24dp.png");
	}

}

void Blueprint::Finalize()
{
	auto releaseTexture = [&](ImTextureID& id)
	{
		if (id)
		{
			if (m_stTexFunSets.funDestroy)
			{
				m_stTexFunSets.funDestroy(id);
			}
			id = nullptr;
		}
	};

	releaseTexture(s_RestoreIcon);
	releaseTexture(s_SaveIcon);
	releaseTexture(s_HeaderBackground);

	if (m_Editor)
	{
		ed::DestroyEditor(m_Editor);
		m_Editor = nullptr;
	}
}

