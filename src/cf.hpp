#include "rack.hpp"

using namespace rack;


extern Plugin *plugin;

////////////////////
// module widgets
////////////////////


struct PLAYERWidget : ModuleWidget {
	PLAYERWidget();
	Menu *createContextMenu() override;
};

struct STEPSWidget : ModuleWidget {
	STEPSWidget();
};

struct DAVEWidget : ModuleWidget {
	DAVEWidget();
};