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

struct trSEQWidget : ModuleWidget {
	trSEQWidget();
Menu *createContextMenu() override;
};

struct DAVEWidget : ModuleWidget {
	DAVEWidget();
};