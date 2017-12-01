


#include "cf.hpp"



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
//		Y_OUTPUT,
//		Z_OUTPUT,
		NUM_OUTPUTS
	};


	CUBE() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;

};


void CUBE::step() {
	//outputs[X_OUTPUT].value = 10;
}

struct CUBEDisplay : TransparentWidget {
	CUBE *module;
	float frameX = 0.0;
	float frameY = 0.0;

	float xx[12] = {-1.0, 1.0, 1.0,-1.0,-1.0, 1.0, 1.0,-1.0};
	float yy[12] = {-1.0,-1.0, 1.0, 1.0,-1.0,-1.0, 1.0, 1.0};
	float zz[12] = {-1.0,-1.0,-1.0,-1.0, 1.0, 1.0, 1.0, 1.0};

	float x[12] = {};
	float y[12] = {};
	float z[12] = {};

	float d =0.0;
	float theta=0.0 ;

	int repx= 55;
	int repy=80;

	CUBEDisplay() {
		
	}
	
	void draw(NVGcontext *vg) {
	float gainX = module->inputs[CUBE::X_INPUT].value;
	float gainY = module->inputs[CUBE::Y_INPUT].value;

	if (!module->inputs[CUBE::X_INPUT].active) gainX=1.0;
	if (!module->inputs[CUBE::Y_INPUT].active) gainY=1.0;

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


		nvgStrokeColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[0]*20+ repx,y[0]*20+ repy);
			nvgLineTo(vg, x[1]*20+ repx,y[1]*20+ repy);
			nvgLineTo(vg, x[2]*20+ repx,y[2]*20+ repy);
			nvgLineTo(vg, x[3]*20+ repx,y[3]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[4]*20+ repx,y[4]*20+ repy);
			nvgLineTo(vg, x[5]*20+ repx,y[5]*20+ repy);
			nvgLineTo(vg, x[6]*20+ repx,y[6]*20+ repy);
			nvgLineTo(vg, x[7]*20+ repx,y[7]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[0]*20+ repx,y[0]*20+ repy);
			nvgLineTo(vg, x[4]*20+ repx,y[4]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[1]*20+ repx,y[1]*20+ repy);
			nvgLineTo(vg, x[5]*20+ repx,y[5]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[2]*20+ repx,y[2]*20+ repy);
			nvgLineTo(vg, x[6]*20+ repx,y[6]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[3]*20+ repx,y[3]*20+ repy);
			nvgLineTo(vg, x[7]*20+ repx,y[7]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		
		frameX=frameX+gainX/50.0;
		frameY=frameY+gainY/50.0;

		

	module->outputs[CUBE::X_OUTPUT].value=z[0]*5.0;
//	module->outputs[CUBE::Y_OUTPUT].value=y[2]*5.0;
//	module->outputs[CUBE::Z_OUTPUT].value=z[2]*5.0;
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
		display->module = module;
		display->box.pos = Vec(5, 40);
		display->box.size = Vec(130, 250);
		addChild(display);
	}

	addInput(createInput<PJ301MPort>(Vec(15, 321), module, CUBE::X_INPUT));
	addInput(createInput<PJ301MPort>(Vec(47, 321), module, CUBE::Y_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(80, 321), module, CUBE::X_OUTPUT));
	//addOutput(createOutput<PJ301MPort>(Vec(80, 286), module, CUBE::Y_OUTPUT));
	//addOutput(createOutput<PJ301MPort>(Vec(80, 316), module, CUBE::Z_OUTPUT));
	
}
