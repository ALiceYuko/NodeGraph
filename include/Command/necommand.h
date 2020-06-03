#ifndef NECOMMAND_H
#define NECOMMAND_H

#include "Command/neparam.h"
#include "nesingleton.h"
#include <list>

class NENodeMgr;

namespace node_edit
{

	class NECommand
	{
	protected:
		std::shared_ptr<NEParameters> params;
		std::weak_ptr<NENodeMgr> viewModel;
	public:
		NECommand() = default;

		void set_parameters(std::weak_ptr<NEParameters> parameters) {
			if (!parameters.expired())
			{
				params = parameters.lock();
			}
		}
		void setViewModel(std::weak_ptr<NENodeMgr> vm)
		{
			viewModel = vm;
		}
		virtual void exec() = 0;
		virtual void undo() = 0;
		virtual ~NECommand() {};
	};



	class NECommandMgr:public NESingleTon<NECommandMgr>
	{
	public:
		NECommandMgr();
		~NECommandMgr();

		void push_back(std::weak_ptr<NECommand> cmd);
		void pop_back();
		bool CheckRedo();
		bool CheckUndo();
		void Redo();
		void Undo();
		void Clear();
	private:
		std::list<std::shared_ptr<NECommand>> m_List;
		int m_Index;
	};

}

#endif