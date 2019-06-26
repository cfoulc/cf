#include "plugin.hpp"



using namespace std;


struct CHOKE : Module {
	enum ParamIds {
		PAN_PARAM,
		NUM_PARAMS 
	};
	enum InputIds {
		TRIG1_INPUT,
		TRIG2_INPUT,
		IN1_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		L2_LIGHT,
		NUM_LIGHTS
	};
	
	bool play = false;
	dsp::SchmittTrigger tr1Trigger;
	dsp::SchmittTrigger tr2Trigger;

CHOKE()  { 
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PAN_PARAM, -1.0f, 1.0f, 0.0f, "Pan");
}


void process(const ProcessArgs &args) override {

	if (tr1Trigger.process(inputs[TRIG1_INPUT].getVoltage()))
		{
		play = false ;

		};
	if (tr2Trigger.process(inputs[TRIG2_INPUT].getVoltage()))
		{
		play = true ;

		};
	if (play) 
		outputs[OUT_OUTPUT].setVoltage(inputs[IN2_INPUT].getVoltage()*(1-clamp(-params[PAN_PARAM].getValue(),0.0f,1.0f)));
			else outputs[OUT_OUTPUT].setVoltage(inputs[IN1_INPUT].getVoltage()*(1-clamp(params[PAN_PARAM].getValue(),0.0f,1.0f))); 
		

	lights[L2_LIGHT].value=play;
}
};



struct CHOKEWidget : ModuleWidget {
	CHOKEWidget(CHOKE *module) {
	setModule(module);
	
	setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CHOKE.svg")));


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

 	addParam(createParam<Trimpot>(Vec(6, 298), module, CHOKE::PAN_PARAM));

	addInput(createInput<PJ301MPort>(Vec(3, 61), module, CHOKE::IN1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(3, 91), module, CHOKE::TRIG1_INPUT));

	addInput(createInput<PJ301MPort>(Vec(3, 181), module, CHOKE::IN2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(3, 211), module, CHOKE::TRIG2_INPUT));

	addChild(createLight<LargeLight<BlueLight>>(Vec(8, 276), module, CHOKE::L2_LIGHT));

	addOutput(createOutput<PJ301MPort>(Vec(3, 321), module, CHOKE::OUT_OUTPUT));

}
};


Model *modelCHOKE = createModel<CHOKE, CHOKEWidget>("CHOKE");
