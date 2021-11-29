#include "plugin.hpp"



using namespace std;


struct FUNKTION : Module {
	enum ParamIds {
		PREV_PARAM,
		NEXT_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ERROR_LIGHT,
		PERROR_LIGHT,
		OERROR_LIGHT,
		NUM_LIGHTS
	};
	
	std::string fctDesc;
	int OP_STATE = 0 ;
	int perror = 0 ;
	int oerror = 0 ;
	dsp::SchmittTrigger nextTrigger;
	dsp::SchmittTrigger prevTrigger;


FUNKTION() { 
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	configButton(NEXT_PARAM, "Next function");
	configButton(PREV_PARAM, "Previous function");
		configInput(IN1_INPUT,"Signal");
		configOutput(OUT_OUTPUT,"Result");
	configLight(ERROR_LIGHT, "Function input error");
	configLight(PERROR_LIGHT, "Function input error");
	configLight(OERROR_LIGHT, "Function input error");
		
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
	
	if (nextTrigger.process(params[NEXT_PARAM].getValue()) & (OP_STATE<20)) OP_STATE+= 1;
	if (prevTrigger.process(params[PREV_PARAM].getValue()) & (OP_STATE>0)) OP_STATE-= 1;
		
	float val ;

	if (OP_STATE==0) {val = abs(inputs[IN1_INPUT].getVoltage());fctDesc="abs";}

	if (OP_STATE==1) {val = round(inputs[IN1_INPUT].getVoltage());fctDesc="round";}
	if (OP_STATE==2) {val = ceil(inputs[IN1_INPUT].getVoltage());fctDesc="ceil";}
	if (OP_STATE==3) {val = int(inputs[IN1_INPUT].getVoltage());fctDesc="floor";}

	if (OP_STATE==4) {val = pow(inputs[IN1_INPUT].getVoltage(), 2.0);fctDesc="pow2";}
	if (OP_STATE==5) {val = sqrt(inputs[IN1_INPUT].getVoltage());fctDesc="sqrt";}

	if (OP_STATE==6) {val = exp(inputs[IN1_INPUT].getVoltage());fctDesc="exp";}
	if (OP_STATE==7) {val = log(inputs[IN1_INPUT].getVoltage());fctDesc="log";}
	if (OP_STATE==8) {val = log10(inputs[IN1_INPUT].getVoltage());fctDesc="log10";}

	if (OP_STATE==9) {val = cos(inputs[IN1_INPUT].getVoltage());fctDesc="cos";}
	if (OP_STATE==10) {val = sin(inputs[IN1_INPUT].getVoltage());fctDesc="sin";}
	if (OP_STATE==11) {val = tan(inputs[IN1_INPUT].getVoltage());fctDesc="tan";}
	if (OP_STATE==12) {val = acos(inputs[IN1_INPUT].getVoltage());fctDesc="acos";}
	if (OP_STATE==13) {val = asin(inputs[IN1_INPUT].getVoltage());fctDesc="asin";}
	if (OP_STATE==14) {val = atan(inputs[IN1_INPUT].getVoltage());fctDesc="atan";}

	if (OP_STATE==15) {val = cosh(inputs[IN1_INPUT].getVoltage());fctDesc="cosh";}
	if (OP_STATE==16) {val = sinh(inputs[IN1_INPUT].getVoltage());fctDesc="sinh";}
	if (OP_STATE==17) {val = tanh(inputs[IN1_INPUT].getVoltage());fctDesc="tanh";}
	if (OP_STATE==18) {val = acosh(inputs[IN1_INPUT].getVoltage());fctDesc="acosh";}
	if (OP_STATE==19) {val = asinh(inputs[IN1_INPUT].getVoltage());fctDesc="asinh";}
	if (OP_STATE==20) {val = atanh(inputs[IN1_INPUT].getVoltage());fctDesc="atanh";}

	if (isfinite(val)) lights[ERROR_LIGHT].setBrightness(0); else {lights[ERROR_LIGHT].setBrightness(1);perror=10000;oerror=50000;}
	if (perror>0) perror-=1;if (oerror>0) oerror-=1;
	lights[PERROR_LIGHT].setBrightness(float(perror)/10000.0);lights[OERROR_LIGHT].setBrightness(float(oerror)/50000.0);

	outputs[OUT_OUTPUT].setVoltage(val);
}
};
struct FUNKTIONDisplay : TransparentWidget {
	FUNKTION *module;

	int frame = 0;
	

	FUNKTIONDisplay() {
		
	}
	
	void drawLayer(const DrawArgs &args, int layer) override {
if (layer ==1) {
shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, "res/LEDCalculator.ttf"));
std::string fD= module ? module->fctDesc : "sin";
		std::string to_display = "";
		for (int i=0; i<14; i++) to_display = to_display + fD[i];
		nvgFontSize(args.vg, 24);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 0);
		nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0xff));
		nvgRotate(args.vg, -M_PI / 2.0f);	
		nvgTextBox(args.vg, 5, 5,350, to_display.c_str(), NULL);
	}
Widget::drawLayer(args, layer);
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
	}
};


struct FUNKTIONWidget : ModuleWidget {
	FUNKTIONWidget(FUNKTION *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/FUNKTION.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));


	addInput(createInput<PJ301MPort>(Vec(3, 31), module, FUNKTION::IN1_INPUT));
	//addInput(Port::create<PJ301MPort>(Vec(3, 95), Port::INPUT, module, FUNKTION::IN2_INPUT));

	{
		FUNKTIONDisplay *fdisplay = new FUNKTIONDisplay();
		fdisplay->box.pos = Vec(18, 253);
		fdisplay->box.size = Vec(130, 250);
		fdisplay->module = module;
		addChild(fdisplay);
	}
	addChild(createLight<MediumLight<RedLight>>(Vec(11, 81.4), module, FUNKTION::ERROR_LIGHT));
	addChild(createLight<MediumLight<RedLight>>(Vec(11, 96.4), module, FUNKTION::PERROR_LIGHT));
	addChild(createLight<MediumLight<RedLight>>(Vec(11, 111.4), module, FUNKTION::OERROR_LIGHT));
	
	addParam(createParam<upButton>(Vec(6, 296+2), module, FUNKTION::PREV_PARAM));
	addParam(createParam<downButton>(Vec(6, 276+2), module, FUNKTION::NEXT_PARAM));

	addOutput(createOutput<PJ301MPort>(Vec(3, 321), module, FUNKTION::OUT_OUTPUT));
	
}
};


Model *modelFUNKTION =createModel<FUNKTION, FUNKTIONWidget>("FUNKTION");
