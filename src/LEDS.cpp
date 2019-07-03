#include "plugin.hpp"



struct LEDS : Module {
	enum ParamIds {
		ENUMS(ON_PARAM, 100),
		NUM_PARAMS
	};
	enum InputIds {
		RND_INPUT,
		UP_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
    enum LightIds {
		ENUMS(LED_LIGHT, 100),
		NUM_LIGHTS
	};


int wait = 0;
bool ledState[100] = {};
bool tempState[5] = {};
dsp::SchmittTrigger rndTrigger;
dsp::SchmittTrigger upTrigger;
dsp::SchmittTrigger ledTrigger[100];


	LEDS() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < 100; i++) {
			configParam(ON_PARAM + i, 0.f, 1.f, 0.f);
		}
}


json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// leds
		json_t *ledsJ = json_array();
		for (int i = 0; i < 100; i++) {
			json_t *ledJ = json_integer((int) ledState[i]);
			json_array_append_new(ledsJ, ledJ);
		}
		json_object_set_new(rootJ, "leds", ledsJ);

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {

		// leds
		json_t *ledsJ = json_object_get(rootJ, "leds");
		if (ledsJ) {
			for (int i = 0; i < 100; i++) {
				json_t *ledJ = json_array_get(ledsJ, i);
				if (ledJ)
					ledState[i] = !!json_integer_value(ledJ);
			}
		}

	}

	void onReset() override {
		for (int i = 0; i < 100; i++) {
			ledState[i] = false;
		}
	}

	void onRandomize() override {
		for (int i = 0; i < 100; i++) {
			ledState[i] = (random::uniform() > 0.5);
		}
	}




void process(const ProcessArgs &args) override {

	if (rndTrigger.process(inputs[RND_INPUT].getVoltage()))
			{for (int i = 0; i < 100; i++) 
				{ledState[i] = (random::uniform() > 0.5);}
			}

	if (upTrigger.process(inputs[UP_INPUT].getVoltage()))
			{
			for (int i = 0; i < 5; i++) 
				{tempState[i] = ledState[i];}

			for (int i = 0; i < 95; i++) 
				{ledState[i] = ledState[i+5];}

			for (int i = 0; i < 5; i++) 
				{ledState[i+95] = tempState[i];}
			}

	
		for (int i = 0; i < 100; i++) {
			
			if (ledTrigger[i].process(params[ON_PARAM +i].getValue())) {ledState[i]=!ledState[i];}
			lights[LED_LIGHT +i].setBrightness(ledState[i]);
		}


};
};

struct LButton : app::SvgSwitch {
	LButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/L.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Ldown.svg")));
	}
};

struct LEDSWidget : ModuleWidget {
	LEDSWidget(LEDS *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LEDS.svg")));


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	for (int i = 0; i < 20; i++) {
	for (int j = 0; j < 5; j++) {
     		addParam(createParam<LButton>(Vec(j*15+10-0.8, i*15+35-0.8), module, LEDS::ON_PARAM + (i*5+j)));
		addChild(createLight<MediumLight<BlueLight>>(Vec(j*15+10, i*15+35), module, LEDS::LED_LIGHT + (i*5+j)));
	}}
	addInput(createInput<PJ301MPort>(Vec(11, 340), module, LEDS::RND_INPUT));
	addInput(createInput<PJ301MPort>(Vec(54, 340), module, LEDS::UP_INPUT));

	
	
};
};

Model *modelLEDS = createModel<LEDS, LEDSWidget>("LEDS");
