#include "plugin.hpp"

struct STEPS : Module {
	enum ParamIds {
		LEVEL1_PARAM,
		TRIM1_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		LIN1_INPUT,
		IN1_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		NUM_OUTPUTS
	};

float max_steps = 8 ;

	STEPS() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LEVEL1_PARAM, 1.0f, 32.0f, 8.1f, "Steps");
		configParam(TRIM1_PARAM, -10.0f, 10.0f, 0.0f, "Trim");
}

void process(const ProcessArgs &args) override {


if (inputs[LIN1_INPUT].isConnected()) 
	{
	max_steps = round(clamp(params[LEVEL1_PARAM].getValue() + inputs[LIN1_INPUT].getVoltage()*0.32*params[TRIM1_PARAM].getValue(),1.0f,32.0f));
	outputs[OUT1_OUTPUT].setVoltage(floor((inputs[IN1_INPUT].getVoltage() * round(clamp(params[LEVEL1_PARAM].getValue() + inputs[LIN1_INPUT].getVoltage()*0.32*params[TRIM1_PARAM].getValue(),1.0f,32.0f))) / 10.01) * (10 / round(clamp(params[LEVEL1_PARAM].getValue() + inputs[LIN1_INPUT].getVoltage()*0.32*params[TRIM1_PARAM].getValue(),1.0f,32.0f)))) ;
	} 
	else 
	{
	max_steps = round(params[LEVEL1_PARAM].getValue());
	outputs[OUT1_OUTPUT].setVoltage(floor((inputs[IN1_INPUT].getVoltage() * round(params[LEVEL1_PARAM].getValue())) / 10.01) * (10 / round(params[LEVEL1_PARAM].getValue()))); 
	}
};


};

struct NumbeDisplayWidget : TransparentWidget {
	STEPS *module;

  std::shared_ptr<Font> font;

  NumbeDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

  void draw(const DrawArgs &args) override {
nvgGlobalTint(args.vg, color::WHITE);
int st = module ? module->max_steps : 0;
    // Background
    NVGcolor backgroundColor = nvgRGB(0x44, 0x44, 0x44);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(args.vg);
    nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(args.vg, backgroundColor);
    nvgFill(args.vg);
    nvgStrokeWidth(args.vg, 1.0);
    nvgStrokeColor(args.vg, borderColor);
    nvgStroke(args.vg);

    nvgFontSize(args.vg, 18);
    nvgFontFaceId(args.vg, font->handle);
    nvgTextLetterSpacing(args.vg, 2.5);

 std::string to_display = std::to_string(st);

//char d_string[10];
// if(st) sprintf(d_string,"%6i",st);

  while(to_display.length()<3) to_display = ' ' + to_display;

    Vec textPos = Vec(6.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);

    nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
    nvgText(args.vg, textPos.x, textPos.y, "\\\\\\", NULL);


    textColor = nvgRGB(0x28, 0xb0, 0xf3);
    nvgFillColor(args.vg, textColor);
    nvgText(args.vg, textPos.x, textPos.y, to_display.c_str(), NULL);
//nvgText(args.vg, textPos.x, textPos.y, d_string, NULL);
  }
};

struct STEPSWidget : ModuleWidget {
	STEPSWidget(STEPS *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/STEPS.svg")));


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addParam(createParam<RoundLargeBlackKnob>(Vec(27, 157), module, STEPS::LEVEL1_PARAM));

	addParam(createParam<Trimpot>(Vec(37, 207), module, STEPS::TRIM1_PARAM));

	addInput(createInput<PJ301MPort>(Vec(34, 250), module, STEPS::LIN1_INPUT));

	addInput(createInput<PJ301MPort>(Vec(11, 321), module, STEPS::IN1_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(54, 321), module, STEPS::OUT1_OUTPUT));


NumbeDisplayWidget *display = new NumbeDisplayWidget();
	display->box.pos = Vec(20,56);
	display->box.size = Vec(50, 20);
	display->module = module;
	addChild(display);
	
}
};

Model *modelSTEPS = createModel<STEPS, STEPSWidget>("STEPS");
