#include "plugin.hpp"



struct STEREO : Module {
	enum ParamIds {
		PAN_PARAM,
	        GAIN_PARAM,
	        SOLO_PARAM,
		ON_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		SOLOT_INPUT,
		ONT_INPUT,
		PAN_INPUT,
		GAIN_INPUT,
		EXTSOLO_INPUT,
		LEFT_INPUT,
		RIGHT_INPUT,
		PANPOT_INPUT,
		IN1_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		EXTSOLO_OUTPUT,
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		TLEFT_OUTPUT,
		TRIGHT_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		SOLO_LIGHT,
		ON_LIGHT,
		ENUMS(LEVEL_LIGHTS, 11),
		NUM_LIGHTS
	};


float SIGNAL1 = 0.0 ;
float SIGNAL2 = 0.0 ;
bool ON_STATE = false ;
bool SOLO_STATE = false ;
bool soloed = false;
int lightState[11] = {};
int cligno =0;
int retard =0;
int retard2 =0;
dsp::SchmittTrigger onTrigger;
dsp::SchmittTrigger oninTrigger;
dsp::SchmittTrigger soloTrigger;
dsp::SchmittTrigger soloinTrigger;
float or_gain ;
int or_affi ;
float orp_gain ;
int orp_affi ;

	STEREO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SOLO_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(PAN_PARAM, -1.0f, 1.0f, 0.0f, "Pan");
		configParam(ON_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(GAIN_PARAM, 0.0f, 10.0f, 5.0f, "Gain");
		onReset();
	}


void onReset() override {
			ON_STATE = true;
			SOLO_STATE = false;
			}

json_t *dataToJson() override {
		json_t *rootJ = json_object();
		// solo
		json_object_set_new(rootJ, "solostate", json_integer(SOLO_STATE));

		// solo
		json_object_set_new(rootJ, "onstate", json_integer(ON_STATE));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		// solo
		json_t *solostateJ = json_object_get(rootJ, "solostate");
		if (solostateJ)
			SOLO_STATE = json_integer_value(solostateJ);

		// solo
		json_t *onstateJ = json_object_get(rootJ, "onstate");
		if (onstateJ)
			ON_STATE = json_integer_value(onstateJ);
	
	}




void process(const ProcessArgs &args) override {

        SIGNAL1 = inputs[IN1_INPUT].getVoltage() ;
	SIGNAL2 = inputs[IN2_INPUT].getVoltage() ;

	if (!inputs[GAIN_INPUT].isConnected()) {
		SIGNAL1 = SIGNAL1 * params[GAIN_PARAM].getValue()/5.0 ;
		SIGNAL2 = SIGNAL2 * params[GAIN_PARAM].getValue()/5.0 ;
		or_affi =0;
		}
		else {
		SIGNAL1 = SIGNAL1 * clamp(inputs[GAIN_INPUT].getVoltage()/5.0,0.0f,2.0f) ;
		SIGNAL2 = SIGNAL2 * clamp(inputs[GAIN_INPUT].getVoltage()/5.0,0.0f,2.0f) ;
		or_affi=1;or_gain=clamp(inputs[GAIN_INPUT].getVoltage(),0.0f,10.0f);
		}

	if (onTrigger.process(params[ON_PARAM].getValue())+oninTrigger.process(inputs[ONT_INPUT].getVoltage()))
			{if (ON_STATE == 0) ON_STATE = 1; else ON_STATE = 0;}

	if (inputs[EXTSOLO_INPUT].getVoltage() == 0) soloed = 0;
	if (inputs[EXTSOLO_INPUT].getVoltage() == 10) soloed = 1;

	if (soloTrigger.process(params[SOLO_PARAM].getValue())+soloinTrigger.process(inputs[SOLOT_INPUT].getVoltage()))
			{if (SOLO_STATE == 0) {SOLO_STATE = 1;} else {SOLO_STATE = 0;soloed=0;}}

	if ((!SOLO_STATE and !soloed) and (retard > 0)) retard = 0; else if (retard < 1000) retard = retard + 1;


	outputs[EXTSOLO_OUTPUT].setVoltage(round(10*retard/1000));

	if (!SOLO_STATE) {SIGNAL1 = SIGNAL1 * ON_STATE ; SIGNAL2 = SIGNAL2 * ON_STATE ;}
	if (soloed and !SOLO_STATE) {SIGNAL1 = 0; SIGNAL2 = 0;}
	

	if (!inputs[PAN_INPUT].isConnected()) {
			outputs[LEFT_OUTPUT].setVoltage(inputs[LEFT_INPUT].getVoltage() + SIGNAL1*(1-clamp(params[PAN_PARAM].getValue(),0.0f,1.0f)));
			outputs[RIGHT_OUTPUT].setVoltage(inputs[RIGHT_INPUT].getVoltage() + SIGNAL2*(1-clamp(-params[PAN_PARAM].getValue(),0.0f,1.0f)));
			outputs[TLEFT_OUTPUT].setVoltage(SIGNAL1*(1-clamp(params[PAN_PARAM].getValue(),0.0f,1.0f)));
			outputs[TRIGHT_OUTPUT].setVoltage(SIGNAL2*(1-clamp(-params[PAN_PARAM].getValue(),0.0f,1.0f)));
			orp_affi = 0;orp_gain = params[PAN_PARAM].getValue()*5+5;
		} else {
			outputs[LEFT_OUTPUT].setVoltage(inputs[LEFT_INPUT].getVoltage() + SIGNAL1*(1-(clamp(inputs[PAN_INPUT].getVoltage(),5.0f,10.0f)-5)/5));
			outputs[RIGHT_OUTPUT].setVoltage(inputs[RIGHT_INPUT].getVoltage() + SIGNAL2*(1-(clamp(inputs[PAN_INPUT].getVoltage(),0.0f,5.0f)+5)/5));
			outputs[TLEFT_OUTPUT].setVoltage(SIGNAL1*(1-(clamp(inputs[PAN_INPUT].getVoltage(),5.0f,10.0f)-5)/5));
			outputs[TRIGHT_OUTPUT].setVoltage(SIGNAL2*(1-(clamp(inputs[PAN_INPUT].getVoltage(),0.0f,5.0f)+5)/5));
			orp_affi = 1;orp_gain = clamp(inputs[PAN_INPUT].getVoltage(),0.0f,10.0f);
		}

	if (ON_STATE==1) lights[ON_LIGHT].setBrightness(1); else lights[ON_LIGHT].setBrightness(0);
	
	if (SOLO_STATE==1) {if (cligno == 0) cligno =20000; else cligno=cligno-1;} else cligno = 0;
	if (cligno>5000) lights[SOLO_LIGHT].setBrightness(1); else lights[SOLO_LIGHT].setBrightness(0);

	for (int i = 0; i < 11; i++) {
		if (std::max(SIGNAL1,SIGNAL2)> i) {if (i<10) lightState[i]=5000;else lightState[i]=20000;}
	}
	for (int i = 0; i < 11; i++) {
		if (lightState[i]> 0) {lightState[i]=lightState[i]-1;lights[LEVEL_LIGHTS + i].setBrightness(1);} else lights[LEVEL_LIGHTS + i].setBrightness(0);
	}
};
};


struct SMODisplay : TransparentWidget {
	STEREO *module;

	SMODisplay() {
		
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

struct SMOPDisplay : TransparentWidget {
	STEREO *module;

	SMOPDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {
//nvgGlobalTint(args.vg, color::WHITE);
float gainX = module ? module->orp_gain : 1.0f;
//int affich = module ? module->orp_affi : 0;
float d=8;

		//if (affich==1) {
		float xx = d*sin(-(gainX*0.17+0.15)*M_PI) ;
		float yy = d*cos((gainX*0.17+0.15)*M_PI) ;
		float xx0 = (d-6)*sin(-(gainX*0.17+0.15)*M_PI) ;
		float yy0 = (d-6)*cos((gainX*0.17+0.15)*M_PI) ;
		
			//nvgBeginPath(args.vg);
			//nvgCircle(args.vg, 0,0, d);
			//nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
			//nvgFill(args.vg);	
		
			nvgStrokeWidth(args.vg,2);
			nvgStrokeColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0x88));
			{
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, xx0,yy0);
				nvgLineTo(args.vg, xx,yy);
				nvgClosePath(args.vg);
			}
			nvgStroke(args.vg);
		//}

	}
};


struct STEREOWidget : ModuleWidget {
	STEREOWidget(STEREO *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/STEREO.svg")));


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));


	addParam(createParam<cfTrimpot>(Vec(38, 127), module, STEREO::PAN_PARAM));
	addInput(createInput<PJ301MPort>(Vec(11, 131), module, STEREO::PAN_INPUT));
	{
		SMOPDisplay *pdisplay = new SMOPDisplay();
		pdisplay->box.pos = Vec(47, 136);
		pdisplay->module = module;
		addChild(pdisplay);

	}

    	addParam(createParam<cfBigKnob>(Vec(27, 247), module, STEREO::GAIN_PARAM));
	addInput(createInput<PJ301MPort>(Vec(11, 281), module, STEREO::GAIN_INPUT));
	{
		SMODisplay *display = new SMODisplay();
		display->box.pos = Vec(45, 265);
		display->module = module;
		addChild(display);
	}


   	addParam(createParam<LEDButton>(Vec(38, 167), module, STEREO::SOLO_PARAM));
 	addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 171.4), module, STEREO::SOLO_LIGHT));
	addInput(createInput<PJ301MPort>(Vec(11, 171), module, STEREO::SOLOT_INPUT));

     	addParam(createParam<LEDButton>(Vec(38, 208), module, STEREO::ON_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 212.4), module, STEREO::ON_LIGHT));
	addInput(createInput<PJ301MPort>(Vec(11, 211), module, STEREO::ONT_INPUT));
    

	addInput(createInput<PJ301MPort>(Vec(11, 308), module, STEREO::IN1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(11, 334), module, STEREO::IN2_INPUT));
	
	addOutput(createOutput<PJ301MPort>(Vec(54, 31), module, STEREO::EXTSOLO_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 61), module, STEREO::LEFT_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 91), module, STEREO::RIGHT_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(11, 31), module, STEREO::EXTSOLO_INPUT));
	addInput(createInput<PJ301MPort>(Vec(11, 61), module, STEREO::LEFT_INPUT));
	addInput(createInput<PJ301MPort>(Vec(11, 91), module, STEREO::RIGHT_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(54, 308), module, STEREO::TLEFT_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(54, 334), module, STEREO::TRIGHT_OUTPUT));

	for (int i = 0; i < 11; i++) {
		if (i<10) addChild(createLight<MediumLight<BlueLight>>(Vec(70, 242-i*12), module, STEREO::LEVEL_LIGHTS + i));
			else addChild(createLight<MediumLight<RedLight>>(Vec(70, 242-i*12), module, STEREO::LEVEL_LIGHTS + i));
	}
	
	
}
};

Model *modelSTEREO = createModel<STEREO, STEREOWidget>("STEREO");
