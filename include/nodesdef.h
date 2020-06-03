#ifndef __NODES_DEF_H__
#define __NODES_DEF_H__

#include "nebase.h"

namespace node_edit {

	//Pin UI Type
	enum class PinUIType
	{
		Flow,
		Bool,
		Int,
		Float,
		String,
		Object,
		Function,
		Delegate,
	};

	enum class PinKind
	{
		Output,
		Input
	};

	enum class NodeUIType
	{
		Blueprint,
		Simple,
		State,
		Comment
	};

	typedef struct tagNEPos
	{
		float x;
		float y;

		tagNEPos()
		{
			x = 0;
			y = 0;
		}

		tagNEPos(float x,float y)
		{
			this->x = x;
			this->y = y;
		}
	}NEPOS;

	enum class NEGraphType
	{
		NE_GRAPH_TYPE_NONE,
		NE_GRAPH_TYPE_PIN,
		NE_GRAPH_TYPE_NODE,
		NE_GRAPH_TYPE_LINK
	};

	enum class NEPinKind
	{
		NE_PIN_KIND_NONE,
		NE_PIN_KIND_INPUT,
		NE_PIN_KIND_OUTPUT
	};


	#define RETCODE_DELTEBEFORE_NOPASS  (-1)
	#define RETCODE_DELTEBEFORE_PASS    (0)

	#define MAX_DEF_NAME_LEN (1024)

	typedef struct tagFactoryGraphParam
	{
		NEGraphType graphType;
		char        szDefUIName[MAX_DEF_NAME_LEN]; //Deafult UI Name

		union GraphUnion
		{
			struct tagNodeParam
			{
				NodeUIType uiType;
			}NodeParam;

			struct tagPinParam
			{
				PinUIType pinType;
				PinKind pinKind;
			}PinParam;

			struct tagLinkParam
			{
				uint32_t uidStartNodeData;
				uint32_t uidEndNodeData;
			}LinkParam;
		}graphParam;

		tagFactoryGraphParam()
		{
			graphType = NEGraphType::NE_GRAPH_TYPE_NONE;
			memset(szDefUIName, 0, MAX_DEF_NAME_LEN);
		}

	}FactoryGraphParam;

////////////////////////////////////////////////////////////////
	class NEGraphBase: public NEBase
	{
	public:
		NEGraphBase();
		virtual ~NEGraphBase();

		NEGraphType GetGraphType() { return m_eGraphType; }
		void SetName(std::string name) { m_strName = name; }
		std::string GetName() { return m_strName; }

		//FeedBack Function For UI
		virtual void OnInspectorGUI();
		virtual void OnLeftCliked();
		virtual void OnRightCliked();
		virtual int  OnDeleteBefore(); //return RETCODE_DELTEBEFORE_NOPASS if cant't be deleted
		virtual void OnDelete();
		virtual int OnCreateBefore();
		virtual void OnCreate();
		virtual bool IsDataActive();
		virtual bool IsDataExpired();
	protected:
		NEGraphType m_eGraphType;
		std::string m_strName;
	};

	class NEGraphBaseNode;
	class NEGraphBasePin;
////////////////////////////////////////////////////////////////

	class NEGraphBasePin:public NEGraphBase
	{
	public:
		NEGraphBasePin(NEPinKind type, PinUIType uitype);
		virtual ~NEGraphBasePin();

		void SetNode(std::weak_ptr<NEGraphBaseNode> node) { m_Node = node; };
		auto GetNode() { return m_Node; }
		auto GetPinType() { return m_pinType; }
		auto GetPinUIType() { return m_pinUIType; }
	private:
		std::weak_ptr<NEGraphBaseNode> m_Node;
		NEPinKind m_pinType;
		PinUIType m_pinUIType;
	};

/////////////////////////////////////////////////////////////////

	class NEGraphBaseNode:public NEGraphBase
	{
	public:
		NEGraphBaseNode(NodeUIType uiType);
		virtual ~NEGraphBaseNode();

		NEPOS GetPos() { return m_pos; };
		void  SetPos(const NEPOS & pos) { m_pos = pos; }

		NodeUIType GetUIType() { return m_uiType; }

		bool AddPin(std::weak_ptr<NEGraphBasePin> pinData);
		void DelPin(uint32_t pinDataID);
		std::weak_ptr<NEGraphBasePin> FindPin(uint32_t pinDataID);
		int GetPinCount(NEPinKind type);
		std::weak_ptr<NEGraphBasePin> GetPinByIndex(NEPinKind type, int index);

		void MapPin(std::function<void(std::weak_ptr<NEGraphBasePin>)> funMap);

	private:
		NodeUIType m_uiType;
		NEPOS  m_pos;
		std::vector<std::shared_ptr<NEGraphBasePin>> m_vecInputPin;
		std::vector<std::shared_ptr<NEGraphBasePin>> m_vecOutputPin;
	};


//////////////////////////////////////////////////////////////////

	class NEGraphBaseLink:public NEGraphBase
	{
	public:
		NEGraphBaseLink();
		virtual ~NEGraphBaseLink();

		auto GetStartPin() { return m_StartPin; }
		auto GetEndPin() { return m_EndPin; }
		void SetStartPin(std::weak_ptr<NEGraphBasePin> pin) { m_StartPin = pin; }
		void SetEndPin(std::weak_ptr<NEGraphBasePin> pin) { m_EndPin = pin; }

	private:
		
		std::weak_ptr<NEGraphBasePin> m_StartPin;
		std::weak_ptr<NEGraphBasePin> m_EndPin;
	};

	typedef std::function<std::shared_ptr<node_edit::NEGraphBase>(const FactoryGraphParam & param, void * userData)>   NECBSpawnFactory;
}

#endif