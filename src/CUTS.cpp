#include "plugin.hpp"



struct CUTS : Module {
	enum ParamIds {
		POTF_PARAM,
		POTR_PARAM,
		LINK_PARAM,
		POLE_PARAM,
BPOTF_PARAM,
BPOTR_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		IN2_INPUT,
		F_INPUT,
		R_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		LINK_LIGHT,
		NUM_LIGHTS
	};

int poles = 4;

float prevf1[8];
float prevf2[8];
float prevf3[8];
float prevf4[8];

float delta=0.0;
float temp1=0.0;
float temp2=0.0;
float temp3=0.0;
float temp4=0.0;

float rin=0;
bool rv =false;
float fin=0;
bool fv =false;

bool LINK_STATE =false;
float link_delta;
dsp::SchmittTrigger linkTrigger;

	CUTS() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LINK_PARAM, 0.0, 1.0, 0.0, "Link");
		configParam(POLE_PARAM, 1.0f, 8.0f, 4.0f, "Pole");
		configParam(POTR_PARAM, 0.0f, 1.0f, 0.0f, "R");
		configParam(POTF_PARAM, 0.0f, 1.0f, 1.0f, "F");
		configParam(BPOTR_PARAM, 0.0f, 1.0f, 0.0f, "R");
		configParam(BPOTF_PARAM, 0.0f, 1.0f, 1.0f, "F");
onReset();
}


	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		

		json_object_set_new(rootJ, "linkstate", json_integer(LINK_STATE));
		json_object_set_new(rootJ, "linkdelta", json_real(link_delta));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		

		json_t *linkstateJ = json_object_get(rootJ, "linkstate");
		if (linkstateJ)
			LINK_STATE = json_integer_value(linkstateJ);

		json_t *ldJ = json_object_get(rootJ, "linkdelta");
		if (ldJ)
			link_delta = json_real_value(ldJ);
	
	}



void process(const ProcessArgs &args) override {
		
poles = int(params[POLE_PARAM].getValue());

if (linkTrigger.process(params[LINK_PARAM].getValue()))
			{if (LINK_STATE == 0) {LINK_STATE = 1; link_delta = fin-rin;} else LINK_STATE = 0;}
lights[LINK_LIGHT].setBrightness(LINK_STATE);

if (inputs[R_INPUT].isConnected()) {
			rv = true;
			rin = clamp(inputs[R_INPUT].getVoltage(),0.0,10.0)/10.0;
params[BPOTR_PARAM].setValue(rin);
		} else {
			rv = false;
			rin = params[POTR_PARAM].getValue();
		}

if (!LINK_STATE) {
	if (inputs[F_INPUT].isConnected()) {
				fv = true;
				fin = clamp(inputs[F_INPUT].getVoltage(),0.0,10.0)/10.0;
params[BPOTF_PARAM].setValue(fin);
			} else {
				fv = false;
				fin = params[POTF_PARAM].getValue();
			}
	} else {
		if (inputs[R_INPUT].isConnected()) {
				fv = true;
				fin = clamp(inputs[R_INPUT].getVoltage()/10.0+link_delta,0.0f,1.0f);
params[BPOTF_PARAM].setValue(fin);
////////////////////params[BPOTR_PARAM].setValue(fin);
			} else {
				if (inputs[F_INPUT].isConnected()) {
					rv = true;
					rin = clamp(inputs[F_INPUT].getVoltage()/10-link_delta,0.0f,1.0f);
params[BPOTR_PARAM].setValue(rin);
					fv = true;
					fin = clamp(inputs[F_INPUT].getVoltage()/10,0.0f,1.0f);
params[BPOTF_PARAM].setValue(fin);
				} else {
					fv = true;
					fin = clamp(params[POTR_PARAM].getValue()+link_delta,0.0f,1.0f);
params[BPOTF_PARAM].setValue(fin);
				}
			}
	}

if (inputs[IN_INPUT].isConnected()) {
	temp1 = inputs[IN_INPUT].getVoltage();

	for (int i=0;i<poles;i++){
		delta = temp1 - prevf1[i];
		delta = delta * fin;
		prevf1[i] = prevf1[i] + delta ;
		temp1= prevf1[i];
	}
	temp2=temp1;
	for (int i=0;i<poles;i++){
		delta = temp2 - prevf2[i];
		delta = delta * rin;
		prevf2[i] = prevf2[i] + delta ;
		temp2= prevf2[i];
	}
	outputs[OUT_OUTPUT].setVoltage(temp1 - temp2);

} else outputs[OUT_OUTPUT].setVoltage(0);


if (inputs[IN2_INPUT].isConnected()) {
	temp3 = inputs[IN2_INPUT].getVoltage();

	for (int i=0;i<poles;i++){
		delta = temp3 - prevf3[i];
		delta = delta * fin;
		prevf3[i] = prevf3[i] + delta ;
		temp3= prevf3[i];
	}
	temp4=temp3;
	for (int i=0;i<poles;i++){
		delta = temp4 - prevf4[i];
		delta = delta * rin;
		prevf4[i] = prevf4[i] + delta ;
		temp4= prevf4[i];
	}

	
	outputs[OUT2_OUTPUT].setVoltage(temp3 - temp4);

} else outputs[OUT2_OUTPUT].setVoltage(0);

};
};


struct CUTSWidget : ModuleWidget {
ParamWidget *bsfParam;
ParamWidget *bsrParam;
ParamWidget *sfParam;
ParamWidget *srParam;


	CUTSWidget(CUTS *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CUTS.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));


	addInput(createInput<PJ301MPort>(Vec(3, 308), module, CUTS::IN_INPUT));
	addInput(createInput<PJ301MPort>(Vec(3, 334), module, CUTS::IN2_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(32, 308), module, CUTS::OUT_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(32, 334), module, CUTS::OUT2_OUTPUT));

	addParam(createParam<RoundBlackSnapKnob>(Vec(15.5, 54), module, CUTS::POLE_PARAM));

	//addParam(createParam<LEDSliderWhite>(Vec(5, 131), module, CUTS::POTR_PARAM));
	srParam = createParam<LEDSliderWhite>(Vec(5, 131), module, CUTS::POTR_PARAM);
	addParam(srParam);
	addInput(createInput<PJ301MPort>(Vec(3, 252), module, CUTS::R_INPUT));
	//addParam(createParam<LEDSliderWhite>(Vec(35, 131), module, CUTS::POTF_PARAM));
	sfParam = createParam<LEDSliderWhite>(Vec(35, 131), module, CUTS::POTF_PARAM);
	addParam(sfParam);
	addInput(createInput<PJ301MPort>(Vec(32, 252), module, CUTS::F_INPUT));


	bsrParam = createParam<LEDSliderBlue>(Vec(5, 131), module, CUTS::BPOTR_PARAM);
	addParam(bsrParam);
	bsfParam = createParam<LEDSliderBlue>(Vec(35, 131), module, CUTS::BPOTF_PARAM);
	addParam(bsfParam);

     	addParam(createParam<LEDButton>(Vec(21.5, 104), module, CUTS::LINK_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(25.9, 108.4), module, CUTS::LINK_LIGHT));

}
	void step() override {
		CUTS *module = dynamic_cast<CUTS*>(this->module);

		if (module) {
			bsfParam->visible = module->fv;
			bsrParam->visible = module->rv;
			sfParam->visible = !module->fv;
			srParam->visible = !module->rv;
		}

		ModuleWidget::step();
	}
};

Model *modelCUTS = createModel<CUTS, CUTSWidget>("CUTS");
