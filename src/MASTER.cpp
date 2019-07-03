#include "plugin.hpp"



struct MASTER : Module {
	enum ParamIds {
	        GAIN_PARAM,
		ON_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ONT_INPUT,
		GAIN_INPUT,
		LEFT_INPUT,
		RIGHT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		LEFT_MAIN_OUTPUT,
		RIGHT_MAIN_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		ON_LIGHT,
		ENUMS(L_LEVEL_LIGHTS, 11),
		ENUMS(R_LEVEL_LIGHTS, 11),
		NUM_LIGHTS
	};


float SIGNAL_LEFT = 0.0 ;
float SIGNAL_RIGHT = 0.0 ;
bool ON_STATE = false ;
int l_lightState[11] = {};
int r_lightState[11] = {};
dsp::SchmittTrigger onTrigger;
dsp::SchmittTrigger oninTrigger;


	MASTER() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ON_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(GAIN_PARAM, 0.0f, 10.0f, 5.0f, "Gain");
	onReset();
}

void onReset() override {
			ON_STATE = true;
			}

json_t *dataToJson() override {
		json_t *rootJ = json_object();
		

		// solo
		json_object_set_new(rootJ, "onstate", json_integer(ON_STATE));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		

		// solo
		json_t *onstateJ = json_object_get(rootJ, "onstate");
		if (onstateJ)
			ON_STATE = json_integer_value(onstateJ);
	
	}





void process(const ProcessArgs &args) override {

        SIGNAL_LEFT = inputs[LEFT_INPUT].getVoltage() ;
	SIGNAL_RIGHT = inputs[RIGHT_INPUT].getVoltage() ;

	outputs[LEFT_OUTPUT].setVoltage(SIGNAL_LEFT);
	outputs[RIGHT_OUTPUT].setVoltage(SIGNAL_RIGHT);


	if (onTrigger.process(params[ON_PARAM].getValue())+oninTrigger.process(inputs[ONT_INPUT].getVoltage()))
			{if (ON_STATE == 0) ON_STATE = 1; else ON_STATE = 0;}

	SIGNAL_LEFT = SIGNAL_LEFT * ON_STATE * params[GAIN_PARAM].getValue()/5.0;
	SIGNAL_RIGHT = SIGNAL_RIGHT * ON_STATE * params[GAIN_PARAM].getValue()/5.0;

	outputs[LEFT_MAIN_OUTPUT].setVoltage(SIGNAL_LEFT);
	outputs[RIGHT_MAIN_OUTPUT].setVoltage(SIGNAL_RIGHT);
	

	if (ON_STATE==1) lights[ON_LIGHT].setBrightness(1); else lights[ON_LIGHT].setBrightness(0);
	

	for (int i = 0; i < 11; i++) {
		if (SIGNAL_LEFT> i) {if (i<10) l_lightState[i]=5000;else l_lightState[i]=20000;}
	}
	for (int i = 0; i < 11; i++) {
		if (l_lightState[i]> 0) {l_lightState[i]=l_lightState[i]-1;lights[L_LEVEL_LIGHTS + i].setBrightness(1);} else lights[L_LEVEL_LIGHTS + i].setBrightness(0);
	}
	for (int i = 0; i < 11; i++) {
		if (SIGNAL_RIGHT> i) {if (i<10) r_lightState[i]=5000;else r_lightState[i]=20000;}
	}
	for (int i = 0; i < 11; i++) {
		if (r_lightState[i]> 0) {r_lightState[i]=r_lightState[i]-1;lights[R_LEVEL_LIGHTS + i].setBrightness(1);} else lights[R_LEVEL_LIGHTS + i].setBrightness(0);
	}
}
};

struct MASTERWidget : ModuleWidget {
	MASTERWidget(MASTER *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MASTER.svg")));



	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));


    	addParam(createParam<RoundLargeBlackKnob>(Vec(27, 247), module, MASTER::GAIN_PARAM));


     	addParam(createParam<LEDButton>(Vec(38, 208), module, MASTER::ON_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 212.4), module, MASTER::ON_LIGHT));
    
	
	addOutput(createOutput<PJ301MPort>(Vec(54, 61), module, MASTER::LEFT_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 91), module, MASTER::RIGHT_OUTPUT));

	addOutput(createOutput<PJ301MPort>(Vec(54, 308), module, MASTER::LEFT_MAIN_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 334), module, MASTER::RIGHT_MAIN_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(11, 61), module, MASTER::LEFT_INPUT));
	addInput(createInput<PJ301MPort>(Vec(11, 91), module, MASTER::RIGHT_INPUT));


	for (int i = 0; i < 11; i++) {
		if (i<10) addChild(createLight<MediumLight<BlueLight>>(Vec(15, 242-i*12), module, MASTER::L_LEVEL_LIGHTS + i));
			else addChild(createLight<MediumLight<RedLight>>(Vec(15, 242-i*12), module, MASTER::L_LEVEL_LIGHTS + i));
		if (i<10) addChild(createLight<MediumLight<BlueLight>>(Vec(68, 242-i*12), module, MASTER::R_LEVEL_LIGHTS + i));
			else addChild(createLight<MediumLight<RedLight>>(Vec(68, 242-i*12), module, MASTER::R_LEVEL_LIGHTS + i));
	}
	
};	
};

Model *modelMASTER = createModel<MASTER, MASTERWidget>("MASTER");
