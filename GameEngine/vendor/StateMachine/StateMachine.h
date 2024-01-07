#pragma once
#include <algorithm>
#include <list>
#include "StateMachineLayer.h"


class StateMachine
{
public:
	std::list<StateMachineLayer> layers;
	StateMachineLayer* baseLayer = nullptr;

private:
	LayerID nextLayerID = 0;

public:
	StateMachineLayer& addLayer(const char * name, bool isBaseLayer = false) {
		layers.push_back(StateMachineLayer(name, nextLayerID));

		if (isBaseLayer)
			baseLayer = &layers.back();

		++nextLayerID;
		return layers.back();
	}
};
