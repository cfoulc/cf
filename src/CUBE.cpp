
#include "cf.hpp"
#include "dsp/digital.hpp"


struct CUBE : Module {
	enum ParamIds {
		
		NUM_PARAMS
	};
	enum InputIds {
		X_INPUT,
		Y_INPUT,
		NUM_INPUTS
		
	};
	enum OutputIds {
		X_OUTPUT,
		NUM_OUTPUTS
	};

	float frameX = 0.0;
	float frameY = 0.0;

	float xx[12] = {-1.0, 1.0, 1.0,-1.0,-1.0, 1.0, 1.0,-1.0};
	float yy[12] = {-1.0,-1.0, 1.0, 1.0,-1.0,-1.0, 1.0, 1.0};
	float zz[12] = {-1.0,-1.0,-1.0,-1.0, 1.0, 1.0, 1.0, 1.0};

	float x[12] = {};
	float y[12] = {};
	float z[12] = {};

	float d = 0.0;
	float theta= 0.0 ;
	float gainX = 1.0;
	float gainY = 1.0;
	

	CUBE() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;

};



void CUBE::step() { 
	gainX = 0.5; gainY = 0.5;
	if (inputs[X_INPUT].active) gainX=inputs[X_INPUT].value;
	if (inputs[Y_INPUT].active) gainY=inputs[Y_INPUT].value;

       	for(int i=0; i<12; i++)
        	{
			d = sqrt(yy[i]*yy[i] + zz[i]*zz[i]);
			theta = atan2(yy[i],zz[i])+frameX;
			x[i] = xx[i]; 
			y[i] = d * sin(theta); 
			z[i] = d * cos(theta);

			d = sqrt(x[i]*x[i] + z[i]*z[i]);
			theta = atan2(x[i],z[i])+frameY;
			x[i] = d * sin(theta); 
			y[i] = y[i]; 
			z[i] = d * cos(theta);
        	}
		
	if (frameX<100) frameX=frameX+gainX/engineGetSampleRate(); else frameX=0;
	if (frameY<100) frameY=frameY+gainY/engineGetSampleRate(); else frameY=0;


	outputs[X_OUTPUT].value=z[0]*5.0;
}

struct CUBEDisplay : TransparentWidget {

	float *xxxx[12] = {};
	float *yyyy[12] = {};

	CUBEDisplay() {
		
	}
	
	void draw(NVGcontext *vg) {

		nvgStrokeColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, *xxxx[0]*20,*yyyy[0]*20);
			nvgLineTo(vg, *xxxx[1]*20,*yyyy[1]*20);
			nvgLineTo(vg, *xxxx[2]*20,*yyyy[2]*20);
			nvgLineTo(vg, *xxxx[3]*20,*yyyy[3]*20);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, *xxxx[4]*20,*yyyy[4]*20);
			nvgLineTo(vg, *xxxx[5]*20,*yyyy[5]*20);
			nvgLineTo(vg, *xxxx[6]*20,*yyyy[6]*20);
			nvgLineTo(vg, *xxxx[7]*20,*yyyy[7]*20);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, *xxxx[0]*20,*yyyy[0]*20);
			nvgLineTo(vg, *xxxx[4]*20,*yyyy[4]*20);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, *xxxx[1]*20,*yyyy[1]*20);
			nvgLineTo(vg, *xxxx[5]*20,*yyyy[5]*20);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, *xxxx[2]*20,*yyyy[2]*20);
			nvgLineTo(vg, *xxxx[6]*20,*yyyy[6]*20);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, *xxxx[3]*20,*yyyy[3]*20);
			nvgLineTo(vg, *xxxx[7]*20,*yyyy[7]*20);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

	}
};


CUBEWidget::CUBEWidget() {
	CUBE *module = new CUBE();
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/CUBE.svg")));
		addChild(panel);

	}
	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 365)));

	{
		CUBEDisplay *display = new CUBEDisplay();
		display->box.pos = Vec(60, 120);
		for (int i=0;i<12;i++) {
			display->xxxx[i] = &module->x[i] ;
			display->yyyy[i] = &module->y[i] ;	
		}
		addChild(display);
	}

	addInput(createInput<PJ301MPort>(Vec(15, 321), module, CUBE::X_INPUT));
	addInput(createInput<PJ301MPort>(Vec(47, 321), module, CUBE::Y_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(80, 321), module, CUBE::X_OUTPUT));       
	
}
