#include "plugin.hpp"


struct EACH : Module {
	enum ParamIds {
		DIV_PARAM,
		BEAT_PARAM,
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
dsp::SchmittTrigger stTrigger;
dsp::SchmittTrigger dzTrigger;
float or_gain =0.0;
int or_affi =0;


	EACH() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(BEAT_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(DIV_PARAM, 1.0f, 48.1f, 3.1f, "Divisions");
}


void process(const ProcessArgs &args) override {
	if (!inputs[DIV_INPUT].isConnected()) {
		max_EACH = floor(params[DIV_PARAM].getValue());
		or_affi=0; or_gain = max_EACH/4.8;
	} else {
		max_EACH = round(clamp((inputs[DIV_INPUT].getVoltage() * 4.8)+1,1.0f,48.0f));
		or_gain = (clamp(inputs[DIV_INPUT].getVoltage(),0.0f,10.0f));
		or_affi=1;
	}

	if (inputs[START_INPUT].isConnected()) {
		outputs[START_OUTPUT].setVoltage(inputs[START_INPUT].getVoltage());
		outputs[RESET_OUTPUT].setVoltage(inputs[START_INPUT].getVoltage());
		if (dzTrigger.process(inputs[START_INPUT].getVoltage())) stepa = max_EACH-1 ;
	}

	if (stTrigger.process(inputs[DOUZE_INPUT].getVoltage())) stepa = stepa +1 ;

	if (inputs[DOUZE_INPUT].isConnected()) {
		
		if (stepa == max_EACH) {
			note = 50;
			stepa = 0; 
			lum = 2000;
			}
		outputs[DOUZE_OUTPUT].setVoltage(inputs[DOUZE_INPUT].getVoltage());
	} 
	if (note >0) {outputs[BEAT_OUTPUT].setVoltage(10.f);note = note -1;} else outputs[BEAT_OUTPUT].setVoltage(0.f);
	if (lum>0) {lights[BEAT_LIGHT].setBrightness(1);lum = lum -1;} else lights[BEAT_LIGHT].setBrightness(0);
};

};

struct NuDisplayWidget : TransparentWidget {
	EACH *module;

  std::shared_ptr<Font> font;

  NuDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

  void draw(const DrawArgs &args) override {
nvgGlobalTint(args.vg, color::WHITE);
	int val = module ? module->max_EACH : 3;

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

    std::string to_display = std::to_string(val);


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
  }
};

struct MOTORPOTDisplay : TransparentWidget {
	EACH *module;


	MOTORPOTDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {
//nvgGlobalTint(args.vg, color::WHITE);
float gainX = module ? module->or_gain : 1.0f;
//int affich = module ? module->or_affi : 0;
float d=18;

		//if (affich==1) {
		float xx = d*sin(-(gainX*0.17+0.15)*M_PI) ;
		float yy = d*cos((gainX*0.17+0.15)*M_PI) ;

		
			//nvgBeginPath(args.vg);
			//nvgCircle(args.vg, 0,0, d);
			//nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
			//nvgFill(args.vg);	
		
			nvgStrokeWidth(args.vg,2);
			nvgStrokeColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0x88));
			{
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, 0,0);
				nvgLineTo(args.vg, xx,yy);
				nvgClosePath(args.vg);
			}
			nvgStroke(args.vg);
		//}

	}
};


struct EACHWidget : ModuleWidget {
	EACHWidget(EACH *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EACH.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addInput(createInput<PJ301MPort>(Vec(11, 26), module, EACH::START_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(35, 275), module, EACH::RESET_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(11, 321), module, EACH::START_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(54, 26), module, EACH::DOUZE_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 321), module, EACH::DOUZE_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(35, 235), module, EACH::BEAT_OUTPUT));

	addParam(createParam<cfBigKnob>(Vec(27, 107), module, EACH::DIV_PARAM));
	addInput(createInput<PJ301MPort>(Vec(11, 141), module, EACH::DIV_INPUT));
	{
		MOTORPOTDisplay *display = new MOTORPOTDisplay();
		display->box.pos = Vec(45, 125);
		display->module = module;
		addChild(display);
	}

     	addParam(createParam<LEDButton>(Vec(38, 197), module, EACH::BEAT_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 201.4), module, EACH::BEAT_LIGHT));
	
	
	NuDisplayWidget *display = new NuDisplayWidget();
	display->box.pos = Vec(20,56);
	display->box.size = Vec(50, 20);
	display->module = module;
	addChild(display);

	
}
};

Model *modelEACH = createModel<EACH, EACHWidget>("EACH");
