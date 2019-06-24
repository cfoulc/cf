#include "plugin.hpp"
#include "dsp/digital.hpp"


struct SUB : Module {
	enum ParamIds {
	        GAIN_PARAM,
		GAIN2_PARAM,
	        LINK_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		GAIN_INPUT,
		GAIN2_INPUT,
		M1_INPUT,
		M2_INPUT,
		IN1_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		M1_OUTPUT,
		OUT1_OUTPUT,
		M2_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		LINK_LIGHT,
		NUM_LIGHTS
	};


float SIGNAL = 0.0 ;
float SIGNAL2 = 0.0 ;
float or_gain ;
float or2_gain ;
int or_affi ;
int or2_affi ;
bool LINK_STATE = false ;
dsp::SchmittTrigger linkTrigger;


	SUB() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LINK_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(GAIN_PARAM, 0.0f, 10.0f, 0.0f, "Gain");
		configParam(GAIN2_PARAM, 0.0f, 10.0f, 0.0f, "Gain");
		onReset();
}


void onReset() override {
			LINK_STATE = false;
			}

json_t *dataToJson() override {
		json_t *rootJ = json_object();
		// solo
		json_object_set_new(rootJ, "linkstate", json_integer(LINK_STATE));

		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		// solo
		json_t *linkstateJ = json_object_get(rootJ, "linkstate");
		if (linkstateJ)
			LINK_STATE = json_integer_value(linkstateJ);

	
	}




void process(const ProcessArgs &args) override {

	if (linkTrigger.process(params[LINK_PARAM].value))
			{LINK_STATE=!LINK_STATE;}
	lights[LINK_LIGHT].value=LINK_STATE;


        SIGNAL = inputs[IN1_INPUT].value ;

	outputs[OUT1_OUTPUT].value = SIGNAL;

	if (!inputs[GAIN_INPUT].active)
		{SIGNAL = SIGNAL * params[GAIN_PARAM].value/10.0 ;or_affi=0;}
		else {SIGNAL = SIGNAL * clamp(inputs[GAIN_INPUT].value/10.0f,0.0f,1.0f) ; or_affi=1;or_gain=clamp(inputs[GAIN_INPUT].value,0.0f,10.0f);}

	outputs[M1_OUTPUT].value = inputs[M1_INPUT].value + SIGNAL;


        SIGNAL2 = inputs[IN2_INPUT].value ;

	outputs[OUT2_OUTPUT].value = SIGNAL2;

	if (!LINK_STATE) {
		if (!inputs[GAIN2_INPUT].active) 
			{SIGNAL2 = SIGNAL2 * params[GAIN2_PARAM].value/10.0 ;or2_affi=0;}
			else {SIGNAL2 = SIGNAL2 * clamp(inputs[GAIN2_INPUT].value/10.0f,0.0f,1.0f) ; or2_affi=1;or2_gain=clamp(inputs[GAIN2_INPUT].value,0.0f,10.0f);}
	} else {
		if (!inputs[GAIN_INPUT].active)
			{SIGNAL2 = SIGNAL2 * params[GAIN_PARAM].value/10.0 ;or2_affi=1;or2_gain=clamp(params[GAIN_PARAM].value,0.0f,10.0f);}
		else {SIGNAL2 = SIGNAL2 * clamp(inputs[GAIN_INPUT].value/10.0f,0.0f,1.0f) ; or2_affi=1;or2_gain=clamp(inputs[GAIN_INPUT].value,0.0f,10.0f);}
	}

	outputs[M2_OUTPUT].value = inputs[M2_INPUT].value + SIGNAL2;


}
};

struct MDisplay : TransparentWidget {
	SUB *module;

	MDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {

float gainX = module ? module->or_gain : 1.0f;
int affich = module ? module->or_affi : 0;
float d=19.1;

		if (affich==1) {
		float xx = d*sin(-(gainX*0.17+0.15)*M_PI) ;
		float yy = d*cos((gainX*0.17+0.15)*M_PI) ;

		
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 0,0, d);
			nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
			nvgFill(args.vg);	
		
			nvgStrokeWidth(args.vg,1.2);
			nvgStrokeColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
			{
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, 0,0);
				nvgLineTo(args.vg, xx,yy);
				nvgClosePath(args.vg);
			}
			nvgStroke(args.vg);
		}

	}
};

struct MSDisplay : TransparentWidget {
	SUB *module;

	MSDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {

float gainX = module ? module->or2_gain : 1.0f;
int affich = module ? module->or2_affi : 0;
float d=19.1;

		if (affich==1) {
		float xx = d*sin(-(gainX*0.17+0.15)*M_PI) ;
		float yy = d*cos((gainX*0.17+0.15)*M_PI) ;

		
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 0,0, d);
			nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
			nvgFill(args.vg);	
		
			nvgStrokeWidth(args.vg,1.2);
			nvgStrokeColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
			{
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, 0,0);
				nvgLineTo(args.vg, xx,yy);
				nvgClosePath(args.vg);
			}
			nvgStroke(args.vg);
		}

	}
};


struct SUBWidget : ModuleWidget {
	SUBWidget(SUB *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SUB.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

     	addParam(createParam<LEDButton>(Vec(22, 179), module, SUB::LINK_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(26.5, 182.5), module, SUB::LINK_LIGHT));


    	addParam(createParam<RoundLargeBlackKnob>(Vec(27, 247), module, SUB::GAIN2_PARAM));
	addInput(createInput<PJ301MPort>(Vec(11, 281), module, SUB::GAIN2_INPUT));
	{
		MSDisplay *display2 = new MSDisplay();
		display2->box.pos = Vec(46, 266);
		display2->module = module;
		addChild(display2);
	}

    

	addInput(createInput<PJ301MPort>(Vec(11, 321), module, SUB::IN2_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(54, 321), module, SUB::OUT2_OUTPUT));
	

	addOutput(createOutput<PJ301MPort>(Vec(54, 61+152), module, SUB::M2_OUTPUT));


	addInput(createInput<PJ301MPort>(Vec(11, 61+152), module, SUB::M2_INPUT));



    	addParam(createParam<RoundLargeBlackKnob>(Vec(27, 247-182), module, SUB::GAIN_PARAM));
	addInput(createInput<PJ301MPort>(Vec(11, 281-182), module, SUB::GAIN_INPUT));
	{
		MDisplay *display = new MDisplay();
		display->box.pos = Vec(46, 266-182);
		display->module = module;
		addChild(display);
	}

    

	addInput(createInput<PJ301MPort>(Vec(11, 321-182), module, SUB::IN1_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(54, 321-182), module, SUB::OUT1_OUTPUT));
	

	addOutput(createOutput<PJ301MPort>(Vec(54, 31), module, SUB::M1_OUTPUT));


	addInput(createInput<PJ301MPort>(Vec(11, 31), module, SUB::M1_INPUT));

	
}
};

Model *modelSUB = createModel<SUB, SUBWidget>("SUB");
