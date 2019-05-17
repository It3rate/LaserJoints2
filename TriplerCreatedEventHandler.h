#pragma once

#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include "TriplerValidateEventHandler.h"
#include "TriplerPreviewEventHandler.h"
#include "TriplerExecuteEventHandler.h"
#include "TriplerDestroyEventHandler.h"
#include "TriplerInputChangedEventHandler.h"

using namespace adsk::core;
using namespace adsk::fusion;

class TriplerCreatedEventHandler :
	public adsk::core::CommandCreatedEventHandler
{
public:
	TriplerCreatedEventHandler();
	~TriplerCreatedEventHandler();

	void notify(const Ptr<CommandCreatedEventArgs>& eventArgs) override;
private:
	TriplerValidateEventHandler onValidateHandler;
	TriplerPreviewEventHandler onPreviewHandler;
	TriplerExecuteEventHandler onExecuteHandler;
	TriplerDestroyEventHandler onDestroyHandler;
	TriplerInputChangedEventHandler onInputChangedHandler;

	void createUI(Ptr<CommandInputs> inputs);
};

