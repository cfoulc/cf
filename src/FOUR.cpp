#include "plugin.hpp"


struct FOUR : Module {
	enum ParamIds {
       		ENUMS(S_PARAM,4),
        	ENUMS(M_PARAM,4),
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(TRM_INPUT,4),
		ENUMS(TRS_INPUT,4),
		ENUMS(IN_INPUT,4),
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUT_OUTPUT,4),
		NUM_OUTPUTS
	};
    enum LightIds {
		ENUMS(M_LIGHT,4),
		ENUMS(S_LIGHT,4),
		NUM_LIGHTS
	};


bool muteState[8] = {};
int solo = 0;
int cligno = 0;

dsp::SchmittTrigger muteTrigger[8];
dsp::SchmittTrigger soloTrigger[8];


	FOUR() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	for (int i = 0; i < 4; i++) {
		configButton(M_PARAM + i,"Mute "+std::to_string(i+1));
		configButton(S_PARAM + i,"Solo "+std::to_string(i+1));
		configInput(IN_INPUT+i,"Number "+std::to_string(i+1));
		configInput(TRM_INPUT+i,"Mute trigger "+std::to_string(i+1));
		configInput(TRS_INPUT+i,"Solo trigger "+std::to_string(i+1));
		configOutput(OUT_OUTPUT+i,"Number "+std::to_string(i+1));
	}
	onReset();
}

void onReset() override {
		for (int i = 0; i < 4; i++) {
			muteState[i] = true;
			muteState[i+4] = false;
		}
		solo = 0;
		
		
	}

void onRandomize() override {
		for (int i = 0; i < 8; i++) {
			muteState[i] = (random::uniform() < 0.5);
		}
	}

json_t *dataToJson() override {
		json_t *rootJ = json_object();
		
		// states
		json_t *mutestatesJ = json_array();
		for (int i = 0; i < 8; i++) {
			json_t *mutestateJ = json_boolean(muteState[i]);
			json_array_append_new(mutestatesJ, mutestateJ);
			}
		json_object_set_new(rootJ, "mutestates", mutestatesJ);

		// solo
		json_object_set_new(rootJ, "solo", json_integer(solo));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		
		// states
		json_t *mutestatesJ = json_object_get(rootJ, "mutestates");
		if (mutestatesJ) {
			for (int i = 0; i < 8; i++) {
				json_t *mutestateJ = json_array_get(mutestatesJ, i);
				if (mutestateJ)
					muteState[i] = json_boolean_value(mutestateJ);
			}
		}
		// solo
		json_t *soloJ = json_object_get(rootJ, "solo");
		if (soloJ)
			solo = json_integer_value(soloJ);
	
	}





void process(const ProcessArgs &args) override {
		
	for (int i = 0; i < 4; i++) {
	
		if (soloTrigger[i].process(params[S_PARAM + i].getValue())+soloTrigger[i+4].process(inputs[TRS_INPUT + i].getVoltage()))
			{
			muteState[i+4] ^= true;
			solo = (i+1)*muteState[i+4];
			};		
		if (solo==i+1)
		{
			float in = inputs[IN_INPUT + i].getVoltage();
			outputs[OUT_OUTPUT + i].setVoltage(in);
			
		} else {muteState[i+4] = false;lights[S_LIGHT + i].setBrightness(0);outputs[OUT_OUTPUT + i].setVoltage(0.0);}
		if (muteState[i+4]==true)
		{
			cligno = cligno + 1;
			if (cligno ==10000) {lights[S_LIGHT + i].setBrightness(!lights[S_LIGHT + i].getBrightness());cligno =0;}
		}		
	}

	for (int i = 0; i < 4; i++) {
		if (muteTrigger[i].process(params[M_PARAM + i].getValue())+muteTrigger[i+4].process(inputs[TRM_INPUT + i].getVoltage()))
			muteState[i] ^= true;
		float in = inputs[IN_INPUT + i].getVoltage();
		if (solo == 0) outputs[OUT_OUTPUT + i].setVoltage(muteState[i] ? in : 0.0);
		lights[M_LIGHT + i].setBrightness(muteState[i]);
	}

	
}
};

struct FOURWidget : ModuleWidget {
	FOURWidget(FOUR *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/FOUR.svg")));

	int y = 56;


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	for (int i = 0; i < 4; i++) {

	  addInput(createInput<PJ301MPort>(Vec(15, y), module, FOUR::IN_INPUT + i));

	  addInput(createInput<PJ301MPort>(Vec(21, y+25), module, FOUR::TRS_INPUT + i));
        	addParam(createParam<LEDButton>(Vec(45, y+4), module, FOUR::S_PARAM + i));
		addChild(createLight<MediumLight<BlueLight>>(Vec(45+4.4, y+8.4), module, FOUR::S_LIGHT + i));

	  addInput(createInput<PJ301MPort>(Vec(46, y+31), module, FOUR::TRM_INPUT + i));
   		addParam(createParam<LEDButton>(Vec(70, y+4), module, FOUR::M_PARAM + i));
 		addChild(createLight<MediumLight<BlueLight>>(Vec(70+4.4, y+8.4), module, FOUR::M_LIGHT + i));

	  addOutput(createOutput<PJ301MPort>(Vec(95, y), module, FOUR::OUT_OUTPUT + i));

	  y = y + 75 ;
	}
	
}
};

Model *modelFOUR = createModel<FOUR, FOURWidget>("FOUR");