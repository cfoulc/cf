#include "plugin.hpp"
#include "dsp/digital.hpp"


using namespace std;


struct ALGEBRA : Module {
	enum ParamIds {
		OP_PARAM,
		NUM_PARAMS = OP_PARAM+6
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		LED_LIGHT,
		NUM_LIGHTS = LED_LIGHT+6
	};
	
	int OP_STATE = 0 ;
	dsp::SchmittTrigger trTrigger[6];

ALGEBRA() { 
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	for (int i = 0; i < 6; i++) {
			configParam(OP_PARAM + i, 0.f, 1.f, 0.f);
		}
}

	
json_t *dataToJson() override {
		json_t *rootJ = json_object();
		

		json_object_set_new(rootJ, "opstate", json_integer(OP_STATE));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		

		json_t *opstateJ = json_object_get(rootJ, "opstate");
		if (opstateJ)
			OP_STATE = json_integer_value(opstateJ);
	
	}





void process(const ProcessArgs &args) override {
	for (int i=0; i<6; i++) {
		if (trTrigger[i].process(params[OP_PARAM+i].value)) OP_STATE= i;
		if (OP_STATE == i) lights[LED_LIGHT+i].value=1; else lights[LED_LIGHT+i].value=0;
	}
	if (OP_STATE==0) outputs[OUT_OUTPUT].value = inputs[IN1_INPUT].value + inputs[IN2_INPUT].value;
	if (OP_STATE==1) outputs[OUT_OUTPUT].value = inputs[IN1_INPUT].value - inputs[IN2_INPUT].value;
	if (OP_STATE==2) outputs[OUT_OUTPUT].value = inputs[IN1_INPUT].value * inputs[IN2_INPUT].value;
	if ((OP_STATE==3) & (inputs[IN2_INPUT].value!=0)) outputs[OUT_OUTPUT].value = inputs[IN1_INPUT].value / inputs[IN2_INPUT].value;
	if (OP_STATE==4) {
			if (inputs[IN1_INPUT].value>=inputs[IN2_INPUT].value)	outputs[OUT_OUTPUT].value = inputs[IN1_INPUT].value;
				else outputs[OUT_OUTPUT].value = inputs[IN2_INPUT].value;
			}
	if (OP_STATE==5) {
			if (inputs[IN1_INPUT].value<=inputs[IN2_INPUT].value)	outputs[OUT_OUTPUT].value = inputs[IN1_INPUT].value;
				else outputs[OUT_OUTPUT].value = inputs[IN2_INPUT].value;
			}

}
};
struct plusButton : app::SvgSwitch {
	plusButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/plusButton.svg")));
	}
};
struct minusButton : app::SvgSwitch {
	minusButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/minusButton.svg")));
	}
};
struct multButton : app::SvgSwitch {
	multButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/multButton.svg")));	}
};
struct divButton : app::SvgSwitch {
	divButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/divButton.svg")));
	}
};
struct maxButton : app::SvgSwitch {
	maxButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/maxButton.svg")));
	}
};
struct minButton : app::SvgSwitch {
	minButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/minButton.svg")));
	}
};

struct ALGEBRAWidget : ModuleWidget {
	ALGEBRAWidget(ALGEBRA *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ALGEBRA.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));


	addInput(createInput<PJ301MPort>(Vec(3, 31), module, ALGEBRA::IN1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(3, 95), module, ALGEBRA::IN2_INPUT));

	int i = 0;
		addChild(createLight<LargeLight<BlueLight>>(Vec(3+4.4, i*24+133+4.4), module, ALGEBRA::LED_LIGHT + i));
     		addParam(createParam<plusButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
		addChild(createLight<LargeLight<BlueLight>>(Vec(3+4.4, i*24+133+4.4), module, ALGEBRA::LED_LIGHT + i));
     		addParam(createParam<minusButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
		addChild(createLight<LargeLight<BlueLight>>(Vec(3+4.4, i*24+133+4.4), module, ALGEBRA::LED_LIGHT + i));
     		addParam(createParam<multButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
		addChild(createLight<LargeLight<BlueLight>>(Vec(3+4.4, i*24+133+4.4), module, ALGEBRA::LED_LIGHT + i));
     		addParam(createParam<divButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
		addChild(createLight<LargeLight<BlueLight>>(Vec(3+4.4, i*24+133+4.4), module, ALGEBRA::LED_LIGHT + i));
     		addParam(createParam<maxButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
		addChild(createLight<LargeLight<BlueLight>>(Vec(3+4.4, i*24+133+4.4), module, ALGEBRA::LED_LIGHT + i));
     		addParam(createParam<minButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i));
		
	


	addOutput(createOutput<PJ301MPort>(Vec(3, 321), module, ALGEBRA::OUT_OUTPUT));
	
}
};


Model *modelALGEBRA = createModel<ALGEBRA, ALGEBRAWidget>("ALGEBRA");
