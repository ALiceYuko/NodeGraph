#include "nodemgr.h"

using namespace node_edit;

NENodeMgr::NENodeMgr():m_funFactory(nullptr),m_userData(nullptr)
{

}


NENodeMgr::~NENodeMgr()
{
	
}

int NENodeMgr::GetNextId() {
	return m_NextId++;
}

ed::LinkId NENodeMgr::GetNextLinkId()
{
	return ed::LinkId(GetNextId());
}

Node* NENodeMgr::FindNode(ed::NodeId id)
{
	for (auto& node : m_Nodes)
		if (node.ID == id)
			return &node;

	return nullptr;
}

Link* NENodeMgr::FindLink(ed::LinkId id)
{
	for (auto& link : m_Links)
		if (link.ID == id)
			return &link;

	return nullptr;
}

Pin* NENodeMgr::FindPin(ed::PinId id)
{
	if (!id)
		return nullptr;

	for (auto& node : m_Nodes)
	{
		for (auto& pin : node.Inputs)
			if (pin.ID == id)
				return &pin;

		for (auto& pin : node.Outputs)
			if (pin.ID == id)
				return &pin;
	}

	return nullptr;
}

bool NENodeMgr::IsPinLinked(ed::PinId id)
{
	if (!id)
		return false;

	for (auto& link : m_Links)
		if (link.StartPinID == id || link.EndPinID == id)
			return true;

	return false;
}

bool NENodeMgr::IsNodeActive(ed::NodeId id)
{
	bool bActive = false;
	
	Node * pNode = FindNode(id);
	if (pNode && pNode->NodeData)
	{
		bActive = pNode->NodeData->IsDataActive();
	}

	return bActive;
}

bool NENodeMgr::CanCreateLink(Pin* a, Pin* b)
{
	if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
		return false;

	return true;
}

void NENodeMgr::BuildNode(Node* node)
{
	for (auto& input : node->Inputs)
	{
		input.Node = node;
		input.Kind = PinKind::Input;
	}

	for (auto& output : node->Outputs)
	{
		output.Node = node;
		output.Kind = PinKind::Output;
	}
}

//Need Link Pin Data When Added Node By Blueprints 
void NENodeMgr::BuildDataNode(Node* node)
{
	for (auto& input : node->Inputs)
	{
		if (input.PinData && node->NodeData)
		{
			input.PinData->SetNode(node->NodeData);
			node->NodeData->AddPin(input.PinData);
		}
	}

	for (auto& output : node->Outputs)
	{
		if (output.PinData && node->NodeData)
		{
			output.PinData->SetNode(node->NodeData);
			node->NodeData->AddPin(output.PinData);
		}
	}
}

void NENodeMgr::BuildNodes()
{
	for (auto& node : m_Nodes)
		BuildNode(&node);
}

int NENodeMgr::DelLinkBefore(ed::LinkId id)
{
	int iRet = RETCODE_DELTEBEFORE_PASS;
	auto linkItor = std::find_if(m_Links.begin(), m_Links.end(), [id](auto& link) { return link.ID == id; });
	if (linkItor != m_Links.end())
	{
		if (linkItor->LinkData)
		{
			iRet = linkItor->LinkData->OnDeleteBefore();
		}
	}

	return iRet;
}

int NENodeMgr::DelNodeBefore(ed::NodeId id)
{
	int iRet = RETCODE_DELTEBEFORE_PASS;
	auto nodeItor = std::find_if(m_Nodes.begin(), m_Nodes.end(), [id](auto& node) { return node.ID == id; });
	if (nodeItor != m_Nodes.end())
	{
		if (nodeItor->NodeData)
		{
			iRet = nodeItor->NodeData->OnDeleteBefore();
		}
	}

	return iRet;
}

void NENodeMgr::DelLink(ed::LinkId id)
{
	auto linkItor = std::find_if(m_Links.begin(), m_Links.end(), [id](auto& link) { return link.ID == id; });
	if (linkItor != m_Links.end())
	{
		if (linkItor->LinkData)
		{
			linkItor->LinkData->OnDelete();
		}
		m_Links.erase(linkItor);
	}
		
}

void NENodeMgr::DelNode(ed::NodeId id)
{
	auto nodeItor = std::find_if(m_Nodes.begin(), m_Nodes.end(), [id](auto& node) { return node.ID == id; });
	if (nodeItor != m_Nodes.end())
	{
		if (nodeItor->NodeData)
		{
			nodeItor->NodeData->OnDelete();
		}
		m_Nodes.erase(nodeItor);
	}
}


void NENodeMgr::MapNode(std::function<void(Node &)> funMap)
{
	for (auto& node : m_Nodes)
	{
		funMap(node);
	}
}

void NENodeMgr::MapLink(std::function<void(Link&)> funMap)
{
	for (auto& link : m_Links)
	{
		funMap(link);
	}
}

void NENodeMgr::CleanExpired()
{
	for (auto itor = m_Nodes.begin();itor != m_Nodes.end();)
	{
		if (itor->NodeData && itor->NodeData->IsDataExpired())
		{
			itor = m_Nodes.erase(itor);
		}
		else
		{
			itor++;
		}
	}

	for (auto itor = m_Links.begin(); itor != m_Links.end();)
	{
		if (itor->LinkData && itor->LinkData->IsDataExpired())
		{
			itor = m_Links.erase(itor);
		}
		else
		{
			itor++;
		}
	}
}

#pragma region SPAWN

Node* NENodeMgr::SpawnInputActionNode()
{
	m_Nodes.emplace_back(GetNextId(), "InputAction Fire", ImColor(255, 128, 128));
	m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinUIType::Delegate);
	m_Nodes.back().Outputs.emplace_back(GetNextId(), "Pressed", PinUIType::Flow);
	m_Nodes.back().Outputs.emplace_back(GetNextId(), "Released", PinUIType::Flow);

	BuildNode(&m_Nodes.back());

	return &m_Nodes.back();
}

Node* NENodeMgr::SpawnBranchNode()
{
	m_Nodes.emplace_back(GetNextId(), "Branch");
	m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinUIType::Flow);
	m_Nodes.back().Inputs.emplace_back(GetNextId(), "Condition", PinUIType::Bool);
	m_Nodes.back().Outputs.emplace_back(GetNextId(), "True", PinUIType::Flow);
	m_Nodes.back().Outputs.emplace_back(GetNextId(), "False", PinUIType::Flow);

	BuildNode(&m_Nodes.back());

	return &m_Nodes.back();
}

Node* NENodeMgr::SpawnOutputActionNode()
{
	m_Nodes.emplace_back(GetNextId(), "OutputAction");
	m_Nodes.back().Inputs.emplace_back(GetNextId(), "Sample", PinUIType::Float);
	m_Nodes.back().Outputs.emplace_back(GetNextId(), "Condition", PinUIType::Bool);
	m_Nodes.back().Inputs.emplace_back(GetNextId(), "Event", PinUIType::Delegate);

	BuildNode(&m_Nodes.back());

	return &m_Nodes.back();
}

static std::string getDefName(const char * PreName, int nID)
{
	std::string strName = PreName;
	char szBuf[64] = { 0 };
	strName += itoa(nID, szBuf, 10);

	return strName;
}


/*
make UI node with Data
*/
Node* NENodeMgr::SpawnStateNodeHadData(std::shared_ptr<NEGraphBaseNode> dataNode)
{
	if (!dataNode)
	{
		return nullptr;
	}

	int nID = GetNextId();
	m_Nodes.emplace_back(nID, dataNode->GetName().c_str());
	m_Nodes.back().Type = NodeUIType::State;

	auto funAddPin = [&](std::weak_ptr<NEGraphBasePin> pin) {

		if (pin.expired())
		{
			return;
		}

		auto pinShare = pin.lock();
		NE_CHECK_NOVALID_POINT_RETURN(pinShare); 

		if (pinShare->GetPinType() == NEPinKind::NE_PIN_KIND_INPUT)
		{
			m_Nodes.back().Inputs.emplace_back(GetNextId(), pinShare->GetName().c_str(), pinShare->GetPinUIType()); //PinUIType::Flow
			m_Nodes.back().Inputs.back().PinData = pinShare;
		}
		else
		{
			m_Nodes.back().Outputs.emplace_back(GetNextId(), pinShare->GetName().c_str(), pinShare->GetPinUIType());
			m_Nodes.back().Outputs.back().PinData = pinShare;
		}
	};

	dataNode->MapPin(funAddPin);

	BuildNode(&m_Nodes.back());

	//ed::SetNodePosition(m_Nodes.back().ID, ImVec2(dataNode->GetPos().x, dataNode->GetPos().y));
	m_Nodes.back().NodeData = dataNode;

	return &m_Nodes.back();
}

bool NENodeMgr::HasSetFactory()
{
	return m_funFactory && m_userData;
}


/*
make ui node and create def Data
*/
Node* NENodeMgr::SpawnStateNodeMakeDefData()
{
	NE_CHECK_FALSE_RETURN_VALUE(HasSetFactory(), nullptr);

	bool bSetOK = false;
	int nID = GetNextId();
	std::string strName = getDefName("State", nID);
	m_Nodes.emplace_back(nID, strName.c_str());

	do
	{
		m_Nodes.back().Type = NodeUIType::State;
		m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinUIType::Flow);
		m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinUIType::Flow);

		FactoryGraphParam facParamPinIn;
		facParamPinIn.graphParam.PinParam.pinKind = PinKind::Input;
		facParamPinIn.graphParam.PinParam.pinType = PinUIType::Flow;
		strcpy_s(facParamPinIn.szDefUIName, "NOSET");
		facParamPinIn.graphType = NEGraphType::NE_GRAPH_TYPE_PIN;
		m_Nodes.back().Inputs.back().PinData = std::static_pointer_cast<NEGraphBasePin>(m_funFactory(facParamPinIn, m_userData));
		if (!m_Nodes.back().Inputs.back().PinData)
		{
			NELOGE("Spawn Pin Data Failed!!!!!!!!!!!");
			break;
		}

		FactoryGraphParam facParamPinOut;
		facParamPinOut.graphParam.PinParam.pinKind = PinKind::Output;
		facParamPinOut.graphParam.PinParam.pinType = PinUIType::Flow;
		strcpy_s(facParamPinOut.szDefUIName, "NOSET");
		facParamPinOut.graphType = NEGraphType::NE_GRAPH_TYPE_PIN;
		m_Nodes.back().Outputs.back().PinData = std::static_pointer_cast<NEGraphBasePin>(m_funFactory(facParamPinOut, m_userData));
		if (!m_Nodes.back().Outputs.back().PinData)
		{
			NELOGE("Spawn Pin Data Failed!!!!!!!!!!!");
			break;
		}

		FactoryGraphParam facParam;
		facParam.graphParam.NodeParam.uiType = NodeUIType::State;
		strcpy_s(facParam.szDefUIName, strName.c_str());
		facParam.graphType = NEGraphType::NE_GRAPH_TYPE_NODE;
		m_Nodes.back().NodeData = std::static_pointer_cast<NEGraphBaseNode>(m_funFactory(facParam, m_userData));

		if (!m_Nodes.back().NodeData)
		{
			NELOGE("Spawn Node Data Failed!!!!!!!!!!!");
			break;
		}

		bSetOK = true;
	} while (0);

	
	if (bSetOK)
	{
		BuildDataNode(&m_Nodes.back());
		BuildNode(&m_Nodes.back());

		m_Nodes.back().NodeData->OnCreate();
		m_Nodes.back().Inputs.back().PinData->OnCreate();
		m_Nodes.back().Outputs.back().PinData->OnCreate();

		return &m_Nodes.back();
	}
	else
	{
		m_Nodes.pop_back();
		return nullptr;
	}
}


Link * NENodeMgr::SpawnLinkHadData(ed::PinId startPinID, ed::PinId endPinID, ImColor color)
{
	m_Links.emplace_back(Link(GetNextId(), startPinID, endPinID));
	m_Links.back().Color = color;
	return &m_Links.back();
}


Link * NENodeMgr::SpawnLinkMakeDefData(ed::PinId startPinID, ed::PinId endPinID,ImColor color)
{
	m_Links.emplace_back(Link(GetNextId(), startPinID, endPinID));
	m_Links.back().Color = color;

	FactoryGraphParam facParam;
	facParam.graphType = NEGraphType::NE_GRAPH_TYPE_LINK;
	std::string strName = getDefName("Link", m_Links.back().ID.Get());
	strcpy_s(facParam.szDefUIName, strName.c_str());

	Pin * pinStart = FindPin(startPinID);
	Pin * pinEnd = FindPin(endPinID);
	bool bHasPinComplete = pinStart && pinEnd && pinStart->PinData && pinEnd->PinData;
	bHasPinComplete = bHasPinComplete && !pinStart->PinData->GetNode().expired() && !pinEnd->PinData->GetNode().expired();

	if (bHasPinComplete)
	{
		facParam.graphParam.LinkParam.uidStartNodeData = pinStart->PinData->GetNode().lock()->uid;
		facParam.graphParam.LinkParam.uidEndNodeData = pinEnd->PinData->GetNode().lock()->uid;

		m_Links.back().LinkData = std::static_pointer_cast<NEGraphBaseLink>(m_funFactory(facParam, m_userData));
	}

	if (!m_Links.back().LinkData || !bHasPinComplete)
	{
		NELOGE("Spawn Link Data Failed!!!!!!!!!!!");
		m_Links.pop_back();
		return nullptr;
	}
	else
	{
		
		m_Links.back().LinkData->SetStartPin(pinStart->PinData);
		m_Links.back().LinkData->SetEndPin(pinEnd->PinData);

		return &m_Links.back();
	}
}


#pragma endregion

bool NENodeMgr::AddGraphNode(std::shared_ptr<NEGraphBaseNode> dataNode)
{
	bool bRet = false;
	NE_CHECK_NOVALID_POINT_RETURN_VALUE(dataNode, false);

	NodeUIType eNodeType = dataNode->GetUIType();

	switch (eNodeType)
	{
	case NodeUIType::Blueprint:
	{
		break;
	}
	case NodeUIType::State:
	{
		Node * pUINode = SpawnStateNodeHadData(dataNode);
		if (pUINode)
		{
			bRet = true;
		}
		break;
	}
	default:
		break;
	}

	return bRet;
}

Pin * NENodeMgr::FindPinInNode(Node & node, uint32_t graphID)
{
	Pin * pinRet = nullptr;

	do 
	{
		auto itorPin = std::find_if(node.Inputs.begin(), node.Inputs.end(), [graphID](Pin & pin) {
			return pin.PinData->uid == graphID;
		});

		if (itorPin != node.Inputs.end())
		{
			pinRet = &(*itorPin);
			break;
		}

		auto itorPin2 = std::find_if(node.Outputs.begin(), node.Outputs.end(), [graphID](Pin & pin) {
			return pin.PinData->uid == graphID;
		});

		if (itorPin2 != node.Outputs.end())
		{
			pinRet = &(*itorPin2);
			break;
		}

	} while (0);
	
	return pinRet;
}


Pin * NENodeMgr::GetPinByPinData(std::shared_ptr<NEGraphBasePin> dataPin)
{
	Pin * pinRet = nullptr;

	 do 
	 {
		 if (dataPin->GetNode().expired())
		 {
			 break;
		 }

		 auto nodeShared = dataPin->GetNode().lock();
		 NE_CHECK_NOVALID_POINT_BREAK(nodeShared);

		 uint32_t graphID = nodeShared->uid;
		 auto itorNode = std::find_if(m_Nodes.begin(), m_Nodes.end(), [graphID](Node & node) {
			 return node.NodeData->uid == graphID;
		 });

		 if (itorNode == m_Nodes.end())
		 {
			 break;
		 }

		 pinRet = FindPinInNode(*itorNode, dataPin->uid);
		 
	 } while (0);

	 return pinRet;
}

bool NENodeMgr::AddGraphLink(std::shared_ptr<NEGraphBaseLink> dataLink)
{
	bool bRet = false;
	NE_CHECK_NOVALID_POINT_RETURN_VALUE(dataLink, false);

	auto pinStart = dataLink->GetStartPin();
	auto pinEnd = dataLink->GetEndPin();

	do 
	{
		if (pinStart.expired() || pinEnd.expired())
		{
			break;
		}

		auto pinStartShare = pinStart.lock();
		auto pinEndShare = pinEnd.lock();

		if (!pinStartShare || !pinStartShare)
		{
			break;
		}

		Pin * pinUIStart = GetPinByPinData(pinStartShare);
		NE_CHECK_NOVALID_POINT_BREAK(pinUIStart);

		Pin * pinUIEnd = GetPinByPinData(pinEndShare);
		NE_CHECK_NOVALID_POINT_BREAK(pinUIEnd);


		Link * pAddedLink = SpawnLinkHadData(pinUIStart->ID, pinUIEnd->ID, GetIconColor(pinUIStart->Type));
		if (pAddedLink)
		{
			pAddedLink->LinkData = dataLink;
			bRet = true;
		}

	} while (0);

	

	return bRet;
}

bool NENodeMgr::AddGraph(std::weak_ptr<NEGraphBase> & pBase)
{
	bool bRet = false;
	NE_CHECK_FALSE_RETURN_VALUE(!pBase.expired(), false);

	std::shared_ptr<NEGraphBase> baseGraph = pBase.lock();
	NE_CHECK_NOVALID_POINT_RETURN_VALUE(baseGraph, false);

	bool bAddOK = !HasAddedIn(baseGraph->uid, NEGraphType::NE_GRAPH_TYPE_NODE);
	if (bAddOK)
	{
		NEGraphType gtype = baseGraph->GetGraphType();
		switch (gtype)
		{
		case NEGraphType::NE_GRAPH_TYPE_NODE:
		{
			bAddOK = AddGraphNode(std::static_pointer_cast<NEGraphBaseNode>(baseGraph));
			break;
		}
		case NEGraphType::NE_GRAPH_TYPE_LINK:
		{
			bAddOK = AddGraphLink(std::static_pointer_cast<NEGraphBaseLink>(baseGraph));
			break;
		}
		default:
			bAddOK = false;
			break;
		}
	}

	return bAddOK;
}

void NENodeMgr::DelGraph(uint32_t graphID, NEGraphType graphType)
{
	switch (graphType)
	{
	case node_edit::NEGraphType::NE_GRAPH_TYPE_NODE:
	{
		auto itorNode = std::find_if(m_Nodes.begin(), m_Nodes.end(), [graphID](Node & node) {
			return node.NodeData->uid == graphID;
		});

		if (itorNode != m_Nodes.end())
		{
			m_Nodes.erase(itorNode);
		}
	}
		break;
	case node_edit::NEGraphType::NE_GRAPH_TYPE_LINK:
	{
		auto itorLink = std::find_if(m_Links.begin(), m_Links.end(), [graphID](Link & node) {
			return node.LinkData->uid == graphID;
		});

		if (itorLink != m_Links.end())
		{
			m_Links.erase(itorLink);
		}
	}
		break;
	default:
		break;
	}
}

std::weak_ptr<NEGraphBase> NENodeMgr::GetGraph(uint32_t graphID, NEGraphType graphType)
{
	std::weak_ptr<NEGraphBase> weakGraph;

	switch (graphType)
	{
	case node_edit::NEGraphType::NE_GRAPH_TYPE_NODE:
	{
		auto itorNode = std::find_if(m_Nodes.begin(), m_Nodes.end(), [graphID](Node & node) {
			return node.NodeData->uid == graphID;
		});

		if (itorNode != m_Nodes.end())
		{
			weakGraph = std::static_pointer_cast<NEGraphBase>(itorNode->NodeData);
		}
	}
	break;
	case node_edit::NEGraphType::NE_GRAPH_TYPE_LINK:
	{
		auto itorLink = std::find_if(m_Links.begin(), m_Links.end(), [graphID](Link & node) {
			return node.LinkData->uid == graphID;
		});

		if (itorLink != m_Links.end())
		{
			weakGraph = std::static_pointer_cast<NEGraphBase>(itorLink->LinkData);
		}
	}
	break;
	default:
		break;
	}

	return weakGraph;
}

void NENodeMgr::SetGraphFactory(NECBSpawnFactory funCBFactory, void * userData)
{
	m_funFactory = funCBFactory;
	m_userData = userData;
}

bool NENodeMgr::HasAddedIn(uint32_t graphID, NEGraphType graphType)
{
	bool bRet = false;

	switch (graphType)
	{
	case node_edit::NEGraphType::NE_GRAPH_TYPE_NODE:
	{
		auto itorNode = std::find_if(m_Nodes.begin(), m_Nodes.end(), [graphID](Node & node) {
			return node.NodeData->uid == graphID;
		});

		if (itorNode != m_Nodes.end())
		{
			bRet = true;
		}
	}
	break;
	case node_edit::NEGraphType::NE_GRAPH_TYPE_LINK:
	{
		auto itorLink = std::find_if(m_Links.begin(), m_Links.end(), [graphID](Link & node) {
			return node.LinkData->uid == graphID;
		});

		if (itorLink != m_Links.end())
		{
			bRet = true;
		}
	}
	break;
	default:
		break;
	}

	return bRet;
}
 
void NENodeMgr::ShowNodeInspector(ed::NodeId id)
{
	Node * pNode = FindNode(id);
	if (pNode && pNode->NodeData)
	{
		pNode->NodeData->OnInspectorGUI();
	}
}


void NENodeMgr::ShowLinkInspector(ed::LinkId id)
{
	Link * pLink = FindLink(id);
	if (pLink && pLink->LinkData)
	{
		pLink->LinkData->OnInspectorGUI();
	}
}
