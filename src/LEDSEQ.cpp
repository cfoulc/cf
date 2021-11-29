#include "plugin.hpp"



struct LEDSEQ : Module {
	enum ParamIds {
		EDIT_PARAM,
		ENUMS(ON_PARAM,80),
		NUM_PARAMS
	};
	enum InputIds {
		RST_INPUT,
		UP_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(TR_OUTPUT,5),
		NUM_OUTPUTS
	};
    enum LightIds {
		EDIT_LIGHT,
		ENUMS(LED_LIGHT,80),
		NUM_LIGHTS
	};



int pas = 0;
bool ledState[80] = {};
int tempState[5] = {};
bool editState = false ;
dsp::SchmittTrigger rstTrigger;
dsp::SchmittTrigger upTrigger;
dsp::SchmittTrigger editTrigger;
dsp::SchmittTrigger ledTrigger[80] ={};


	LEDSEQ() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configButton(EDIT_PARAM, "Edit mode");
		for (int i = 0; i < 80; i++) {
			configButton(ON_PARAM + i, std::to_string(int(i%5)+1) + ":" + std::to_string(int(i/5)+1));
		}
		for (int i = 0; i < 5; i++) {
			configOutput(TR_OUTPUT+i,"No " +std::to_string(i+1)+ " trigger");
		}
		configInput(RST_INPUT,"Reset trigger");
		configInput(UP_INPUT,"Step trigger");
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

	if (rstTrigger.process(inputs[RST_INPUT].getVoltage()))
			{
			pas = 0;
			}

	if (upTrigger.process(inputs[UP_INPUT].getVoltage()))
			{
				for (int i = 0; i < 5; i++) {
					if (ledState[(i+pas*5)%80]) tempState [i] = 50;
				}
				if (pas <15) pas = pas+1; else pas =0;
			}

	if (editTrigger.process(params[EDIT_PARAM].getValue()))
			{
			editState = !editState ;
			lights[EDIT_LIGHT].setBrightness(editState) ;
			}
	if (!editState)
		{
			for (int i = 0; i < 80; i++) {lights[LED_LIGHT +i].setBrightness(ledState[(i+pas*5)%80]);}

			
				for (int i = 0; i < 80; i++) {
					if (ledTrigger[i].process(params[ON_PARAM +i].getValue())) {ledState[(i+pas*5)%80]=!ledState[(i+pas*5)%80];}
			};

		} else {
			for (int i = 0; i < 80; i++) {lights[LED_LIGHT +i].setBrightness(ledState[i]);}
			
				for (int i = 0; i < 80; i++) {
					if (ledTrigger[i].process(params[ON_PARAM +i].getValue())) {ledState[i]=!ledState[i];}
				};
		}

	for (int i = 0; i < 5; i++) {
			if (tempState [i]>0) {tempState [i] = tempState [i]-1;outputs[TR_OUTPUT+i].setVoltage(10.0f);} else outputs[TR_OUTPUT+i].setVoltage(0.0f);
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

struct LEDSEQWidget : ModuleWidget {
	LEDSEQWidget(LEDSEQ *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LEDSEQ.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));




	for (int i = 0; i < 16; i++) {
	for (int j = 0; j < 5; j++) {
     		addParam(createParam<LButton>(Vec(j*15+10-0.8, i*15+35-0.8+51), module, LEDSEQ::ON_PARAM + (i*5+j)));
		addChild(createLight<MediumLight<BlueLight>>(Vec(j*15+10, i*15+35+51), module, LEDSEQ::LED_LIGHT + (i*5+j)));
	}}
	addInput(createInput<PJ301MPort>(Vec(4, 340), module, LEDSEQ::RST_INPUT));
	addInput(createInput<PJ301MPort>(Vec(60, 340), module, LEDSEQ::UP_INPUT));  

	addParam(createParam<LEDButton>(Vec(35, 340), module, LEDSEQ::EDIT_PARAM));
		addChild(createLight<MediumLight<BlueLight>>(Vec(39.4, 344.4), module, LEDSEQ::EDIT_LIGHT));

	for (int i = 0; i < 5; i++) {
		addOutput(createOutput<PJ301MPort>(Vec(4+i*14, 30+ 22*(i%2)), module, LEDSEQ::TR_OUTPUT +i));
	};


	

};
};

Model *modelLEDSEQ =createModel<LEDSEQ, LEDSEQWidget>("LEDSEQ");

