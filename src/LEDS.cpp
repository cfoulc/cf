#include "cf.hpp"
#include "dsp/digital.hpp"


struct LEDS : Module {
	enum ParamIds {
		ON_PARAM,
		NUM_PARAMS = ON_PARAM + 100
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
    enum LightIds {
		LED_LIGHT,
		NUM_LIGHTS = LED_LIGHT + 100
	};


int wait = 0;
bool ledState[100] = {};



	LEDS() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

json_t *toJson() override {
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

	void fromJson(json_t *rootJ) override {

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

	void reset() override {
		for (int i = 0; i < 100; i++) {
			ledState[i] = false;
		}
	}

	void randomize() override {
		for (int i = 0; i < 100; i++) {
			ledState[i] = (randomf() > 0.5);
		}
	}

};


void LEDS::step() {


	if (wait == 0) {
		for (int i = 0; i < 100; i++) {
			
			if (params[ON_PARAM +i].value) {ledState[i]=!ledState[i]; wait = 20000;}
			lights[LED_LIGHT +i].value=ledState[i];
	}} else wait = wait-1;


}


struct LButton : SVGSwitch, MomentarySwitch {
	LButton() {
		addFrame(SVG::load(assetPlugin(plugin, "res/L.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Ldown.svg")));
		sw->wrap();
		box.size = sw->box.size;
	}
};

LEDSWidget::LEDSWidget() {
	LEDS *module = new LEDS();
	setModule(module);
	box.size = Vec(15*6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/LEDS.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 365)));

	for (int i = 0; i < 20; i++) {
	for (int j = 0; j < 5; j++) {
     		addParam(createParam<LButton>(Vec(j*15+10-0.8, i*15+45-0.8), module, LEDS::ON_PARAM + (i+j*20), 0.0, 1.0, 0.0));
		addChild(createLight<MediumLight<BlueLight>>(Vec(j*15+10, i*15+45), module, LEDS::LED_LIGHT + (i+j*20)));
	}}


	
	
}
