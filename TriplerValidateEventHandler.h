#pragma once

#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>

using namespace adsk::core;
using namespace adsk::fusion;

class TriplerValidateEventHandler :
	public ValidateInputsEventHandler// BUG: ValidateInputsEventHandler doesn't inherit from CommandEventHandler
{
public:
	TriplerValidateEventHandler();
	~TriplerValidateEventHandler();
	void notify(const Ptr<ValidateInputsEventArgs>& eventArgs) override;
};

