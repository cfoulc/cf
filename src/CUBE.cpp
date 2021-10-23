
#include "plugin.hpp"



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

	float x[8] = {};
	float y[8] = {};
	float z[8] = {};

	float d = 0.0;
	float theta= 0.0 ;
	float gainX = 1.0;
	float gainY = 1.0;
	

	CUBE() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
}


void process(const ProcessArgs &args) override {
	gainX = 0.5f; gainY = 0.5f;
	if (inputs[X_INPUT].isConnected()) gainX=inputs[X_INPUT].getVoltage();
	if (inputs[Y_INPUT].isConnected()) gainY=inputs[Y_INPUT].getVoltage();

       	for(int i=0; i<8; i++)
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
		
	if (frameX<100) frameX=frameX+gainX* args.sampleTime; else frameX=0;
	if (frameY<100) frameY=frameY+gainY* args.sampleTime; else frameY=0;


	outputs[X_OUTPUT].setVoltage(z[0]*5.0);
};
};

struct CUBEDisplay : TransparentWidget {
	CUBE *module;


	CUBEDisplay() {

	}
	
	void drawLayer(const DrawArgs &args, int layer) override {

if (module) {
if (layer ==1) {
		nvgStrokeColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, module->x[0]*20,module->y[0]*20);
			nvgLineTo(args.vg, module->x[1]*20,module->y[1]*20);
			nvgLineTo(args.vg, module->x[2]*20,module->y[2]*20);
			nvgLineTo(args.vg, module->x[3]*20,module->y[3]*20);
			nvgClosePath(args.vg);
		}
		nvgStroke(args.vg);

		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, module->x[4]*20,module->y[4]*20);
			nvgLineTo(args.vg, module->x[5]*20,module->y[5]*20);
			nvgLineTo(args.vg, module->x[6]*20,module->y[6]*20);
			nvgLineTo(args.vg, module->x[7]*20,module->y[7]*20);
			nvgClosePath(args.vg);
		}
		nvgStroke(args.vg);

		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, module->x[0]*20,module->y[0]*20);
			nvgLineTo(args.vg, module->x[4]*20,module->y[4]*20);
			nvgClosePath(args.vg);
		}
		nvgStroke(args.vg);

		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, module->x[1]*20,module->y[1]*20);
			nvgLineTo(args.vg, module->x[5]*20,module->y[5]*20);
			nvgClosePath(args.vg);
		}
		nvgStroke(args.vg);

		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, module->x[2]*20,module->y[2]*20);
			nvgLineTo(args.vg, module->x[6]*20,module->y[6]*20);
			nvgClosePath(args.vg);
		}
		nvgStroke(args.vg);

		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, module->x[3]*20,module->y[3]*20);
			nvgLineTo(args.vg, module->x[7]*20,module->y[7]*20);
			nvgClosePath(args.vg);
		}
		nvgStroke(args.vg);

	};};
Widget::drawLayer(args, layer);
}
};



struct CUBEWidget : ModuleWidget {
	CUBEWidget(CUBE *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CUBE.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	{
		CUBEDisplay *display = new CUBEDisplay();
		display->box.pos = Vec(60, 120);
		display->module = module;
		addChild(display);
	}

	addInput(createInput<PJ301MPort>(Vec(15, 321), module, CUBE::X_INPUT));
	addInput(createInput<PJ301MPort>(Vec(47, 321), module, CUBE::Y_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(80, 321), module, CUBE::X_OUTPUT));       
	
}
};

Model *modelCUBE = createModel<CUBE, CUBEWidget>("CUBE");