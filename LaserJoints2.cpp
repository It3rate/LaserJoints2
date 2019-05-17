
#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>
#include <CAM/CAMAll.h>
#include "TriplerCreatedEventHandler.h"
#include "TriplerDestroyEventHandler.h"

using namespace adsk::core;
using namespace adsk::fusion;
using namespace adsk::cam;

Ptr<Application> app;
Ptr<UserInterface> ui;
TriplerCreatedEventHandler onCommandCreated;
const std::string commandId = "Trilayer";
const std::string workspaceToUse = "FusionSolidEnvironment";
const std::string panelToUse = "SolidMakePanel";

extern "C" XI_EXPORT bool run(const char* context)
{
	app = Application::get();
	if (!app)
		return false;

	ui = app->userInterface();
	if (!ui)
		return false;

	const std::string commandName = "Laser Joints2";
	const std::string commandDescription = "Makes three layer box based on rect.";

	Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
	if (!commandDefinitions)
		return false;

	Ptr<CommandDefinition> commandDefinition = commandDefinitions->itemById(commandId);
	if (!commandDefinition)
	{
		commandDefinition = commandDefinitions->addButtonDefinition(
			commandId,
			commandName,
			commandDescription,
			"./Resources");
	}

	if (!commandDefinition)
	{
		std::string errMessage;
		int rc = app->getLastError(&errMessage);
		ui->messageBox(errMessage);

		return false;
	}

	Ptr<CommandCreatedEvent> commandCreatedEvent = commandDefinition->commandCreated();
	if (!commandCreatedEvent)
		return false;

	commandCreatedEvent->add(&onCommandCreated);


	// Add a command button on Quick Access Toolbar
	Ptr<Toolbars> toolbars = ui->toolbars();
	Ptr<Toolbar> toolbarQAT = toolbars->itemById("QAT");
	Ptr<ToolbarControls> toolbarControlsQAT = toolbarQAT->controls();
	Ptr<ToolbarControl> toolbarControlQAT = toolbarControlsQAT->itemById(commandId);
	if (!toolbarControlQAT)
	{
		toolbarControlQAT = toolbarControlsQAT->addCommand(commandDefinition, commandId);
		toolbarControlQAT->isVisible(true);
	}

	// Add a command on create panel in modeling workspace
	Ptr<Workspaces> workspaces = ui->workspaces();
	Ptr<Workspace> modelingWorkspace = workspaces->itemById(workspaceToUse);
	Ptr<ToolbarPanels> toolbarPanels = modelingWorkspace->toolbarPanels();
	Ptr<ToolbarPanel> toolbarPanel = toolbarPanels->itemById(panelToUse);
	Ptr<ToolbarControls> toolbarControlsPanel = toolbarPanel->controls();
	Ptr<ToolbarControl> toolbarControlPanel = toolbarControlsPanel->itemById(commandId);
	if (!toolbarControlPanel)
	{
		toolbarControlPanel = toolbarControlsPanel->addCommand(commandDefinition, commandId);
		toolbarControlPanel->isVisible(true);
	}

	//ui->messageBox("start");
	//adsk::autoTerminate(false);
	return true;
}





extern "C" XI_EXPORT bool stop(const char* context)
{
	if (ui)
	{
		//ui->messageBox("end");
		Ptr<Toolbars> toolbars = ui->toolbars();
		Ptr<Toolbar> toolbarQAT = toolbars->itemById("QAT");
		Ptr<ToolbarControls> toolbarControlsQAT = toolbarQAT->controls();
		Ptr<ToolbarControl> toolbarControlQAT = toolbarControlsQAT->itemById(commandId);
		if (toolbarControlQAT)
		{
			toolbarControlQAT->deleteMe();
		}

		Ptr<ToolbarPanelList> toolbarPanels = ui->allToolbarPanels();
		Ptr<ToolbarPanel> toolbarPanel = toolbarPanels->itemById(panelToUse);
		Ptr<ToolbarControls> toolbarControlsPanel = toolbarPanel->controls();
		Ptr<ToolbarControl> toolbarControl = toolbarControlsPanel->itemById(commandId);
		if (toolbarControl)
		{
			toolbarControl->deleteMe();
		}

		Ptr<CommandDefinitions> commandDefinitions = ui->commandDefinitions();
		Ptr<CommandDefinition> commandDefinition = commandDefinitions->itemById(commandId);
		if (commandDefinition)
		{
			commandDefinition->deleteMe();
		}
		ui = nullptr;
	}

	app = nullptr;
	return true;
}

#ifdef XI_WIN

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif // XI_WIN
