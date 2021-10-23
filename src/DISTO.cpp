
#include "plugin.hpp"



struct DISTO : Module {
	enum ParamIds {
		FOLD_PARAM,
		GAIN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,
		GAIN_INPUT,
		FOLD_INPUT,
		NUM_INPUTS
		
	};
	enum OutputIds {
		X_OUTPUT,
		NUM_OUTPUTS
	};


	float x = 0;
	float y = 0;
	int length = 0;
	float fold_gain ;
	int fold_affi ;
	float gain_gain ;
	int gain_affi ;

	DISTO(){
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(GAIN_PARAM, 0.0f, 10.0f, 0.2f, "Gain");
		configParam(FOLD_PARAM, 0.0f, 10.0f, 0.0f, "Fold");
}


void process(const ProcessArgs &args) override {

	if (inputs[FOLD_INPUT].isConnected()) {
		fold_affi =true; fold_gain = clamp(inputs[FOLD_INPUT].getVoltage(),-0.001,10.001) ;} 
	 else {fold_affi =false; fold_gain = params[FOLD_PARAM].getValue() ;}

	if (inputs[GAIN_INPUT].isConnected()) {
		gain_affi =true; gain_gain = clamp(inputs[GAIN_INPUT].getVoltage(),-0.001,10.001) ;} 
	 else {gain_affi =false; gain_gain = params[GAIN_PARAM].getValue() ;}

//////////DISTO
	x=inputs[IN_INPUT].getVoltage()*5.0f*gain_gain;

	if (abs(x)>5) y = clamp((abs(x)-5)/2.2f,0.0f,58.0f); else y=0;

	for (int i =0; i<100; i++) {	if (x<-5.0f) x=-5.0f+(-x-5.0f)*fold_gain/5.0; 
					if (x>5.0f) x=5.0f-(x-5.0f)*fold_gain/5.0;
					if ((x>=-5.0) & (x<=5.0)) i=1000; if (i==99) x=0;}
	

	//float fold;
        //const float bias = (x < 0) ? -5.f : 5.f;
        //int phase = int((x + bias) / 10.f);
        //bool isEven = !(phase & 1);
        //if (isEven) {
        //    fold = x - 10.f * phase;
        //} else {
        //    fold = -x + 10.f * phase;
        //}

 	//x = ((int(x/5.0)%2)*2-1)*(x%5) + (int(x/5.0)%2)*5.0 ;

	//int xa ; float xb, xc;
	//xa=int(x/5.0) ;
	//xb=int(xa-2.0*int(xa/2.0)) ;
	//if (xb == 0) xc = (x-5*xa) ; else xc= xb*5.0 - abs(xb)*(x-5*xa) ;                //+5.0*(2*xb-1) ;

	outputs[X_OUTPUT].setVoltage(x);                      //clamp(x,-5.0f,5.0f); 

}
};

struct cachecl : SvgScrew {
	cachecl() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/distocach.svg")));
		box.size = sw->box.size;
	}
};

struct DISTODisplay : TransparentWidget {
	DISTO *module;

		float bu[10] = {};
	int ind = 0;
		

	DISTODisplay() {

	}
void draw(const DrawArgs &args) override {
		for (int i = 0 ; i<5 ; i++){
			if (bu[i]>=27)
			{//nvgStrokeColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 0,0, bu[i]);
			nvgFillColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xaa));
			nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
			nvgFill(args.vg);
			nvgClosePath(args.vg);
			}
		}
}

void drawLayer(const DrawArgs &args, int layer) override {


float xxxx = module ? module->y : 1.0f;

for (int i = 4 ; i>0 ; i--){bu[i] = bu[i-1] ;}
		bu[0] = xxxx ;
if (layer ==1) {
		for (int i = 0 ; i<5 ; i++){
			if (bu[i]<27)
			{//nvgStrokeColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 0,0, bu[i]);
			nvgFillColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xaa));
			nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
			nvgFill(args.vg);
			nvgClosePath(args.vg);
			}else
{//nvgStrokeColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 0,0, 27);
			nvgFillColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xaa));
			nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
			nvgFill(args.vg);
			nvgClosePath(args.vg);
			}
		}
		//nvgStroke(args.vg);
	}
Widget::drawLayer(args, layer);
};
};

struct MOGAINDisplay : TransparentWidget {
	DISTO *module;

	MOGAINDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {
//nvgGlobalTint(args.vg, color::WHITE);
float gainX = module ? module->gain_gain : 1.0f;
//int affich = module ? module->gain_affi : 0;
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

struct MOFOLDDisplay : TransparentWidget {
	DISTO *module;

	MOFOLDDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {
//nvgGlobalTint(args.vg, color::WHITE);
float gainX = module ? module->fold_gain : 1.0f;
//int affich = module ? module->fold_affi : 0;
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

struct DISTOWidget : ModuleWidget {
	DISTOWidget(DISTO *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DISTO.svg")));


	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	{
		DISTODisplay *distdisplay = new DISTODisplay();
			distdisplay->box.pos = Vec(60, 170);
			distdisplay->module = module ;	
		addChild(distdisplay);
	}

	addInput(createInput<PJ301MPort>(Vec(15, 321), module, DISTO::IN_INPUT));

	addInput(createInput<PJ301MPort>(Vec(47, 321), module, DISTO::GAIN_INPUT));
	addParam(createParam<cfTrimpot>(Vec(50.4, 284), module, DISTO::GAIN_PARAM));
	{
		MOGAINDisplay *gaindisplay = new MOGAINDisplay();
		gaindisplay->box.pos = Vec(59.4, 293);
		gaindisplay->module = module;
		addChild(gaindisplay);
	}
	addInput(createInput<PJ301MPort>(Vec(80, 321), module, DISTO::FOLD_INPUT));
	addParam(createParam<cfTrimpot>(Vec(83.4, 284), module, DISTO::FOLD_PARAM)); 
	{
		MOFOLDDisplay *folddisplay = new MOFOLDDisplay();
		folddisplay->box.pos = Vec(92.4, 293);
		folddisplay->module = module;
		addChild(folddisplay);
	}
	addOutput(createOutput<PJ301MPort>(Vec(80, 31), module, DISTO::X_OUTPUT)); 
		addChild(createWidget<cachecl>(Vec(0, 0)));
}
};

Model *modelDISTO = createModel<DISTO, DISTOWidget>("DISTO");