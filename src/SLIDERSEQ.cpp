#include "plugin.hpp"



struct SLIDERSEQ : Module {
	enum ParamIds {
		OFFSET_PARAM,
		ENUMS(LVL_PARAM, 16),
		ON_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		RST_INPUT,
		UP_INPUT,
		POS_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		TR_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		OFFSET_LIGHT,
		ENUMS(LVL_LIGHT, 16),
		NUM_LIGHTS
	};


int pas = 0;
bool OFFSET_STATE = false ;

dsp::SchmittTrigger rstTrigger;
dsp::SchmittTrigger upTrigger;
dsp::SchmittTrigger offsetTrigger;
dsp::SchmittTrigger posTrigger;
float sl_pas ;
float sl_value ;

	SLIDERSEQ() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ON_PARAM, 0.f, 1.f, 0.f);
		configButton(OFFSET_PARAM, "-5V offset");
		for (int i = 0; i < 16; i++) {
		configParam(LVL_PARAM + i, 0.f, 1.f, 0.f,"Step " +std::to_string(i+1));
		}
		configInput(RST_INPUT,"Reset trigger");
		configInput(UP_INPUT,"Step trigger");
		configInput(POS_INPUT,"Position control");
		configOutput(TR_OUTPUT,"CV");
}




json_t *dataToJson() override {
		json_t *rootJ = json_object();
		

		json_object_set_new(rootJ, "offsetstate", json_integer(OFFSET_STATE));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		

		json_t *offsetstateJ = json_object_get(rootJ, "offsetstate");
		if (offsetstateJ)
			OFFSET_STATE = json_integer_value(offsetstateJ);
	
	}





void process(const ProcessArgs &args) override {

	
if (!inputs[POS_INPUT].isConnected()) {
	if (rstTrigger.process(inputs[RST_INPUT].getVoltage()))
			{
			pas = -1;
			}
	if (upTrigger.process(inputs[UP_INPUT].getVoltage()))
			{
				if (pas <15) pas = pas+1; else pas =0;
			}


} else { pas = int(inputs[POS_INPUT].getVoltage()*1.6);
	if (pas<0) pas =0;
	if (pas>15) pas =15;
};

	if (offsetTrigger.process(params[OFFSET_PARAM].getValue()))
			{if (OFFSET_STATE == 0) OFFSET_STATE = 1; else OFFSET_STATE = 0;}

	if (OFFSET_STATE==1) lights[OFFSET_LIGHT].setBrightness(1); else lights[OFFSET_LIGHT].setBrightness(0);


if (pas>-1) {
	sl_pas=pas; sl_value = params[LVL_PARAM +pas].getValue() ;
	outputs[TR_OUTPUT].setVoltage(params[LVL_PARAM +pas].getValue()*10-OFFSET_STATE*5.0);}
  else {
	sl_pas=0; sl_value = params[LVL_PARAM +0].getValue() ;
	outputs[TR_OUTPUT].setVoltage(params[LVL_PARAM +0].getValue()*10-OFFSET_STATE*5.0);}

	for (int i = 0; i < 16; i++) {
 		if (i!=pas) {
			lights[LVL_LIGHT+i].setBrightness(0.2f);
			//lights[LVL_LIGHT+i].setSmoothBrightness(0.f, 0.2);
		}
		else {
			lights[LVL_LIGHT+i].setBrightness(5.f);
			//lights[LVL_LIGHT+i].setSmoothBrightness(0.f, 1);
		}
	
 	};

};

};

struct SLIDERSEQWidget : ModuleWidget {
	SLIDERSEQWidget(SLIDERSEQ *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SLIDERSEQ.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addInput(createInput<PJ301MPort>(Vec(10, 320), module, SLIDERSEQ::RST_INPUT));
	addInput(createInput<PJ301MPort>(Vec(39, 320), module, SLIDERSEQ::UP_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(100, 320), module, SLIDERSEQ::TR_OUTPUT));


     	addParam(createParam<LEDButton>(Vec(84, 288), module, SLIDERSEQ::OFFSET_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(88.4, 292.4), module, SLIDERSEQ::OFFSET_LIGHT));


	for (int i = 0; i < 8; i++) {
addParam(createLightParamCentered<LEDLightSlider<BlueLight>>(Vec(4+i*15+10, 30+ 30+30), module, SLIDERSEQ::LVL_PARAM+i, SLIDERSEQ::LVL_LIGHT+i));
		//addParam(createParam<LEDSliderWhite>(Vec(4+i*15, 30+ 30), module, SLIDERSEQ::LVL_PARAM+i));
		
	}
	for (int i = 8; i < 16; i++) {
addParam(createLightParamCentered<LEDLightSlider<BlueLight>>(Vec(4+(i-8)*15+10, 30+ 155+30), module, SLIDERSEQ::LVL_PARAM+i, SLIDERSEQ::LVL_LIGHT+i));
		//addParam(createParam<LEDSliderWhite>(Vec(4+(i-8)*15, 30+ 155), module, SLIDERSEQ::LVL_PARAM+i));
		
	}


	addInput(createInput<PJ301MPort>(Vec(68, 320), module, SLIDERSEQ::POS_INPUT));
};
};

Model *modelSLIDERSEQ = createModel<SLIDERSEQ, SLIDERSEQWidget>("SLIDERSEQ");

