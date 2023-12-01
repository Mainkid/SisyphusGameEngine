#pragma once
#include <stack>

enum EActionType
{
	NONE,
	ASSET_CREATED,
	FOLDER_CREATED,
	FOLDER_RENAMED,

};

class Action
{
	
};

class ComponentAction : Action
{
	
};

class HistoryManager
{
public:
	void Undo()
	{
		
	}

	void Redo()
	{
		
	}



private:
	std::stack<Action> _actionHistory;

};

