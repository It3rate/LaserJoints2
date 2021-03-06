#include "TriplerExecuteEventHandler.h"
#include <iostream>

const std::vector<std::string> TriplerExecuteEventHandler::colorNames = std::vector<std::string>({
	"Yellow", "Red", "Green", "Blue", "White", "Gray", "Black",
	"Yellow", "Red", "Green", "Blue", "White", "Gray", "Black"});

TriplerExecuteEventHandler::TriplerExecuteEventHandler()
{
}

TriplerExecuteEventHandler::~TriplerExecuteEventHandler()
{
}

bool TriplerExecuteEventHandler::setLinkages()
{
	app = Application::get();
	if (!app)
		return false;

	ui = app->userInterface();
if (!ui)
return false;

product = app->activeProduct();
if (!product)
return false;

design = product;
if (!design)
return false;

params = design->userParameters();
if (!params)
return false;

return true;
}

void TriplerExecuteEventHandler::notify(const Ptr<CommandEventArgs>& eventArgs)
{
	if (!setLinkages()) {
		return;
	}

	auto inputs = eventArgs->command()->commandInputs();
	ensureParams();

	Ptr<SelectionCommandInput> rectSel = inputs->itemById("selectionRect");
	Ptr<Profile> selProfile;
	int extrudeDir;
	if (rectSel)
	{
		selProfile = rectSel->selection(0)->entity();

		Ptr<ButtonRowCommandInput> extendDirectionInput = inputs->itemById("extrudeDirection");
		extrudeDir = extendDirectionInput->selectedItem()->index(); // 0 Pos, 1 Cent, 2 Neg
		extrudeDir = (extrudeDir - 1) * -1; // 1 Pos, 0 Cent, -1 NegectSel)

		std::vector<Ptr<ProfileCurve>> lines = {};
		int index = 0;
		for (Ptr<ProfileLoop> pl : selProfile->profileLoops())
		{
			if (pl->isOuter() && pl->profileCurves()->count() == 4)
			{
				for (Ptr<ProfileCurve> pc : pl->profileCurves())
				{
					if (pc->geometryType() == Line3DCurveType) {
						lines.push_back(pc);
					}
				}
			}
		}
		auto wallFaces = getWallFaces(selProfile);

		for (auto face : wallFaces)
		{
			auto profile = createProfileOnFace(face);
			// pass plane centriod for notches
			auto extrudes = tripleProfile(profile, extrudeDir);
		}


		//auto extrudes = tripleProfile(selProfile, extrudeDir);


	//	auto sketch0 = profile0->parentSketch();
	//	auto lines0 = sketch1->sketchCurves()->sketchLines();
	//	auto lines1 = sketch1->sketchCurves()->sketchLines();
	//	//for each (auto line in lines1)
	//	//{
	//	//	auto proj = sketch0->project(line);
	//	//}

	//	Ptr<BRepFace> face = extrudes[0]->startFaces()->item(0);
	//	std::vector<Ptr<Base>> entities;
	//	entities.push_back(extrudes[0]);
	//	std::vector<Ptr<SketchEntity>> resVec = sketch1->intersectWithSketchPlane(entities);

	//	//Ptr<SketchCurves> curves = femaleSketch->sketchCurves();
	//	//Ptr<SketchLines> lines = curves->sketchLines();
	//	//for each (auto ent in resVec)
	//	//{
	//	//	femaleSketch->
	//	//}

	//	cutNotch(extrudes[0], profile1);
	//	cutNotch(extrudes[1], profile1);
	//	cutNotch(extrudes[2], profile1);
	//}
	//if (maleSel)
	//{
	//	auto extrudes = tripleProfile(profile1, extrudeDir1);
	//	cutNotch(extrudes[0], profile0);
	//	cutNotch(extrudes[1], profile0);
	//	cutNotch(extrudes[2], profile0);
	}
}

void TriplerExecuteEventHandler::ensureParams()
{
	Ptr<adsk::fusion::UserParameter> wallHeightExpr = addOrGetParam(wallHeight, "20 mm");
	Ptr<adsk::fusion::UserParameter> outerExpr = addOrGetParam(thicknessOuter, "2 mm");
	Ptr<adsk::fusion::UserParameter> innerExpr = addOrGetParam(thicknessInner, "3 mm");
	Ptr<adsk::fusion::UserParameter> thicknessTotalExpr = addOrGetParam(thicknessTotal,
		thicknessOuter + "* 2 + " + thicknessInner);
	Ptr<adsk::fusion::UserParameter> thicknessHalfExpr = addOrGetParam(thicknessHalf,
		thicknessInner + "/ 2 + " + thicknessOuter);
	Ptr<adsk::fusion::UserParameter> gripExpr = addOrGetParam(grip, "2 mm");
	Ptr<adsk::fusion::UserParameter> snapExpr = addOrGetParam(snap, "0.5 mm");
	Ptr<adsk::fusion::UserParameter> kerfExpr = addOrGetParam(kerf, "0.1 mm");
}

std::vector<Ptr<BRepFace>> TriplerExecuteEventHandler::getWallFaces(Ptr<Profile> profile)
{
	design->activateRootComponent();
	auto rootComponent = design->activeComponent();
	//auto ident = Matrix3D::create();
	//auto baseOccurance = rootComponent->occurrences()->addNewComponent(ident);
	//baseOccurance->activate();
	auto extrudeRoot = extrudeBody(rootComponent, profile, "0", wallHeight, 6);
	std::vector<Ptr<BRepFace>> result = {};
	auto profilePlane = profile->plane();
	Ptr<BRepEdges> edges;
	Ptr<Line3D> line;
	for (auto face : extrudeRoot->faces())
	{
		edges = face->edges();
		for (Ptr<BRepEdge> edge : edges)
		{
			Ptr<Line3D> line = Line3D::create(edge->startVertex()->geometry(), edge->endVertex()->geometry());
			if (profilePlane->isPerpendicularToLine(line))
			{
				result.push_back(face);
				break;
			}
		}
	}

	int cnt = result.size();
	int colorIndex = 0;
	for (auto face : result)
	{
		addAppearance(face, colorIndex);
		colorIndex++;
	}
	extrudeRoot->bodies()->item(0)->isLightBulbOn(false);
	return result;
}

Ptr<Profile> TriplerExecuteEventHandler::createProfileOnFace(Ptr<BRepFace> face)
{
	design->activateRootComponent();
	auto rootComponent = design->rootComponent();
	auto ident = Matrix3D::create();
	auto newOccurrance = rootComponent->occurrences()->addNewComponent(ident);
	newOccurrance->activate();
	auto sketches = newOccurrance->component()->sketches();
	auto sketch = sketches->add(face);
	sketch->project(face->geometry());
	return sketch->profiles()->item(0);
}

void TriplerExecuteEventHandler::makeMidNotch(Ptr<SketchLine> line)
{
	auto lines = line->parentSketch()->sketchCurves()->sketchLines();
	auto x0 = line->startSketchPoint()->geometry()->x();
	auto y0 = line->startSketchPoint()->geometry()->y();
	auto x1 = line->endSketchPoint()->geometry()->x();
	auto y1 = line->endSketchPoint()->geometry()->y();
	double xDir = (x0 < x1) ? 1 : (x0 > x1) ? -1 : 0;
	double yDir = (y0 < y1) ? 1 : (y0 > y1) ? -1 : 0;
	double step = 0.4;
	double stepX = step * xDir;
	double stepY = step * yDir;
	double midx0 = (x1 - x0) / 2.0 + x0 - step * xDir;
	double midx1 = (x1 - x0) / 2.0 + x0 + step * xDir;
	double midy0 = (y1 - y0) / 2.0 + y0 - step * yDir;
	double midy1 = (y1 - y0) / 2.0 + y0 + step * yDir;
	line->isConstruction(true);
	lines->addByTwoPoints(Point3D::create(x0, y0, 0), Point3D::create(midx0, midy0, 0));
	lines->addByTwoPoints(Point3D::create(midx0, midy0, 0), Point3D::create(midx0 + stepY, midy0 - stepX, 0));
	lines->addByTwoPoints(Point3D::create(midx0 + stepY, midy0 - stepX, 0), Point3D::create(midx1 + stepY, midy1 - stepX, 0));
	lines->addByTwoPoints(Point3D::create(midx1 + stepY, midy1 - stepX, 0), Point3D::create(midx1, midy1, 0));
	lines->addByTwoPoints(Point3D::create(midx1, midy1, 0), Point3D::create(x1, y1, 0));
}
std::vector<Ptr<ExtrudeFeature>> TriplerExecuteEventHandler::tripleProfile(Ptr<Profile> profile, int extrudeDir)
{
	auto sketch = profile->parentSketch();

	auto lines = sketch->sketchCurves()->sketchLines();
	makeMidNotch(lines->item(0));
	makeMidNotch(lines->item(1));
	makeMidNotch(lines->item(2));
	makeMidNotch(lines->item(3));
	//sketchLines->addTwoPointRectangle(
	//	Point3D::create(min->x(), min->y(), 0),
	//	Point3D::create(min->x() - 0.5, min->y() + 0.5, 0));

	auto max = profile->boundingBox()->maxPoint();
	auto min = profile->boundingBox()->minPoint();
	auto cx = (max->x() - min->x()) / 2.0 + min->x();
	auto cy = (max->y() - min->y()) / 2.0 + min->y();
	auto cz = (max->z() - min->z()) / 2.0 + min->z();
	auto cent = Point3D::create(cx, cy, cz);
	sketch->sketchCurves()->sketchCircles()->addByCenterRadius(cent, 0.5);

	auto profileCount = sketch->profiles()->count();
	double maxArea = 0;
	for (auto p : sketch->profiles())
	{
		if (p->areaProperties()->area() > maxArea)
		{
			profile = p;
			maxArea = p->areaProperties()->area();
		}
	}

	auto component = sketch->parentComponent();
	std::string start = (extrudeDir == 0) ? "-" + thicknessHalf : "0";
	std::string dist = (extrudeDir == -1) ? "-" + thicknessOuter : thicknessOuter;
	auto extrude0 = this->extrudeBody(component, profile, start, dist, 0);

	start = (extrudeDir == -1) ? "-" + thicknessOuter :
		(extrudeDir == 0) ? "-" + thicknessInner + " / 2" : thicknessOuter;
	dist = (extrudeDir == -1) ? "-" + thicknessInner : thicknessInner;
	auto extrude1 = this->extrudeBody(component, profile, start, dist, 1);

	start = (extrudeDir == -1) ? "-" + thicknessHalf :
		(extrudeDir == 0) ? thicknessInner + " / 2" : thicknessOuter + " + " + thicknessInner;
	dist = (extrudeDir == -1) ? "-" + thicknessOuter : thicknessOuter;
	auto extrude2 = this->extrudeBody(component, profile, start, dist, 2);

	design->activateRootComponent();
	return std::vector<Ptr<ExtrudeFeature>>{ extrude0, extrude1, extrude2 };
}

void TriplerExecuteEventHandler::cutNotch(
	Ptr<ExtrudeFeature> extrude,
	Ptr<Profile> perpendicular)
{
	Ptr<BRepFace> face = extrude->startFaces()->item(0);
	auto surface = face->geometry();
	if(surface->surfaceType() == PlaneSurfaceType)
	{
		Ptr<Surface> surface = face->geometry();
		Ptr<Vector3D> norm;
		surface->evaluator()->getNormalAtParameter(Point2D::create(0.5, 0.5), norm);

		//auto pNorm = perpendicular->parentSketch()->referencePlane()->normal();
		auto minPt = face->boundingBox()->minPoint();
		auto maxPt = face->boundingBox()->maxPoint();
		auto edges = face->edges();
		auto edge0 = edges->item(0);
		auto edge1 = edges->item(1);
		auto edge2 = edges->item(2);
		std::cout << "test\n";
		std::cout << norm->x() << ", " << norm->y() << ", " << norm->z() << "\n";
		//std::cout << pNorm->x() << ", " << pNorm->y() << ", " << pNorm->z() << "\n";
	}
}
Ptr<adsk::fusion::UserParameter> TriplerExecuteEventHandler::addOrGetParam(
	std::string name, 
	std::string defaultExpression)
{
	Ptr<adsk::fusion::UserParameter> param = params->itemByName(name);
	if (!param)
	{
		param = params->add(name, adsk::core::ValueInput::createByString(defaultExpression), "mm", "");
	}
	return param;
}

Ptr<adsk::fusion::ExtrudeFeature> TriplerExecuteEventHandler::extrudeBody(
	Ptr<adsk::fusion::Component> component,
	Ptr<adsk::fusion::Profile> profile,
	std::string start,
	std::string distance,
	int colorIndex)
{
	auto extrudes = component->features()->extrudeFeatures();
	auto extrudeInput = extrudes->createInput(profile, FeatureOperations::NewBodyFeatureOperation);

	auto startExpr = ValueInput::createByString(start);
	auto distanceExpr = ValueInput::createByString(distance);
	auto zeroExpr = ValueInput::createByReal(0.0);
	Ptr<ExtentDefinition> startExtent = OffsetStartDefinition::create(startExpr);
	auto distExtent = DistanceExtentDefinition::create(distanceExpr);
	auto zeroExtent = DistanceExtentDefinition::create(zeroExpr);

	// BUG: Setting only one side extent evaluates the expression rather than recording it.
	//extrudeInput->setOneSideExtent(distExtent, ExtentDirections::PositiveExtentDirection, zeroExpr);
	extrudeInput->setTwoSidesExtent(distExtent, zeroExtent);
	extrudeInput->startExtent(startExtent);

	extrudeInput->operation(FeatureOperations::NewBodyFeatureOperation);
	auto extrude = extrudes->add(extrudeInput);
	if (extrude->bodies()->count() > 0)
	{
		auto body = extrude->bodies()->item(0);
		body->name("b_" + colorNames[colorIndex]);
		addAppearance(body, colorIndex);
	}
	return extrude;
}

Ptr<adsk::core::Appearance> TriplerExecuteEventHandler::addAppearance(
	Ptr<BRepBody> body,
	int colorIndex)
{
	Ptr<MaterialLibrary> lib = app->materialLibraries()->itemByName("Fusion 360 Appearance Library");
	auto colorName = "Plastic - Matte (" + colorNames[colorIndex] + ")";
	if (!design->appearances()->itemByName(colorName)) {
		auto appearance = lib->appearances()->itemByName(colorName);
		appearance->copyTo(design);
	}
	auto localAppearance = design->appearances()->itemByName(colorName);
	body->appearance(localAppearance);
	return localAppearance;
}

Ptr<adsk::core::Appearance> TriplerExecuteEventHandler::addAppearance(
	Ptr<BRepFace> face,
	int colorIndex)
{
	Ptr<MaterialLibrary> lib = app->materialLibraries()->itemByName("Fusion 360 Appearance Library");
	auto colorName = "Plastic - Matte (" + colorNames[colorIndex] + ")";
	if (!design->appearances()->itemByName(colorName)) {
		auto appearance = lib->appearances()->itemByName(colorName);
		appearance->copyTo(design);
	}
	auto localAppearance = design->appearances()->itemByName(colorName);
	face->appearance(localAppearance);
	return localAppearance;
}

