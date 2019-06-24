#include "plugin.hpp"
#include "dsp/digital.hpp"


struct SLIDERSEQ : Module {
	enum ParamIds {
		OFFSET_PARAM,
		LVL_PARAM,
		ON_PARAM = LVL_PARAM +16,
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
		configParam(OFFSET_PARAM, 0.f, 1.f, 0.f);
		for (int i = 0; i < 16; i++) {
			configParam(LVL_PARAM + i, 0.f, 1.f, 0.f);
		}
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

	
if (!inputs[POS_INPUT].active) {
	if (rstTrigger.process(inputs[RST_INPUT].value))
			{
			pas = -1;
			}
	if (upTrigger.process(inputs[UP_INPUT].value))
			{
				if (pas <15) pas = pas+1; else pas =0;
			}


} else { pas = int(inputs[POS_INPUT].value*1.6);
	if (pas<0) pas =0;
	if (pas>15) pas =15;
};

	if (offsetTrigger.process(params[OFFSET_PARAM].value))
			{if (OFFSET_STATE == 0) OFFSET_STATE = 1; else OFFSET_STATE = 0;}

	if (OFFSET_STATE==1) lights[OFFSET_LIGHT].value=true; else lights[OFFSET_LIGHT].value=false;


if (pas>-1) {
	sl_pas=pas; sl_value = params[LVL_PARAM +pas].value ;
	outputs[TR_OUTPUT].value=params[LVL_PARAM +pas].value*10-OFFSET_STATE*5.0;}
  else {
	sl_pas=0; sl_value = params[LVL_PARAM +0].value ;
	outputs[TR_OUTPUT].value=params[LVL_PARAM +0].value*10-OFFSET_STATE*5.0;}

};
};

struct SLDisplay : TransparentWidget {
	SLIDERSEQ *module;


	SLDisplay() {
		
	};
	
	void draw(const DrawArgs &args) override {
		float pp = module ? module->sl_pas : 0;
		float vv = module ? module->sl_value : 1;
		float xx = (int(pp)%8) ;
		float yy = int(pp/8)-vv/2 ;

		
			nvgBeginPath(args.vg);
			nvgRect(args.vg, xx*15,65+yy*125, 4.5,10.5);
			nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0xff));
			nvgFill(args.vg);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, xx*15-3,65+yy*125-3, 10.5,16.5,2.0);
			nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0x30));
			nvgFill(args.vg);		
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, xx*15-6,65+yy*125-6, 16.5,22.5,4.0);
			nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0x10));
			nvgFill(args.vg);

	}
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
		addParam(createParam<LEDSliderWhite>(Vec(4+i*15, 30+ 30), module, SLIDERSEQ::LVL_PARAM+i));
		
	}
	for (int i = 8; i < 16; i++) {
		addParam(createParam<LEDSliderWhite>(Vec(4+(i-8)*15, 30+ 155), module, SLIDERSEQ::LVL_PARAM+i));
		
	}

		{
		SLDisplay *pdisplay = new SLDisplay();
		pdisplay->box.pos = Vec(12, 61);
		pdisplay->module = module;
		//pdisplay->pp = &module->sl_pas;
		//pdisplay->vv = &module->sl_value;
		addChild(pdisplay);
	}
	addInput(createInput<PJ301MPort>(Vec(68, 320), module, SLIDERSEQ::POS_INPUT));
};
};

Model *modelSLIDERSEQ = createModel<SLIDERSEQ, SLIDERSEQWidget>("SLIDERSEQ");

