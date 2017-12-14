#include "cf.hpp"
#include "dsp/digital.hpp"

struct EACH : Module {
	enum ParamIds {
		DIV_PARAM,
		BEAT_PARAM,
		ON_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		DOUZE_INPUT,
		START_INPUT,
		ON_INPUT,
		DIV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		DOUZE_OUTPUT,
		RESET_OUTPUT,
		BEAT_OUTPUT,
		START_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		ON_LIGHT,
		BEAT_LIGHT,
		NUM_LIGHTS
	};

int max_EACH = 3 ;
int stepa = 0 ;
int lum = 0 ;
int note = 0;
SchmittTrigger stTrigger;
SchmittTrigger dzTrigger;

	EACH() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};


void EACH::step() {
	if (!inputs[DIV_INPUT].active) max_EACH = floor(params[DIV_PARAM].value) ; else max_EACH = floor(inputs[DIV_INPUT].value * 1.2)+1 ;
	if (inputs[START_INPUT].active) {
		outputs[START_OUTPUT].value = inputs[START_INPUT].value;
		outputs[RESET_OUTPUT].value = inputs[START_INPUT].value;
		if (dzTrigger.process(inputs[START_INPUT].value)) stepa = max_EACH-1 ;
	}

	if (stTrigger.process(inputs[DOUZE_INPUT].value)) stepa = stepa +1 ;

	if (inputs[DOUZE_INPUT].active) {
		
		if (stepa == max_EACH) {
			note = 5;
			stepa = 0; 
			lum = 1000;
			}
		outputs[DOUZE_OUTPUT].value = inputs[DOUZE_INPUT].value;
	} 
	if (note >0) {outputs[BEAT_OUTPUT].value = 10.f;note = note -1;} else outputs[BEAT_OUTPUT].value = 0.f;
	if (lum>0) {lights[BEAT_LIGHT].value = true;lum = lum -1;} else lights[BEAT_LIGHT].value = false;
}

struct NuDisplayWidget : TransparentWidget {

  int *value;
  std::shared_ptr<Font> font;

  NuDisplayWidget() {
    font = Font::load(assetPlugin(plugin, "res/Segment7Standard.ttf"));
  };

  void draw(NVGcontext *vg) {
    // Background
    NVGcolor backgroundColor = nvgRGB(0x44, 0x44, 0x44);
    NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 0.0, 0.0, box.size.x, box.size.y, 4.0);
    nvgFillColor(vg, backgroundColor);
    nvgFill(vg);
    nvgStrokeWidth(vg, 1.0);
    nvgStrokeColor(vg, borderColor);
    nvgStroke(vg);

    nvgFontSize(vg, 18);
    nvgFontFaceId(vg, font->handle);
    nvgTextLetterSpacing(vg, 2.5);

    std::string to_display = std::to_string(*value);


    while(to_display.length()<3) to_display = ' ' + to_display;

    Vec textPos = Vec(6.0f, 17.0f);

    NVGcolor textColor = nvgRGB(0xdf, 0xd2, 0x2c);
    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "~~~", NULL);

    textColor = nvgRGB(0xda, 0xe9, 0x29);

    nvgFillColor(vg, nvgTransRGBA(textColor, 16));
    nvgText(vg, textPos.x, textPos.y, "\\\\\\", NULL);


    textColor = nvgRGB(0x28, 0xb0, 0xf3);
    nvgFillColor(vg, textColor);
    nvgText(vg, textPos.x, textPos.y, to_display.c_str(), NULL);
  }
};

EACHWidget::EACHWidget() {
	EACH *module = new EACH();
	setModule(module);
	box.size = Vec(15*6, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/EACH.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 365)));

	addInput(createInput<PJ301MPort>(Vec(11, 26), module, EACH::START_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(35, 275), module, EACH::RESET_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(11, 321), module, EACH::START_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(54, 26), module, EACH::DOUZE_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 321), module, EACH::DOUZE_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(35, 235), module, EACH::BEAT_OUTPUT));

	addParam(createParam<RoundBlackKnob>(Vec(27, 107), module, EACH::DIV_PARAM, 1.0, 12.1, 3.1));
	addInput(createInput<PJ301MPort>(Vec(11, 141), module, EACH::DIV_INPUT));

     	addParam(createParam<LEDButton>(Vec(38, 197), module, EACH::BEAT_PARAM, 0.0, 1.0, 0.0));
	addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 201.4), module, EACH::BEAT_LIGHT));
	
	
	NuDisplayWidget *display = new NuDisplayWidget();
	display->box.pos = Vec(20,56);
	display->box.size = Vec(50, 20);
	display->value = &module->max_EACH;
	addChild(display);

	
}
