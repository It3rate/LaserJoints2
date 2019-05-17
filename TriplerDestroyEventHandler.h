#pragma once

#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>

using namespace adsk::core;
using namespace adsk::fusion;

class TriplerDestroyEventHandler :
	public adsk::core::CommandEventHandler
{
public:
	TriplerDestroyEventHandler();
	~TriplerDestroyEventHandler();
	void notify(const Ptr<CommandEventArgs>& eventArgs) override;
};

