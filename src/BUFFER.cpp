
#include "cf.hpp"
#include "dsp/digital.hpp"


struct BUFFER : Module {
	enum ParamIds {
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


	float buf[10000] ={};
	float x = 0;
	int pos = 0;
	int length = 0;
	float l_gain ;
	int l_affi ;

	BUFFER() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;

};



void BUFFER::step() { 

	if (!inputs[LENGTH_INPUT].active) {
		length = int(params[LENGTH_PARAM].value*9998.0f)+1;
		l_affi =0;
		}
	else {
		length = clamp(int(inputs[LENGTH_INPUT].value*999.8f),0,9999)+1;
		l_gain = inputs[LENGTH_INPUT].value;
		l_affi = 1;
		}


	buf[pos]=inputs[IN_INPUT].value+inputs[FB_INPUT].value*params[FB_PARAM].value;

	x = float(pos) ;
	if (pos<10000) pos=pos+1; else pos=0;

	if ((pos-length)>0)
		outputs[X_OUTPUT].value=buf[pos-length];
	else
		outputs[X_OUTPUT].value=buf[9999+pos-length];
}



struct BUFFERDisplay : TransparentWidget {

	float *xxxx;
	int *llll;
	float *dbuf[10000] = {};
	//int bpos = 0;

	BUFFERDisplay() {
	
		
	}
	
	void draw(NVGcontext *vg) {

		nvgStrokeColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, clamp(*dbuf[int(*xxxx)]*4.0f,-45.0f,45.0f),0.0f);
			for (int i=1;i<*llll; i++) {if ((*xxxx-i)>0) nvgLineTo(vg, clamp(*dbuf[int(*xxxx)-i]*4.0f,-45.0f,45.0f), -200*(i+1)/(*llll)); 
							       else nvgLineTo(vg, clamp(*dbuf[9999+int(*xxxx)-i]*4.0f,-45.0f,45.0f), -200*(i+1)/(*llll));
								}
			//nvgClosePath(vg);
		}
		nvgStroke(vg);

	}
};

struct MOTORPOTDisplay : TransparentWidget {

	float d;
	float *gainX ;
	int *affich;

	MOTORPOTDisplay() {
		
	}
	
	void draw(NVGcontext *vg) {
		if (*affich==1) {
		float xx = d*sin(-(*gainX*0.17+0.15)*M_PI) ;
		float yy = d*cos((*gainX*0.17+0.15)*M_PI) ;

		
			nvgBeginPath(vg);
			nvgCircle(vg, 0,0, d);
			nvgFillColor(vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
			nvgFill(vg);	
		
			nvgStrokeWidth(vg,1.2);
			nvgStrokeColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
			{
				nvgBeginPath(vg);
				nvgMoveTo(vg, 0,0);
				nvgLineTo(vg, xx,yy);
				nvgClosePath(vg);
			}
			nvgStroke(vg);
		}

	}
};

struct BUFFERWidget : ModuleWidget {
	BUFFERWidget(BUFFER *module);
};

BUFFERWidget::BUFFERWidget(BUFFER *module) : ModuleWidget(module) {
	setPanel(SVG::load(assetPlugin(plugin, "res/BUFFER.svg")));

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x-30, 365)));

	{
		BUFFERDisplay *bdisplay = new BUFFERDisplay();
		bdisplay->box.pos = Vec(60, 270);
			bdisplay->xxxx = &module->x ;
			bdisplay->llll = &module->length ;
			for (int i=0;i<10000;i++) {
				bdisplay->dbuf[i] = &module->buf[i] ;	
			}	
		addChild(bdisplay);
	}

	addInput(Port::create<PJ301MPort>(Vec(15, 321), Port::INPUT, module, BUFFER::IN_INPUT));

	addInput(Port::create<PJ301MPort>(Vec(47, 321), Port::INPUT, module, BUFFER::FB_INPUT));
	addParam(ParamWidget::create<Trimpot>(Vec(50.4, 284), module, BUFFER::FB_PARAM, 0.0f, 1.0f, 0.5f));

	addInput(Port::create<PJ301MPort>(Vec(80, 321), Port::INPUT, module, BUFFER::LENGTH_INPUT));
	addParam(ParamWidget::create<Trimpot>(Vec(83.4, 284), module, BUFFER::LENGTH_PARAM, 0.0f, 1.0f, 0.5f)); 
	{
		MOTORPOTDisplay *pdisplay = new MOTORPOTDisplay();
		pdisplay->box.pos = Vec(92.8, 293.2);
		pdisplay->d = 9.3;
		pdisplay->gainX = &module->l_gain;
		pdisplay->affich = &module->l_affi;
		addChild(pdisplay);
	}
	addOutput(Port::create<PJ301MPort>(Vec(80, 31), Port::OUTPUT, module, BUFFER::X_OUTPUT)); 
	
}

Model *modelBUFFER = Model::create<BUFFER, BUFFERWidget>("cf", "BUFFER", "Buffer", DELAY_TAG);