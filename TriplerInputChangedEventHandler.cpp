#include "TriplerInputChangedEventHandler.h"



TriplerInputChangedEventHandler::TriplerInputChangedEventHandler()
{
}


TriplerInputChangedEventHandler::~TriplerInputChangedEventHandler()
{
}

void TriplerInputChangedEventHandler::notify(const Ptr<InputChangedEventArgs>& eventArgs)
{

	Ptr<UserInterface> ui = Application::get()->userInterface();
	if (!ui)
		return;

	Ptr<CommandInputs> inputs = eventArgs->inputs();
	if (!inputs)
		return;

	Ptr<CommandInput> cmdInput = eventArgs->input();
	if (!cmdInput)
		return;

}