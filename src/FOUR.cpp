#include "cf.hpp"
#include "dsp/digital.hpp"

struct FOUR : Module {
	enum ParamIds {
        S_PARAM,
        M_PARAM=S_PARAM + 4,
	NUM_PARAMS = M_PARAM + 4
	};
	enum InputIds {
		SOLOED_INPUT,
		TRM_INPUT,
		TRS_INPUT=TRM_INPUT+4,
		IN_INPUT=TRS_INPUT+4,
		NUM_INPUTS=IN_INPUT+4
	};
	enum OutputIds {
		SOLOED_OUTPUT,
		OUT_OUTPUT,
		NUM_OUTPUTS=OUT_OUTPUT+4
	};
    enum LightIds {
		TEST_LIGHT,
		M_LIGHT,
		S_LIGHT=M_LIGHT+4,
		NUM_LIGHTS=S_LIGHT+4
	};

bool soloState[4] = {};
bool muteState[4] = {};
int solo = 0;
int cligno = 0;
int shot = 0;

SchmittTrigger soloedTrigger;
SchmittTrigger actTrigger;
SchmittTrigger muteTrigger[8];
SchmittTrigger soloTrigger[8];

PulseGenerator gatePulse;

FOUR() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

json_t *toJson() override {
		json_t *rootJ = json_object();
		// states
		json_t *mutestatesJ = json_array();
		for (int i = 0; i < 4; i++) {
			json_t *mutestateJ = json_boolean(muteState[i]);
			json_array_append_new(mutestatesJ, mutestateJ);
			}
		json_object_set_new(rootJ, "mutestates", mutestatesJ);
		return rootJ;
		}
	void fromJson(json_t *rootJ) override {
		// states
		json_t *mutestatesJ = json_object_get(rootJ, "mutestates");
		if (mutestatesJ) {
			for (int i = 0; i < 4; i++) {
				json_t *mutestateJ = json_array_get(mutestatesJ, i);
				if (mutestateJ)
					muteState[i] = json_boolean_value(mutestateJ);
			}
		}
	
	}


};


void FOUR::step() {

	
	for (int i = 0; i < 4; i++) {
		if (soloTrigger[i].process(params[S_PARAM + i].value)+soloTrigger[i+4].process(inputs[TRS_INPUT + i].value))
			{
			soloState[i] ^= true;
			solo = (i+1)*soloState[i];
			};
		if (solo==i+1)
		{
			float in = inputs[IN_INPUT + i].value;
			outputs[OUT_OUTPUT + i].value = in;
			
		} else {soloState[i] = false;lights[S_LIGHT + i].value = 0;outputs[OUT_OUTPUT + i].value = 0.0;}
		if (soloState[i]==true)
		{
			cligno = cligno + 1;
			if (cligno ==10000) {lights[S_LIGHT + i].value = !lights[S_LIGHT + i].value;cligno =0;}
		}		
	}
	for (int i = 0; i < 4; i++) {
		if (muteTrigger[i].process(params[M_PARAM + i].value)+muteTrigger[i+4].process(inputs[TRM_INPUT + i].value))
			muteState[i] ^= true;
		float in = inputs[IN_INPUT + i].value;
		if (solo == 0) outputs[OUT_OUTPUT + i].value = muteState[i] ? in : 0.0;
		lights[M_LIGHT + i].value = muteState[i];
	}

	if (soloedTrigger.process(inputs[SOLOED_INPUT].value)) {solo=0;}
if (solo>0) outputs[SOLOED_OUTPUT].value = 10; else outputs[SOLOED_OUTPUT].value = 0.0;
//	if (soloedTrigger.process(inputs[SOLOED_INPUT].value)) {solo=0;shot=1;} 
//	if ((solo>0)||(shot>0)) {outputs[SOLOED_OUTPUT].value = 10;shot=0;} else {outputs[SOLOED_OUTPUT].value = 0.0;}
//||
}

FOURWidget::FOURWidget() {
	FOUR *module = new FOUR();
	setModule(module);
	int y = 56;
	box.size = Vec(15*9, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/FOUR.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 365)));

for (int i = 0; i < 4; i++) {

	addInput(createInput<PJ301MPort>(Vec(15, y), module, FOUR::IN_INPUT + i));

	addInput(createInput<PJ301MPort>(Vec(21, y+25), module, FOUR::TRS_INPUT + i));
        	addParam(createParam<LEDButton>(Vec(45, y+4), module, FOUR::S_PARAM + i, 0.0, 1.0, 0.0));
		addChild(createLight<MediumLight<BlueLight>>(Vec(45+4.4, y+8.4), module, FOUR::S_LIGHT + i));

	addInput(createInput<PJ301MPort>(Vec(46, y+31), module, FOUR::TRM_INPUT + i));
   		addParam(createParam<LEDButton>(Vec(70, y+4), module, FOUR::M_PARAM + i, 0.0, 1.0, 1.0));
 		addChild(createLight<MediumLight<BlueLight>>(Vec(70+4.4, y+8.4), module, FOUR::M_LIGHT + i));

	addOutput(createOutput<PJ301MPort>(Vec(95, y), module, FOUR::OUT_OUTPUT + i));

	y = y + 75 ;
}
addInput(createInput<PJ301MPort>(Vec(15, y-20), module, FOUR::SOLOED_INPUT));
addOutput(createOutput<PJ301MPort>(Vec(95, y-20), module, FOUR::SOLOED_OUTPUT));
addChild(createLight<MediumLight<BlueLight>>(Vec(70+4.4, y+18.4), module, FOUR::TEST_LIGHT));
	
}
