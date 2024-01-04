#pragma once
#include "pugixml.hpp"
#include <memory>
typedef int StateID;
typedef int TransitionID;

enum class EAnimParamType
{
	INT,
	BOOL,
	FLOAT
};

enum class EAnimCompareType
{
	Greater,
	Less,
	Equal,
	NotEqual
};

struct AnimParam
{
	AnimParam() = default;
	AnimParam(std::string str)
	{
		paramName = str;
	}

	EAnimParamType paramType = EAnimParamType::BOOL;
	std::string paramName;
	float valFloat;
	bool valBool;
	int valInt;
};

struct AnimCondition
{
	AnimCondition() = default;

	std::string animParameterName;
	EAnimCompareType animComparator;
	float value;
};

/* In the Graph, multi-edge, self-loop are all allowed. */
class Transition
{
public:
	StateID fromID, toID;

	std::shared_ptr<AnimCondition> animCondition;

//	Condition[] conditions;
	bool solo, mute, hasExit;

	Transition( const StateID& from, const StateID& to, bool solo = false, bool mute = false, bool has_exit = false)
		: fromID(from),
		  toID(to),
		  solo(solo),
		  mute(mute),
		  hasExit(has_exit)
	{
		animCondition = std::make_shared<AnimCondition>();
	}

	void gen_xml_node(pugi::xml_node root, TransitionID id)
	{
		auto _trans = root.append_child("transition");

		_trans.append_attribute("id") = id;

		_trans.append_attribute("fromID") = fromID;
		_trans.append_attribute("toID") = toID;

		_trans.append_attribute("solo") = solo;
		_trans.append_attribute("mute") = mute;
		_trans.append_attribute("hasExit") = hasExit;

	}
};
