#include "plugin.hpp"


struct METRO : Module {
	enum ParamIds {
		BPM_PARAM,
		RST_PARAM,
		BEAT_PARAM,
		ON_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ON_INPUT,
		BPM_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		MES_OUTPUT,
		BEAT_OUTPUT,
		START_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		ON_LIGHT,
		MES_LIGHT,
		BEAT_LIGHT,
		NUM_LIGHTS
	};

int max_METRO = 120 ;
int beatl = 0 ;
int mesl = 0 ;
int beats = 0 ;
int mess = 0 ;
int strt = 0 ;
int note ;
float toc_phase = 0.f;
uint32_t toc = 0u;
dsp::SchmittTrigger onTrigger;
dsp::SchmittTrigger oninTrigger;
dsp::SchmittTrigger rstTrigger;
bool ON_STATE = false;
float or_gain ;
int or_affi ;

	METRO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(BEAT_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(RST_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(ON_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(BPM_PARAM, 0.0f, 301.0f, 120.1f, "BPM");
onReset();
}



void onReset() override {
			ON_STATE = true;
			
			}

json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// on
		json_object_set_new(rootJ, "onstate", json_integer(ON_STATE));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {

		// on
		json_t *onstateJ = json_object_get(rootJ, "onstate");
		if (onstateJ)
			ON_STATE = json_integer_value(onstateJ);
	
	}



void process(const ProcessArgs &args) override {

	if (!inputs[BPM_INPUT].isConnected()) {
		max_METRO = floor(params[BPM_PARAM].getValue());
		or_affi = 0;
		or_gain = max_METRO/30.0;
	} else {
		max_METRO = round(clamp(inputs[BPM_INPUT].getVoltage() *30,0.0f,300.0f));
		or_affi = 1;
		or_gain = max_METRO/30.0;
	}

	float bpm = max_METRO ;
	bool toced = false;

	if (onTrigger.process(params[ON_PARAM].getValue())+oninTrigger.process(inputs[ON_INPUT].getVoltage()))
			{if (ON_STATE == 0) {ON_STATE = 1; strt = 5;} else ON_STATE = 0;}

	lights[ON_LIGHT].setBrightness(ON_STATE) ;

	
	if (rstTrigger.process(params[RST_PARAM].getValue()))
		{toc = 47u;toc_phase = 1.f; strt = 5;}
	

	if (ON_STATE) {
		toc_phase += ((bpm / 60.f) * args.sampleTime) * 12.f;
		
		if(toc_phase >= 1.f) {
			toced = true;
			toc = (toc+1u) % 48u ;
			toc_phase -= 1.f;
			}

		if(toced) {
			if (toc % 12u ? 0 : 1) beatl = 4000;
			if (toc % 48u ? 0 : 1) mesl = 5000;
			if (toc % 12u ? 0 : 1) beats = 200;
			if (toc % 48u ? 0 : 1) mess = 200;
			note =5;
			}

		if (beatl>0) {
			lights[BEAT_LIGHT].setBrightness(1); 
			beatl = beatl -1;
		} else {
			lights[BEAT_LIGHT].setBrightness(0);
			}

		if (mesl>0) {
			lights[MES_LIGHT].setBrightness(1); 
			mesl = mesl -1;
		} else {
			lights[MES_LIGHT].setBrightness(0);
			}


		if (beats>0) {
			beats = beats -1;
			outputs[BEAT_OUTPUT].setVoltage(2.5 * (beats- 150*round(beats/150))/150);
		} else {
			outputs[BEAT_OUTPUT].setVoltage(0.0);
			}

		if (mess>0) { 
			mess = mess -1;
			outputs[BEAT_OUTPUT].setVoltage(5.0 * (mess- 150*round(mess/150))/150);
			}

	} else {toc = 47u;toc_phase = 1.f;outputs[OUT_OUTPUT].setVoltage(0.f);}
    

	if (strt > 0) {outputs[START_OUTPUT].setVoltage(10.f);strt=strt-1;} else outputs[START_OUTPUT].setVoltage(0.f);
	if (note > 0) {outputs[OUT_OUTPUT].setVoltage(10.f);note=note-1;} else outputs[OUT_OUTPUT].setVoltage(0.f);
}
};

struct NumDisplayWidget : TransparentWidget {
	METRO *module;

  std::shared_ptr<Font> font;

  NumDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

	void drawLayer(const DrawArgs &args, int layer) override {
if (layer ==1) {

	int val = module ? module->max_METRO : 120;
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
Widget::drawLayer(args, layer);
}
};

struct METROPOTDisplay : TransparentWidget {
	METRO *module;

	METROPOTDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {

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


struct METROWidget : ModuleWidget {
	METROWidget(METRO *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/METRO.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addParam(createParam<cfBigKnob>(Vec(27, 107), module, METRO::BPM_PARAM));
	addInput(createInput<PJ301MPort>(Vec(11, 141), module, METRO::BPM_INPUT));
	{
		METROPOTDisplay *display = new METROPOTDisplay();
		display->box.pos = Vec(45, 125);
		display->module = module;
		addChild(display);
	}

     	addParam(createParam<LEDButton>(Vec(38, 167), module, METRO::ON_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 171.4), module, METRO::ON_LIGHT));
	addInput(createInput<PJ301MPort>(Vec(11, 171), module, METRO::ON_INPUT));

     	addParam(createParam<LEDButton>(Vec(38, 197), module, METRO::RST_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 201.4), module, METRO::MES_LIGHT));

     	addParam(createParam<LEDButton>(Vec(38, 227), module, METRO::BEAT_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 231.4), module, METRO::BEAT_LIGHT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 265), module, METRO::BEAT_OUTPUT));

	addOutput(createOutput<PJ301MPort>(Vec(11, 321), module, METRO::START_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 321), module, METRO::OUT_OUTPUT));

	NumDisplayWidget *display = new NumDisplayWidget();
	display->box.pos = Vec(20,56);
	display->box.size = Vec(50, 20);
	display->module = module;
	addChild(display);

	
}
};

Model *modelMETRO = createModel <METRO, METROWidget>("METRO");