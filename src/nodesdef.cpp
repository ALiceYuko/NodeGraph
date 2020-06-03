#include "nodesdef.h"
#include "imgui.h"

namespace node_edit
{
	NEGraphBase::NEGraphBase():m_eGraphType(NEGraphType::NE_GRAPH_TYPE_NONE),m_strName("NOSET")
	{
	}

	NEGraphBase::~NEGraphBase()
	{
	}

	void NEGraphBase::OnInspectorGUI()
	{
		ImGui::Text(GetName().c_str());
	}

	void NEGraphBase::OnLeftCliked()
	{
		
	}

	void NEGraphBase::OnRightCliked()
	{

	}

	void NEGraphBase::OnDelete()
	{
	
	}

	void NEGraphBase::OnCreate()
	{

	}

	int  NEGraphBase::OnDeleteBefore()
	{
		return RETCODE_DELTEBEFORE_PASS;
	}

	int NEGraphBase::OnCreateBefore()
	{
		return RETCODE_DELTEBEFORE_PASS;
	}

	bool NEGraphBase::IsDataActive()
	{
		return false;
	}

	bool NEGraphBase::IsDataExpired()
	{
		return false;
	}

/////////////////////////////////////////////////////////////////////////////////
	NEGraphBasePin::NEGraphBasePin(NEPinKind type, PinUIType uitype):m_pinType(type),m_pinUIType(uitype)
	{
		m_eGraphType = NEGraphType::NE_GRAPH_TYPE_PIN;
	}

	NEGraphBasePin::~NEGraphBasePin()
	{
	}

//////////////////////////////////////////////////////////////////////////////////


	NEGraphBaseNode::NEGraphBaseNode(NodeUIType uiType)
	{
		m_eGraphType = NEGraphType::NE_GRAPH_TYPE_NODE;
		m_uiType = uiType;
		m_pos = { 0,0 };
	}

	NEGraphBaseNode::~NEGraphBaseNode()
	{
	}

	void NEGraphBaseNode::MapPin(std::function<void(std::weak_ptr<NEGraphBasePin>)> funMap)
	{
		auto MapVecPin = [&](std::vector<std::shared_ptr<NEGraphBasePin>> & vecPin) {
			for (auto& node : vecPin)
			{
				if (node)
				{
					std::weak_ptr<NEGraphBasePin> weak = node;
					funMap(weak);
				}
			}
		};

		MapVecPin(m_vecInputPin);
		MapVecPin(m_vecOutputPin);
	}

	std::weak_ptr<NEGraphBasePin> NEGraphBaseNode::FindPin(uint32_t pinDataID)
	{
		auto FindInVec = [&](std::vector<std::shared_ptr<NEGraphBasePin>> & vecPin) {
			auto itorPin = std::find_if(vecPin.begin(), vecPin.end(), [pinDataID](std::shared_ptr<NEGraphBasePin> & pin) {
				return pin->uid == pinDataID;
			});

			if (itorPin != vecPin.end())
			{
				return *itorPin;
			}
			else
			{
				return std::shared_ptr<NEGraphBasePin>(nullptr);
			}
		};

		auto ret = FindInVec(m_vecInputPin);
		if (ret == nullptr)
		{
			ret = FindInVec(m_vecOutputPin);
		}

		return ret;
	}

	bool NEGraphBaseNode::AddPin(std::weak_ptr<NEGraphBasePin> pinData)
	{
		if (pinData.expired())
		{
			return false;
		}
		std::shared_ptr<NEGraphBasePin> pinDataShare = pinData.lock();
		NE_CHECK_NOVALID_POINT_RETURN_VALUE(pinDataShare, false);

		auto pinFind = FindPin(pinDataShare->uid);
		if (pinFind.expired() || pinFind.lock() == nullptr)
		{
			if (pinDataShare->GetPinType() == NEPinKind::NE_PIN_KIND_INPUT)
			{
				m_vecInputPin.emplace_back(pinDataShare);
			}
			else
			{
				m_vecOutputPin.emplace_back(pinDataShare);
			}
			
			return true;
		}
		else
		{
			return false;
		}
	}

	void NEGraphBaseNode::DelPin(uint32_t pinDataID)
	{
		auto DelPinInVec = [&](std::vector<std::shared_ptr<NEGraphBasePin>> & vecPin) {
			auto itorPin = std::find_if(vecPin.begin(), vecPin.end(), [pinDataID](std::shared_ptr<NEGraphBasePin> & pin) {
				return pin->uid == pinDataID;
			});

			if (itorPin != vecPin.end())
			{
				vecPin.erase(itorPin);
			}
		};

		DelPinInVec(m_vecInputPin);
		DelPinInVec(m_vecOutputPin);
	}

	int NEGraphBaseNode::GetPinCount(NEPinKind type)
	{
		int iRet = 0;
		switch (type)
		{
		case node_edit::NEPinKind::NE_PIN_KIND_INPUT:
		{
			iRet = m_vecInputPin.size();
		}
			break;
		case node_edit::NEPinKind::NE_PIN_KIND_OUTPUT:
		{
			iRet = m_vecOutputPin.size();
		}
			break;
		default:
			break;
		}

		return iRet;
	}

	std::weak_ptr<NEGraphBasePin> NEGraphBaseNode::GetPinByIndex(NEPinKind type, int index)
	{
		auto GetPinInVecByIndex = [&](std::vector<std::shared_ptr<NEGraphBasePin>> & vecPin) {
			if (index >= vecPin.size())
			{
				return std::shared_ptr<NEGraphBasePin>(nullptr);
			}
			else
			{
				return vecPin[index];
			}
		};

		std::shared_ptr<NEGraphBasePin> ret = nullptr;

		switch (type)
		{
		case node_edit::NEPinKind::NE_PIN_KIND_INPUT:
		{
			ret = GetPinInVecByIndex(m_vecInputPin);
		}
		break;
		case node_edit::NEPinKind::NE_PIN_KIND_OUTPUT:
		{
			ret = GetPinInVecByIndex(m_vecOutputPin);
		}
		break;
		default:
			break;
		}

		return ret;
	}

//////////////////////////////////////////////////////////////////////////////////

	NEGraphBaseLink::NEGraphBaseLink()
	{
		m_eGraphType = NEGraphType::NE_GRAPH_TYPE_LINK;
	}

	NEGraphBaseLink::~NEGraphBaseLink()
	{
	}

}