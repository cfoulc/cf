#include "plugin.hpp"
#include "dsp/digital.hpp"


struct L3DS3Q : Module {
	enum ParamIds {
		EDIT_PARAM,
		ON_PARAM,
		NUM_PARAMS = ON_PARAM + 80
	};
	enum InputIds {
		RST_INPUT,
		UP_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		TR_OUTPUT,
		NUM_OUTPUTS = TR_OUTPUT + 5
	};
    enum LightIds {
		EDIT_LIGHT,
		LED_LIGHT,
		NUM_LIGHTS = LED_LIGHT + 80
	};


int pas = 0;
bool ledState[80] = {};
int tempState[5] = {};
bool editState = false ;
dsp::SchmittTrigger rstTrigger;
dsp::SchmittTrigger upTrigger;
dsp::SchmittTrigger editTrigger;
dsp::SchmittTrigger ledTrigger[80] ={};

	L3DS3Q() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(EDIT_PARAM, 0.f, 1.f, 0.f);
		for (int i = 0; i < 80; i++) {
			configParam(ON_PARAM + i, 0.f, 1.f, 0.f);
		}
}


json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// leds
		json_t *ledsJ = json_array();
		for (int i = 0; i < 80; i++) {
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
			for (int i = 0; i < 80; i++) {
				json_t *ledJ = json_array_get(ledsJ, i);
				if (ledJ)
					ledState[i] = !!json_integer_value(ledJ);
			}
		}

	}

	void onReset() override {
		for (int i = 0; i < 80; i++) {
			ledState[i] = false;
		}
	}

	void onRandomize() override {
		for (int i = 0; i < 80; i++) {
			ledState[i] = (random::uniform() > 0.5);
		}
	}




void process(const ProcessArgs &args) override {

	if (rstTrigger.process(inputs[RST_INPUT].value))
			{
			pas = 0;
			}

	if (upTrigger.process(inputs[UP_INPUT].value))
			{
				for (int i = 0; i < 5; i++) {
					if (ledState[(i+pas*5)%80]) tempState [i] = 50;
				}
				if (pas <15) pas = pas+1; else pas =0;
			}

	if (editTrigger.process(params[EDIT_PARAM].value))
			{
			editState = !editState ;
			lights[EDIT_LIGHT].value= editState ;
			}
	if (!editState)
		{
			for (int i = 0; i < 80; i++) {lights[LED_LIGHT +i].value=ledState[(i+pas*5)%80];}

				for (int i = 0; i < 80; i++) {
					if (ledTrigger[i].process(params[ON_PARAM +i].value)) {ledState[(i+pas*5)%80]=!ledState[(i+pas*5)%80];}
			};

		} else {
			for (int i = 0; i < 80; i++) {lights[LED_LIGHT +i].value=ledState[i];}

				for (int i = 0; i < 80; i++) {
					if (ledTrigger[i].process(params[ON_PARAM +i].value)) {ledState[i]=!ledState[i];}
				};
		}

	for (int i = 0; i < 5; i++) {
			if (tempState [i]>0) {tempState [i] = tempState [i]-1;outputs[TR_OUTPUT+i].value=10.0f;} else outputs[TR_OUTPUT+i].value=0.0f;
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

struct L3DS3QWidget : ModuleWidget {
	L3DS3QWidget(L3DS3Q *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/L3DS3Q.svg")));


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	for (int i = 0; i < 16; i++) {
	for (int j = 0; j < 5; j++) {
     		addParam(createParam<LButton>(Vec(j*15+10-0.8, (15-i)*15+15-0.8+51), module, L3DS3Q::ON_PARAM + (i*5+j)));
		addChild(createLight<MediumLight<BlueLight>>(Vec(j*15+10, (15-i)*15+15+51), module, L3DS3Q::LED_LIGHT + (i*5+j)));
	}}

	addInput(createInput<PJ301MPort>(Vec(32, 27), module, L3DS3Q::RST_INPUT));
	addInput(createInput<PJ301MPort>(Vec(4, 27), module, L3DS3Q::UP_INPUT));  

	addParam(createParam<LEDButton>(Vec(65, 31), module, L3DS3Q::EDIT_PARAM));
		addChild(createLight<MediumLight<BlueLight>>(Vec(69.4, 35.4), module, L3DS3Q::EDIT_LIGHT));

	for (int i = 0; i < 5; i++) {
		addOutput(createOutput<PJ301MPort>(Vec(4+i*14, 332- 22*(i%2)), module, L3DS3Q::TR_OUTPUT +i));
	};


	
	
};
};

Model *modelL3DS3Q = createModel<L3DS3Q, L3DS3QWidget>("L3DS3Q");


