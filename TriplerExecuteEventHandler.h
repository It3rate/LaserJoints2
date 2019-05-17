#pragma once

#include <Core/CoreAll.h>
#include <Fusion/FusionAll.h>

using namespace adsk::core;
using namespace adsk::fusion;

const std::string wallHeight = "wall_height";
const std::string thicknessOuter = "thickness_outer";
const std::string thicknessInner = "thickness_inner";
const std::string grip = "grip";
const std::string snap = "snap";
const std::string kerf = "kerf";

class TriplerExecuteEventHandler :
	public adsk::core::CommandEventHandler
{
public:
	//const static char* colorNames[];
	static const std::vector<std::string> colorNames;
	TriplerExecuteEventHandler();
	~TriplerExecuteEventHandler();
	void notify(const Ptr<CommandEventArgs>& eventArgs) override;

protected:

	Ptr<Application> app;
	Ptr<UserInterface> ui;
	Ptr<Product> product;
	Ptr<Design> design;
	Ptr<adsk::fusion::UserParameters> params;

	bool setLinkages();
	void ensureParams();

	std::vector<Ptr<ExtrudeFeature>> tripleProfile(Ptr<Profile> profile, int extrudeDir);

	Ptr<adsk::fusion::UserParameter> addOrGetParam(
		std::string name,
		std::string defaultExpression);

	Ptr<adsk::fusion::ExtrudeFeature> extrudeBody(
		Ptr<adsk::fusion::Occurrence> occurance,
		Ptr<adsk::fusion::Profile> profile,
		std::string start,
		std::string distance,
		int colorIndex);

	void cutNotch(
		Ptr<ExtrudeFeature> extrude,
		Ptr<Profile> perpendicular);

	Ptr<adsk::core::Appearance> addAppearance(
		Ptr<BRepBody> body,
		int colorIndex);
};

