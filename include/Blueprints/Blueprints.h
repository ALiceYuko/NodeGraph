#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <imgui_node_editor.h>
#include <ax/Math2D.h>
#include <ax/Builders.h>
#include <ax/Widgets.h>
#include <functional>
#include "BlueprintWidget.h"

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;


typedef struct tagBPFunTexturePlat
{
	std::function<ImTextureID(const char* path)> funLoadFromFile;
	std::function<void(ImTextureID texture)> funDestroy;
	std::function<int(ImTextureID texture)> funGetHeight;
	std::function<int(ImTextureID texture)> funGetWidth;
}BPFunTexturePlat;

class NENodeMgr;

/*
This is View,Only Draw UI 
*/
class Blueprint
{
public:
	Blueprint();
	virtual ~Blueprint();
	
	virtual void Initialize(NENodeMgr * pNodeDataMgr,std::string strUIConfigFileName);
	virtual void UpdateFrame();
	virtual void Finalize();
	virtual bool SetBPFun(BPFunTexturePlat fnSets);

	Node* FindNode(ed::NodeId id);

private:
	
	void TouchNode(ed::NodeId id);
	float GetTouchProgress(ed::NodeId id);
	void UpdateTouch();
	
	void DrawPinIcon(const Pin& pin, bool connected, int alpha);
	void DrawNodeMain(util::BlueprintNodeBuilder & builder, Pin* newLinkPin);
	void DrawNodeType4State(Pin* newLinkPin);
	void DrawLinks();
	void PopupMenuOnNodeElem(ed::NodeId & nodeID, ed::LinkId & linkID, ed::PinId & pinID, Pin * & newNodeLinkPin);
	void ShowSpawnMenu(Pin * & newNodeLinkPin, bool & IsCreatedNewNode, ImVec2 openPopupPosition);
	void ShowNodeMenu(ed::NodeId nodeID);
	void ShowPinMenu(ed::PinId pinID);
	void ShowLinkMenu(ed::LinkId linkID);
	void MakeMenuOnNodeElem(ed::NodeId nodeID, ed::LinkId linkID, ed::PinId pinID,
		Pin * & newNodeLinkPin, bool & IsCreatedNewNode, ImVec2 popPos);
	void LinkOperate(Pin * & newNodeLinkPin, Pin * & newLinkPin, bool & IsCreateNewNode);
	void DeleteLinkAndNode();

	void ShowLeftPane(float paneWidth);
	void ShowInspector();
private:
	ax::NodeEditor::EditorContext* m_Editor = nullptr;
	const int            s_PinIconSize = 24;
	ImTextureID          s_HeaderBackground = nullptr;
	ImTextureID          s_SaveIcon = nullptr;
	ImTextureID          s_RestoreIcon = nullptr;
	const float          s_TouchTime = 1.0f;

	ed::NodeId m_contextNodeId = 0;
	ed::LinkId m_contextLinkId = 0;
	ed::PinId  m_contextPinId = 0;
	bool m_IsCreateNewNode = false;
	Pin* m_newNodeLinkPin = nullptr;
	Pin* m_newLinkPin = nullptr;

	std::map<ed::NodeId, float, NodeIdLess> s_NodeTouchTime;

	BPFunTexturePlat m_stTexFunSets;
	NENodeMgr * m_pNodeDataMgr;
	std::string m_strUIFileName;
};