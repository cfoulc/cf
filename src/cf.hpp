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

struct PEAKWidget : ModuleWidget {
	PEAKWidget();
};

struct FOURWidget : ModuleWidget {
	FOURWidget();
};

struct trSEQWidget : ModuleWidget {
	trSEQWidget();
Menu *createContextMenu() override;
};

struct DAVEWidget : ModuleWidget {
	DAVEWidget();
};
struct CUBEWidget : ModuleWidget {
	CUBEWidget();
};
struct MONOWidget : ModuleWidget {
	MONOWidget();
};
struct STEREOWidget : ModuleWidget {
	STEREOWidget();
};
struct MASTERWidget : ModuleWidget {
	MASTERWidget();
};
struct METROWidget : ModuleWidget {
	METROWidget();
};
struct EACHWidget : ModuleWidget {
	EACHWidget();
};
struct PATCHWidget : ModuleWidget {
	PATCHWidget();
};

struct PadButton : SVGSwitch, MomentarySwitch {
	PadButton() {
		addFrame(SVG::load(assetPlugin(plugin, "res/PadButton.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/PadButtonDown.svg")));
		sw->wrap();
		box.size = sw->box.size;
	}
};
struct upButton : SVGSwitch, MomentarySwitch {
	upButton() {
		addFrame(SVG::load(assetPlugin(plugin, "res/upButton.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/upButtonDown.svg")));
		sw->wrap();
		box.size = sw->box.size;
	}
};
struct downButton : SVGSwitch, MomentarySwitch {
	downButton() {
		addFrame(SVG::load(assetPlugin(plugin, "res/downButton.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/downButtonDown.svg")));
		sw->wrap();
		box.size = sw->box.size;
	}
};