


#include "plugin.hpp"


struct DAVE : Module {
	enum ParamIds {
		
		NUM_PARAMS
	};
	enum InputIds {

		NUM_INPUTS
	};
	enum OutputIds {

		NUM_OUTPUTS
	};


	DAVE() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	}

};



struct DAVEWidget : ModuleWidget {
	DAVEWidget(DAVE *module) {
setModule(module);
setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DAVE.svg")));
}
};


Model *modelDAVE = createModel<DAVE, DAVEWidget>("DAVE");
