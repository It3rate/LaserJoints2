#include "triplerCreatedEventHandler.h"

TriplerCreatedEventHandler::TriplerCreatedEventHandler()
{
}
TriplerCreatedEventHandler::~TriplerCreatedEventHandler()
{
}

void TriplerCreatedEventHandler::notify(const Ptr<CommandCreatedEventArgs>& eventArgs)
{
	if (eventArgs)
	{
		Ptr<UserInterface> ui = Application::get()->userInterface();
		if (!ui)
			return;
		Ptr<Command> command = eventArgs->command();

		bool isOk;

		Ptr<ValidateInputsEvent> onValidate = command->validateInputs();
		if (!onValidate)
			return;
		isOk = onValidate->add(&onValidateHandler);
		if (!isOk)
			return;

		Ptr<CommandEvent> onPreview = command->executePreview();
		if (!onPreview)
			return;
		isOk = onPreview->add(&onPreviewHandler);
		if (!isOk)
			return;

		Ptr<CommandEvent> onExecute = command->execute();
		if (!onExecute)
			return;
		isOk = onExecute->add(&onExecuteHandler);
		if (!isOk)
			return;

		Ptr<CommandEvent> onDestroy = command->destroy();
		if (!onDestroy)
			return;
		isOk = onDestroy->add(&onDestroyHandler);
		if (!isOk)
			return;

		Ptr<InputChangedEvent> onInputChanged = command->inputChanged();
		if (!onInputChanged)
			return;
		isOk = onInputChanged->add(&onInputChangedHandler);
		if (!isOk)
			return;

		Ptr<CommandInputs> inputs = command->commandInputs();
		if (!inputs)
			return;

		createUI(inputs);
	}
}

void TriplerCreatedEventHandler::createUI(Ptr<CommandInputs> inputs)
{
	// Female
	auto selectionInputRect = inputs->addSelectionInput("selectionRect", "Rectangle Profile", "Select box outline profile.");
	selectionInputRect->addSelectionFilter("Profiles");
	//selectionInputFemale->setSelectionLimits(0, 5);

	Ptr<ButtonRowCommandInput> extrudeDirectionInput = inputs->addButtonRowCommandInput("extrudeDirection", "Extrude Direction", false);
	if (!extrudeDirectionInput)
		return;
	Ptr<ListItems> extrudeDirectionItems = extrudeDirectionInput->listItems();
	if (!extrudeDirectionItems)
		return;
	auto btnPos = extrudeDirectionItems->add("Positive", false, "Resources/Positive");
	auto btnCen = extrudeDirectionItems->add("Centered", false, "Resources/Centered");
	auto btnNeg = extrudeDirectionItems->add("Negative", false, "Resources/Negative");
	btnPos->isSelected(true);

	// auto slotSize = inputs->addIntegerSpinnerCommandInput("slotSize", "Slot Size (mm)", 1, 500, 1, 50);
}


