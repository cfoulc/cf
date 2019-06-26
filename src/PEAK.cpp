#include "plugin.hpp"


struct PEAK : Module {
	enum ParamIds {
		TRESHOLD_PARAM,
        	GAIN_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		LIN1_INPUT,
		IN1_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};
    enum LightIds {
		TRESHOLD_LIGHT,
		OVER_LIGHT,
		NUM_LIGHTS
	};
float max_GAIN = 1.0 ;
int affich = 1 ;
int reman_t = 0;
int reman_o = 0;
int sensiv = 10000;


	PEAK() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(GAIN_PARAM, 0.0f, 10.0f, 1.0f, "Gain");
		configParam(TRESHOLD_PARAM, 0.0f, 10.0f, 10.0f, "Treshold");
}


void process(const ProcessArgs &args) override {

	max_GAIN = roundf(params[GAIN_PARAM].getValue()*10);

	affich = round(max_GAIN);

	if (inputs[IN1_INPUT].isConnected())
	{

        if (inputs[IN1_INPUT].getVoltage() > params[TRESHOLD_PARAM].getValue())
        	{
            	outputs[OUT1_OUTPUT].setVoltage((max_GAIN/10.0*(params[TRESHOLD_PARAM].getValue() + ((inputs[IN1_INPUT].getVoltage()-params[TRESHOLD_PARAM].getValue())/(1+(inputs[IN1_INPUT].getVoltage()-params[TRESHOLD_PARAM].getValue()))))));
            	reman_t = sensiv;
        	}
        else if (inputs[IN1_INPUT].getVoltage() < 0-params[TRESHOLD_PARAM].getValue())
        	{
            	outputs[OUT1_OUTPUT].setVoltage((max_GAIN/10.0*(0-(params[TRESHOLD_PARAM].getValue() - ((inputs[IN1_INPUT].getVoltage()+params[TRESHOLD_PARAM].getValue())/(1+(-inputs[IN1_INPUT].getVoltage()-params[TRESHOLD_PARAM].getValue())))))));
            	reman_t = sensiv;
        	}
        	else 
		{
          	  outputs[OUT1_OUTPUT].setVoltage((max_GAIN*inputs[IN1_INPUT].getVoltage())/10.0);
       		}

        if (outputs[OUT1_OUTPUT].getVoltage() >10) reman_o=sensiv;
        
	}
	else
	{
	outputs[OUT1_OUTPUT].setVoltage(max_GAIN/10);
        lights[TRESHOLD_LIGHT].value = 0.0;
        lights[OVER_LIGHT].value = 0.0;
	}



	if (inputs[IN2_INPUT].isConnected())
	{

        if (inputs[IN2_INPUT].getVoltage() > params[TRESHOLD_PARAM].getValue())
        	{
            	outputs[OUT2_OUTPUT].setVoltage((max_GAIN/10.0*(params[TRESHOLD_PARAM].getValue() + ((inputs[IN2_INPUT].getVoltage()-params[TRESHOLD_PARAM].getValue())/(1+(inputs[IN2_INPUT].getVoltage()-params[TRESHOLD_PARAM].getValue()))))));
            	reman_t = sensiv;
        	}
        else if (inputs[IN2_INPUT].getVoltage() < 0-params[TRESHOLD_PARAM].getValue())
        	{
            	outputs[OUT2_OUTPUT].setVoltage((max_GAIN/10.0*(0-(params[TRESHOLD_PARAM].getValue() - ((inputs[IN2_INPUT].getVoltage()+params[TRESHOLD_PARAM].getValue())/(1+(-inputs[IN2_INPUT].getVoltage()-params[TRESHOLD_PARAM].getValue())))))));
            	reman_t = sensiv;
        	}
        	else 
		{
          	  outputs[OUT2_OUTPUT].setVoltage((max_GAIN*inputs[IN2_INPUT].getVoltage())/10.0);
       		}

        if (outputs[OUT2_OUTPUT].getVoltage() >10) reman_o=sensiv;
        
	}
	else
	{
	outputs[OUT2_OUTPUT].setVoltage(max_GAIN/10);
        lights[TRESHOLD_LIGHT].value = 0.0;
        lights[OVER_LIGHT].value = 0.0;
	}


	if (reman_t >0) 
	{
	reman_t--;
	lights[TRESHOLD_LIGHT].value = 1;
	} 
	else lights[TRESHOLD_LIGHT].value = 0.0;

	if (reman_o >0) 
	{
	reman_o--;
	lights[OVER_LIGHT].value = 1;
	} 
	else lights[OVER_LIGHT].value = 0.0;
};
};

struct NumbDisplayWidget : TransparentWidget {
PEAK *module;
//  float *value=0;
  std::shared_ptr<Font> font;

  NumbDisplayWidget() {
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
  };

  void draw(const DrawArgs &args) override {
int st = module ? module->affich : 0;
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

    std::string to_display = std::to_string(st);
    if (st<10) to_display = '0' + to_display;
if (st<100) to_display = ' ' + to_display;


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
    nvgFillColor(args.vg, textColor);
    nvgText(args.vg, textPos.x+1, textPos.y, " . ", NULL);
    nvgText(args.vg, textPos.x+1, textPos.y-1, " . ", NULL);

  }
};


struct PEAKWidget : ModuleWidget {
	PEAKWidget(PEAK *module) {
setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PEAK.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

    	addParam(createParam<RoundLargeBlackKnob>(Vec(27, 97), module, PEAK::GAIN_PARAM));
 		addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 141.4), module, PEAK::OVER_LIGHT));

	addParam(createParam<RoundLargeBlackKnob>(Vec(27, 227), module, PEAK::TRESHOLD_PARAM));
		addChild(createLight<MediumLight<BlueLight>>(Vec(42.4, 211.4), module, PEAK::TRESHOLD_LIGHT));

	addInput(createInput<PJ301MPort>(Vec(11, 308), module, PEAK::IN1_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(54, 308), module, PEAK::OUT1_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(11, 334), module, PEAK::IN2_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(54, 334), module, PEAK::OUT2_OUTPUT));

NumbDisplayWidget *display = new NumbDisplayWidget();
	display->box.pos = Vec(20,56);
	display->box.size = Vec(50, 20);
	//display->value = &module->affich;
	display->module = module;
	addChild(display);

	
}
};
Model *modelPEAK = createModel<PEAK, PEAKWidget>("PEAK");
