#include "plugin.hpp"


struct PATCH : Module {

	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		L_INPUT,
		R_INPUT,
		M1_INPUT,
		M2_INPUT,
		M3_INPUT,
		ENUMS(IN_INPUT,8),
		NUM_INPUTS
	};
	enum OutputIds {
		L_OUTPUT,
		R_OUTPUT,
		M1_OUTPUT,
		M2_OUTPUT,
		M3_OUTPUT,
		ENUMS(OUT_OUTPUT,8),
		NUM_OUTPUTS
	};
   	enum LightIds {
		NUM_LIGHTS
	};






PATCH() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS,NUM_LIGHTS);
};


void process(const ProcessArgs &args) override {

	outputs[L_OUTPUT].setVoltage(inputs[L_INPUT].getVoltage());
	outputs[R_OUTPUT].setVoltage(inputs[R_INPUT].getVoltage());

	outputs[M1_OUTPUT].setVoltage(inputs[M1_INPUT].getVoltage()+inputs[M2_INPUT].getVoltage()+inputs[M3_INPUT].getVoltage());
	outputs[M2_OUTPUT].setVoltage(inputs[M1_INPUT].getVoltage()+inputs[M2_INPUT].getVoltage()+inputs[M3_INPUT].getVoltage());
	outputs[M3_OUTPUT].setVoltage(inputs[M1_INPUT].getVoltage()+inputs[M2_INPUT].getVoltage()+inputs[M3_INPUT].getVoltage());

	for (int i = 0; i < 8; i++) {
		if (inputs[IN_INPUT + i].isConnected()) outputs[OUT_OUTPUT + i].setVoltage(inputs[IN_INPUT + i].getVoltage());
			else outputs[OUT_OUTPUT + i].setVoltage(0);	
		}
	
};
};



struct PATCHWidget : ModuleWidget {
	PATCHWidget(PATCH *module) {
	setModule(module);
	setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PATCH.svg")));


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addInput(createInput<PJ301MPort>(Vec(10, 171), module, PATCH::L_INPUT));
	addInput(createInput<PJ301MPort>(Vec(40, 171), module, PATCH::R_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(70, 171), module, PATCH::L_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(100, 171), module, PATCH::R_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(15, 61), module, PATCH::M1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(55, 41), module, PATCH::M2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(95, 71), module, PATCH::M3_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(40, 121), module, PATCH::M1_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(55, 81), module, PATCH::M2_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(80, 111), module, PATCH::M3_OUTPUT));

for (int i = 0; i < 8; i++) {

	addInput(createInput<PJ301MPort>(Vec(10+(i-4*floor(i/4))*30, 231+60*floor(i/4)), module, PATCH::IN_INPUT + i));
	addOutput(createOutput<PJ301MPort>(Vec(10+(i-4*floor(i/4))*30, 261+60*floor(i/4)), module, PATCH::OUT_OUTPUT + i));

	
}
}
	
};

Model *modelPATCH = createModel<PATCH, PATCHWidget>("PATCH");
