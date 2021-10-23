#include "plugin.hpp"



using namespace std;


struct ALGEBRA : Module {
	enum ParamIds {
		ENUMS(OP_PARAM, 6),
		NUM_PARAMS
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
		NUM_LIGHTS
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
		if (trTrigger[i].process(params[OP_PARAM+i].getValue())) OP_STATE= i;
	}

	if (OP_STATE==0) outputs[OUT_OUTPUT].setVoltage(inputs[IN1_INPUT].getVoltage() + inputs[IN2_INPUT].getVoltage());
	if (OP_STATE==1) outputs[OUT_OUTPUT].setVoltage(inputs[IN1_INPUT].getVoltage() - inputs[IN2_INPUT].getVoltage());
	if (OP_STATE==2) outputs[OUT_OUTPUT].setVoltage(inputs[IN1_INPUT].getVoltage() * inputs[IN2_INPUT].getVoltage());
	if ((OP_STATE==3) & (inputs[IN2_INPUT].getVoltage()!=0)) outputs[OUT_OUTPUT].setVoltage(inputs[IN1_INPUT].getVoltage() / inputs[IN2_INPUT].getVoltage());
	if (OP_STATE==4) {
			if (inputs[IN1_INPUT].getVoltage()>=inputs[IN2_INPUT].getVoltage())	outputs[OUT_OUTPUT].setVoltage(inputs[IN1_INPUT].getVoltage());
				else outputs[OUT_OUTPUT].setVoltage(inputs[IN2_INPUT].getVoltage());
			}
	if (OP_STATE==5) {
			if (inputs[IN1_INPUT].getVoltage()<=inputs[IN2_INPUT].getVoltage())	outputs[OUT_OUTPUT].setVoltage(inputs[IN1_INPUT].getVoltage());
				else outputs[OUT_OUTPUT].setVoltage(inputs[IN2_INPUT].getVoltage());
			}

}
};

struct plusButton : app::SvgSwitch {
	plusButton() {
		momentary = true;
		shadow->opacity = 0;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/plusButton.svg")));
	}
};


struct ALGDisplay : TransparentWidget {
	ALGEBRA *module;
		std::string fileDesc = "+";
int numero =0;
	int frame = 0;
	std::shared_ptr<Font> font;

	ALGDisplay() {
    		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/ArialBlack.ttf"));
	};

void draw(const DrawArgs &args) override {

		nvgFontSize(args.vg, 20);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 0);
		nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));	
		nvgTextBox(args.vg, 5, 5,350, fileDesc.c_str(), NULL);
}

void drawLayer(const DrawArgs &args, int layer) override {
	if (layer ==1) {
		float val = module ? module->OP_STATE : 0;

		if (val==numero){
			nvgFontSize(args.vg, 20);
			nvgFontFaceId(args.vg, font->handle);
			nvgTextLetterSpacing(args.vg, 0);
			nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0xff));	
			nvgTextBox(args.vg, 5, 5,350, fileDesc.c_str(), NULL);
		}
	}
	Widget::drawLayer(args, layer);
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
     		addParam(createParam<plusButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
    		addParam(createParam<plusButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
     		addParam(createParam<plusButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
     		addParam(createParam<plusButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
     		addParam(createParam<plusButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i)); i=i+1;
     		addParam(createParam<plusButton>(Vec(6, i*24+136), module, ALGEBRA::OP_PARAM + i));
		
	


	addOutput(createOutput<PJ301MPort>(Vec(3, 321), module, ALGEBRA::OUT_OUTPUT));

	{
		ALGDisplay *plusdisplay = new ALGDisplay();
		plusdisplay->box.pos = Vec(6, 145);
		plusdisplay->module = module;
		plusdisplay->fileDesc = "+";
		plusdisplay->numero = 0;
		addChild(plusdisplay);
	}
	{
		ALGDisplay *moinsdisplay = new ALGDisplay();
		moinsdisplay->box.pos = Vec(9, 145+24-1);
		moinsdisplay->module = module;
		moinsdisplay->fileDesc = "-";
		moinsdisplay->numero = 1;
		addChild(moinsdisplay);
	}
	{
		ALGDisplay *foisdisplay = new ALGDisplay();
		foisdisplay->box.pos = Vec(6, 145+24*2-1);
		foisdisplay->module = module;
		foisdisplay->fileDesc = "x";
		foisdisplay->numero = 2;
		addChild(foisdisplay);
	}
	{
		ALGDisplay *divdisplay = new ALGDisplay();
		divdisplay->box.pos = Vec(9, 145+24*3);
		divdisplay->module = module;
		divdisplay->fileDesc = "/";
		divdisplay->numero = 3;
		addChild(divdisplay);
	}
	{
		ALGDisplay *maxdisplay = new ALGDisplay();
		maxdisplay->box.pos = Vec(4, 145+24*4);
		maxdisplay->module = module;
		maxdisplay->fileDesc = "M";
		maxdisplay->numero = 4;
		addChild(maxdisplay);
	}
	{
		ALGDisplay *mindisplay = new ALGDisplay();
		mindisplay->box.pos = Vec(4, 145+24*5-1);
		mindisplay->module = module;
		mindisplay->fileDesc = "m";
		mindisplay->numero = 5;
		addChild(mindisplay);
	}
	
}
};


Model *modelALGEBRA = createModel<ALGEBRA, ALGEBRAWidget>("ALGEBRA");
