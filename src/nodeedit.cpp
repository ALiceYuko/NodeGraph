#include "nodeedit.h"
#include "Blueprints/Blueprints.h"
#include "nodemgr.h"

namespace node_edit {

	typedef struct tagNEContext
	{
		Blueprint * pPaint;
		NENodeMgr * pMgr;
	}NEContext;

	GHandle NE_Init(const NEInitParam & param)
	{
		NEContext * pCtx = nullptr;

		do
		{
			pCtx = new NEContext;
			if (!pCtx)
			{
				break;
			}

			pCtx->pPaint = new Blueprint;
			pCtx->pMgr = new NENodeMgr;

			if (pCtx->pPaint)
			{
				pCtx->pPaint->Initialize(pCtx->pMgr, param.strUIConfigFileName);
			}
		} while (0);


		return pCtx;
	}

	void   NE_Update(GHandle handle)
	{
		NEContext * pCtx = (NEContext *)handle;
		if (pCtx && pCtx->pPaint)
		{
			pCtx->pPaint->UpdateFrame();
		}
	}


	void   NE_Destroy(GHandle handle)
	{
		NEContext * pCtx = (NEContext *)handle;
		if (pCtx && pCtx->pPaint)
		{
			pCtx->pPaint->Finalize();
			delete pCtx->pPaint;
			pCtx->pPaint = nullptr;
		}

		if (pCtx && pCtx->pMgr)
		{
			delete pCtx->pMgr;
			pCtx->pMgr = nullptr;
		}

		if (pCtx)
		{
			delete pCtx;
		}
	}

	bool   NE_ADD_Graph(GHandle handle, std::weak_ptr<NEGraphBase> pBase)
	{
		bool bRet = false;

		NEContext * pCtx = (NEContext *)handle;
		if (pCtx && pCtx->pMgr)
		{
			bRet = pCtx->pMgr->AddGraph(pBase);
		}

		return bRet;
	}

	void   NE_DELETE_Graph(GHandle handle, uint32_t graphID, NEGraphType graphType)
	{
		NEContext * pCtx = (NEContext *)handle;
		if (pCtx && pCtx->pMgr)
		{
			pCtx->pMgr->DelGraph(graphID, graphType);
		}
	}

	std::weak_ptr<node_edit::NEGraphBase> NE_GET_GRAPH(GHandle handle, uint32_t graphID, node_edit::NEGraphType graphType)
	{
		std::weak_ptr<node_edit::NEGraphBase> weakGraph;
		NEContext * pCtx = (NEContext *)handle;
		if (pCtx && pCtx->pMgr)
		{
			weakGraph = pCtx->pMgr->GetGraph(graphID, graphType);
		}

		return weakGraph;
	}

	void   NE_SET_GRAPH_FACTORY(GHandle handle, NECBSpawnFactory funCBFactory, void * userData)
	{
		NEContext * pCtx = (NEContext *)handle;
		if (pCtx && pCtx->pMgr)
		{
			pCtx->pMgr->SetGraphFactory(funCBFactory, userData);
		}	
	}
}