#ifndef __NODEMGR_DEF_H__
#define __NODEMGR_DEF_H__

#include "nodesdef.h"
#include <memory>
#include "Blueprints/BlueprintWidget.h"

using namespace node_edit;
/*
This is ViewModel,Save The UI Data And Link With Underlying Data 
*/
class NENodeMgr
{
public:
	NENodeMgr();
	~NENodeMgr();

	//Data->UI
	bool AddGraph(std::weak_ptr<NEGraphBase> & pBase);
	void DelGraph(uint32_t graphID, NEGraphType graphType);
	std::weak_ptr<NEGraphBase> GetGraph(uint32_t graphID, NEGraphType graphType);

	void ShowNodeInspector(ed::NodeId id);
	void ShowLinkInspector(ed::LinkId id);

	bool IsNodeActive(ed::NodeId id);
	Node* FindNode(ed::NodeId id);
	Link* FindLink(ed::LinkId id);
	Pin* FindPin(ed::PinId id);

	int DelLinkBefore(ed::LinkId id);
	int DelNodeBefore(ed::NodeId id);

	void DelLink(ed::LinkId id);
	void DelNode(ed::NodeId id);
	void MapNode(std::function<void(Node&)> funMap);
	void MapLink(std::function<void(Link&)> funMap);
	void CleanExpired();

	bool IsPinLinked(ed::PinId id);
	bool CanCreateLink(Pin* a, Pin* b);
	void BuildNode(Node* node);
	void BuildDataNode(Node* node);
	void BuildNodes();

	//May Load By Config Is OK
	Node* SpawnInputActionNode();
	Node* SpawnBranchNode();
	Node* SpawnOutputActionNode();

	//UI->Data
	Node* SpawnStateNodeMakeDefData();
	//Data->UI
	Node* SpawnStateNodeHadData(std::shared_ptr<NEGraphBaseNode> dataNode);
	
	//UI->Data
	Link * SpawnLinkMakeDefData(ed::PinId startPinID, ed::PinId endPinID, ImColor color);
	//Data->UI
	Link * SpawnLinkHadData(ed::PinId startPinID, ed::PinId endPinID, ImColor color);

	void SetGraphFactory(NECBSpawnFactory funCBFactory, void * userData);
private:

	bool AddGraphNode(std::shared_ptr<NEGraphBaseNode> dataNode);
	bool AddGraphLink(std::shared_ptr<NEGraphBaseLink> dataLink);

	int GetNextId();
	ed::LinkId GetNextLinkId();

	bool HasAddedIn(uint32_t graphID,NEGraphType graphType);

	Pin * GetPinByPinData(std::shared_ptr<NEGraphBasePin> dataPin);

	Pin * FindPinInNode(Node & node, uint32_t graphID);

	bool HasSetFactory();

private:
	std::vector<Node>    m_Nodes;
	std::vector<Link>    m_Links;

	int m_NextId = 1;

	NECBSpawnFactory m_funFactory;
	void * m_userData;
};


#endif