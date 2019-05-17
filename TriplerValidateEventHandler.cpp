#include "TriplerValidateEventHandler.h"

TriplerValidateEventHandler::TriplerValidateEventHandler()
{
}


TriplerValidateEventHandler::~TriplerValidateEventHandler()
{
}

void TriplerValidateEventHandler::notify(const Ptr<ValidateInputsEventArgs>& eventArgs)
{
	auto inputs = eventArgs->inputs();
	Ptr<SelectionCommandInput> rectSel = inputs->itemById("selectionRect");

	if (rectSel)
	{
		bool result = false;
		Ptr<Profile> profile = rectSel->selection(0)->entity();
		for (Ptr<ProfileLoop> pl : profile->profileLoops()) 
		{
			if (pl->isOuter() && pl->profileCurves()->count() == 4)
			{
				result = true;
				for (Ptr<ProfileCurve> pc : pl->profileCurves()) 
				{
					if (pc->geometryType() != Line3DCurveType) {
						result = false;
						break;
					}
				}
			}
		}
		eventArgs->areInputsValid(result);
	}
	else
	{
		eventArgs->areInputsValid(false);
	}
}