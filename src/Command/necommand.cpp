#include "Command/necommand.h"

using namespace node_edit;
#define MAX_STEPS   (20)

NECommandMgr::NECommandMgr():m_Index(0)
{
}

NECommandMgr::~NECommandMgr()
{
}

void NECommandMgr::push_back(std::weak_ptr<NECommand> cmd)
{
	while (m_Index != m_List.size())
	{
		m_List.pop_back();
	}

	if (m_List.size()+1 > MAX_STEPS)
	{
		m_List.pop_front();  //¶ªµôµÚÒ»¸ö
	}

	if (!cmd.expired())
	{
		m_List.push_back(cmd.lock());
		m_Index = m_List.size();
	}
}

void NECommandMgr::pop_back()
{
	m_List.pop_back();
	m_Index = m_List.size();
}

bool NECommandMgr::CheckRedo()
{
	return m_Index < m_List.size();
}


bool NECommandMgr::CheckUndo()
{
	return m_Index > 0;
}

void NECommandMgr::Redo()
{
	if (CheckRedo())
	{
		auto itor = m_List.begin();
		std::advance(itor, m_Index);
		if (itor != m_List.end())
		{
			auto cmd = *itor;
			cmd->exec();
		}

		++m_Index;
	}
}

void NECommandMgr::Undo()
{
	if (CheckUndo())
	{
		--m_Index;

		auto itor = m_List.begin();
		std::advance(itor, m_Index);
		if (itor != m_List.end())
		{
			auto cmd = *itor;
			cmd->undo();
		}
		
	}
}

void NECommandMgr::Clear()
{
	m_List.clear();
	m_Index = 0;
}