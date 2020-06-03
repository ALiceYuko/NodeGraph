#ifndef __NODE_EDIT_H__
#define __NODE_EDIT_H__

/*

NodeEdit

*/
#include "nodesdef.h"

namespace node_edit {

	typedef void* GHandle;

	typedef struct tagNEInitParam
	{
		std::string strUIConfigFileName;
	}NEInitParam;

	/*
	API
	*/

	GHandle NE_Init(const NEInitParam & param);

	void   NE_Update(GHandle handle);

	void   NE_Destroy(GHandle handle);

	bool   NE_ADD_Graph(GHandle handle, std::weak_ptr<node_edit::NEGraphBase> pBase);

	void   NE_DELETE_Graph(GHandle handle, uint32_t graphID, node_edit::NEGraphType graphType);

	std::weak_ptr<node_edit::NEGraphBase> NE_GET_GRAPH(GHandle handle, uint32_t graphID, node_edit::NEGraphType graphType);
	/*
	need user set a factory to make graph data in real type
	*/
	void   NE_SET_GRAPH_FACTORY(GHandle handle, NECBSpawnFactory funCBFactory, void * userData);

}

#endif // !__ANIM_GRAPH_H__
