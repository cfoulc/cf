#include "plugin.hpp"





using namespace std;


struct VARIABLE : Module {
	enum ParamIds {
		PREV_PARAM,
		NEXT_PARAM,
		HOLD_PARAM,
		NUM_PARAMS 
	};
	enum InputIds {
		IN_INPUT,
		TRIG_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		HOLD_LIGHT,
		NUM_LIGHTS
	};
	

	bool lock = false ;
	bool plugged = false ;
	float value = 0;
	dsp::SchmittTrigger trigTrigger;
	dsp::SchmittTrigger holdTrigger;
	dsp::SchmittTrigger nextTrigger;
	dsp::SchmittTrigger prevTrigger;

VARIABLE(){ 
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	configParam(PREV_PARAM, 0.f, 1.f, 0.f);
	configParam(NEXT_PARAM, 0.f, 1.f, 0.f);
	configParam(HOLD_PARAM, 0.f, 1.f, 0.f);
}


	
json_t *dataToJson() override {
		json_t *rootJ = json_object();
		
		json_object_set_new(rootJ, "loc", json_integer(lock));
		json_object_set_new(rootJ, "plu", json_integer(plugged));
		json_object_set_new(rootJ, "val", json_real(value));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		
		json_t *locJ = json_object_get(rootJ, "loc");
		if (locJ)
			lock = json_integer_value(locJ);

		json_t *pluJ = json_object_get(rootJ, "plu");
		if (pluJ)
			plugged = json_integer_value(pluJ);

		json_t *valJ = json_object_get(rootJ, "val");
		if (valJ)
			value = json_real_value(valJ);
	
	}




void process(const ProcessArgs &args) override {

	if (inputs[IN_INPUT].isConnected() & !plugged) {plugged = true; lock = false;}
	if (!inputs[IN_INPUT].isConnected()) {plugged = false;}

	if (inputs[IN_INPUT].isConnected() & !lock) value = inputs[IN_INPUT].getVoltage();


		if ( ( holdTrigger.process(params[HOLD_PARAM].getValue()) or trigTrigger.process(inputs[TRIG_INPUT].getVoltage()) ) & inputs[IN_INPUT].isConnected()) 
			{
			value = inputs[IN_INPUT].getVoltage();
			lock = true;
			}
	

		if (nextTrigger.process(params[NEXT_PARAM].getValue()))
			{
			if ((value<0)&(value!=int(value))) value = int(value) ; else value = int(value+1);
			}
				
			
		if (prevTrigger.process(params[PREV_PARAM].getValue()))
			{
			if ((value>=0)&(value!=int(value))) value = int(value) ; else value = int(value-1);
			} 

		
	lights[HOLD_LIGHT].value = lock ;
	outputs[OUT_OUTPUT].setVoltage(value) ;
		
}
};
struct upButton : app::SvgSwitch {
	upButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/upButton.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/upButtonDown.svg")));
	}
};
struct downButton : app::SvgSwitch {
	downButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/downButton.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/downButtonDown.svg")));
	};
};

struct VARIABLEDisplay : TransparentWidget {
	VARIABLE *module;

	int frame = 0;
	std::shared_ptr<Font> font;

	VARIABLEDisplay() {
    		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/LEDCalculator.ttf"));
	};
	
	void draw(const DrawArgs &args) override {
float val = module ? module->value : 0;
		std::string to_display = "";
		std::string fileDesc = "";
		if (val>=0) 
			fileDesc = "+" + std::to_string(val); else fileDesc = std::to_string(val);
		for (int i=0; i<9; i++) to_display = to_display + fileDesc[i];
		nvgFontSize(args.vg, 24);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 0);
		nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0xff));
		nvgRotate(args.vg, -M_PI / 2.0f);	
		nvgTextBox(args.vg, 5, 5,350, to_display.c_str(), NULL);
	}
};


struct VARIABLEWidget : ModuleWidget {
	VARIABLEWidget(VARIABLE *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VARIABLE.svg")));


	
	{
		VARIABLEDisplay *gdisplay = new VARIABLEDisplay();
		gdisplay->box.pos = Vec(18, 268);
		gdisplay->box.size = Vec(130, 250);
		gdisplay->module = module;
		addChild(gdisplay);
	}


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addInput(createInput<PJ301MPort>(Vec(3, 31), module, VARIABLE::IN_INPUT));
	addInput(createInput<PJ301MPort>(Vec(3, 96), module, VARIABLE::TRIG_INPUT));
	addParam(createParam<LEDButton>(Vec(6, 66+3), module, VARIABLE::HOLD_PARAM));
		addChild(createLight<MediumLight<BlueLight>>(Vec(6+4.4, 69+4.4), module, VARIABLE::HOLD_LIGHT));

	addOutput(createOutput<PJ301MPort>(Vec(3, 321), module, VARIABLE::OUT_OUTPUT));

	addParam(createParam<upButton>(Vec(6, 296+2), module, VARIABLE::PREV_PARAM));
	addParam(createParam<downButton>(Vec(6, 276+2), module, VARIABLE::NEXT_PARAM));
}
};

Model *modelVARIABLE = createModel<VARIABLE, VARIABLEWidget>("VARIABLE");
