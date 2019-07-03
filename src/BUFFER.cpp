
#include "plugin.hpp"



struct BUFFER : Module {
	enum ParamIds {
		MODE_PARAM,
		LENGTH_PARAM,
		FB_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		FB_INPUT,
		LENGTH_INPUT,
		NUM_INPUTS
		
	};
	enum OutputIds {
		X_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		MODE_LIGHT,
		NUM_LIGHTS
	};


	float buf[10000] ={};
	float x = 0;
	int pos = 0;
	int length = 0;
	float l_gain ;
	int l_affi ;

	bool MODE_STATE = false ;
	dsp::SchmittTrigger modeTrigger;


BUFFER() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(MODE_PARAM, 0.0, 1.0, 0.0, "Mode");
		configParam(FB_PARAM, 0.0f, 1.0f, 0.5f, "Feedback");
		configParam(LENGTH_PARAM, 0.0f, 1.0f, 0.5f, "LENGTH");

}


json_t *dataToJson() override {
		json_t *rootJ = json_object();
		

		json_object_set_new(rootJ, "modestate", json_integer(MODE_STATE));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		

		json_t *modestateJ = json_object_get(rootJ, "modestate");
		if (modestateJ)
			MODE_STATE = json_integer_value(modestateJ);
	
	}





void process(const ProcessArgs &args) override { 

	if (modeTrigger.process(params[MODE_PARAM].getValue())) 
			{if (MODE_STATE == 0) MODE_STATE = 1; else MODE_STATE = 0;}

	lights[MODE_LIGHT].setBrightness(MODE_STATE);

	if (!inputs[LENGTH_INPUT].isConnected()) {
		length = int(params[LENGTH_PARAM].getValue()*9998.0f)+1;
		l_affi =0;
		}
	else {
		length = clamp(int(inputs[LENGTH_INPUT].getVoltage()*999.8f),0,9998)+1;
		l_gain = clamp(inputs[LENGTH_INPUT].getVoltage(),0.0f,10.0f);
		l_affi = 1;
		}

if (MODE_STATE) length = (int(length/10))+2;

	buf[pos]=(inputs[IN_INPUT].getVoltage()+inputs[FB_INPUT].getVoltage()*params[FB_PARAM].getValue()) ; // /(1.0+params[FB_PARAM].getValue());

	x = float(pos) ;
	if (pos<9999) pos=pos+1; else pos=0;

if (!MODE_STATE) {
	if ((pos-length)>0)
		outputs[X_OUTPUT].setVoltage(clamp(buf[pos-length],-10.0f,10.0f));
	else
		outputs[X_OUTPUT].setVoltage(clamp(buf[9999+pos-length],-10.0f,10.0f));
   } else {
	float som = 0.0;
	for (int i = 1 ; i < length ; i++) {
		if ((pos-i)>0)
			som=som+buf[pos-i];
		else
			som=som+buf[9999+pos-i];
	}

	outputs[X_OUTPUT].setVoltage(clamp((inputs[FB_INPUT].getVoltage()*params[FB_PARAM].getValue() - (som / float(length-1))),-10.0f,10.0f));
    }


}

};

struct BUFFERDisplay : TransparentWidget {
	BUFFER *module;

	BUFFERDisplay() {
	
		
	}
	
	void draw(const DrawArgs &args) override {
if (module) {
		nvgStrokeWidth(args.vg,1.2);
		nvgStrokeColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff ));
		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, clamp(module->buf[int(module->x)]*4.0f,-45.0f,45.0f),0.0f);
			for (int i=1;i<module->length; i++) {if ((module->x-i)>0) nvgLineTo(args.vg, clamp(module->buf[int(module->x)-i]*4.0f,-45.0f,45.0f), -200.0*(i+1)/(module->length)); 
							       else nvgLineTo(args.vg, clamp(module->buf[9999+int(module->x)-i]*4.0f,-45.0f,45.0f), -200.0*(i+1)/(module->length));
								}
		}
		nvgLineCap(args.vg, NVG_ROUND);
		nvgMiterLimit(args.vg, 20.0f);
		nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
		nvgStroke(args.vg);

	}
};
};

struct MBDisplay : TransparentWidget {
	BUFFER *module;

	MBDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {

float gainX = module ? module->l_gain : 1.0f;
int affich = module ? module->l_affi : 0;
float d=9.3f;

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


///////////////////

struct BUFFERWidget : ModuleWidget {
	BUFFERWidget(BUFFER *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BUFFER.svg")));


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	{
		BUFFERDisplay *bdisplay = new BUFFERDisplay();
		bdisplay->box.pos = Vec(60, 270);
		bdisplay->module = module ;
		addChild(bdisplay);
	}


     	addParam(createParam<LEDButton>(Vec(19, 35), module, BUFFER::MODE_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(23.4, 39.4), module, BUFFER::MODE_LIGHT));

	addInput(createInput<PJ301MPort>(Vec(15, 321), module, BUFFER::IN_INPUT));

	addInput(createInput<PJ301MPort>(Vec(47, 321), module, BUFFER::FB_INPUT));
	addParam(createParam<Trimpot>(Vec(50.4, 284), module, BUFFER::FB_PARAM));

	addInput(createInput<PJ301MPort>(Vec(80, 321), module, BUFFER::LENGTH_INPUT));
	addParam(createParam<Trimpot>(Vec(83.4, 284), module, BUFFER::LENGTH_PARAM)); 
	{
		MBDisplay *pdisplay = new MBDisplay();
		pdisplay->box.pos = Vec(92.8, 293.2);
		pdisplay->module = module;
		addChild(pdisplay);
	}
	addOutput(createOutput<PJ301MPort>(Vec(80, 31), module, BUFFER::X_OUTPUT)); 
	
}
};

Model *modelBUFFER = createModel<BUFFER, BUFFERWidget>("BUFFER");