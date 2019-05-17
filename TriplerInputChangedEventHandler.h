#pragma once

#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>

using namespace adsk::core;
using namespace adsk::fusion;

class TriplerInputChangedEventHandler :
	public adsk::core::InputChangedEventHandler
{
public:
	TriplerInputChangedEventHandler();
	~TriplerInputChangedEventHandler();
	void notify(const Ptr<InputChangedEventArgs>& eventArgs) override;
};

