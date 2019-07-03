#include "plugin.hpp"



struct trSEQ : Module {
	enum ParamIds {
		CLOCK_PARAM,
		RUN_PARAM,
		RESET_PARAM,
		NOTESIN_PARAM,
		CLEAR_PARAM,
		ENUMS(STEPS_PARAM,16),
		ENUMS(GATE_PARAM,16),
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK_INPUT,
		EXT_CLOCK_INPUT,
		RESET_INPUT,
		NOTESIN_INPUT,
		CLEAR_INPUT,
		STEPS_INPUT,
		ENUMS(GATE_INPUT,16),
		NUM_INPUTS
	};
	enum OutputIds {
		GATES_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		RUNNING_LIGHT,
		RESET_LIGHT,
		GATES_LIGHT,
		ENUMS(GATE_LIGHTS,16),
		NUM_LIGHTS
	};

	bool running = true;
	dsp::SchmittTrigger clockTrigger; // for external clock
	// For buttons
	dsp::SchmittTrigger runningTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger gateTriggers[32];

	float phase = 0.0;
	int index = 0;
	bool gateState[16] = {};
	float resetLight = 0.0;
	float stepLights[16] = {};

	enum GateMode {
		TRIGGER,
		RETRIGGER,
		CONTINUOUS,
	};
	GateMode gateMode = TRIGGER;
	dsp::PulseGenerator gatePulse;

	trSEQ() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CLOCK_PARAM, -2.f, 6.f, 2.f, "Clock tempo", " bpm", 2.f, 60.f);
		configParam(RUN_PARAM, 0.f, 1.f, 0.f);
		configParam(RESET_PARAM, 0.f, 1.f, 0.f);
		configParam(STEPS_PARAM, 1.0f, 16.0f, 16.0f);
		for (int i = 0; i < 16; i++) {
			configParam(GATE_PARAM + i, 0.f, 1.f, 0.f);
		}
		configParam(NOTESIN_PARAM, 0.f, 1.f, 0.f);
		configParam(CLEAR_PARAM, 0.0f, 1.0f, 0.0f);

		onReset();
	}

	void onReset() override {
		for (int i = 0; i < 16; i++) {
			gateState[i] = false;
		}
	}

	void onRandomize() override {
		for (int i = 0; i < 16; i++) {
			gateState[i] = (random::uniform() > 0.5);
		}
	}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		// running
		json_object_set_new(rootJ, "running", json_boolean(running));

		// gates
		json_t *gatesJ = json_array();
		for (int i = 0; i < 16; i++) {
			json_t *gateJ = json_integer((int) gateState[i]);
			json_array_append_new(gatesJ, gateJ);
		}
		json_object_set_new(rootJ, "gates", gatesJ);

		// gateMode
		json_t *gateModeJ = json_integer((int) gateMode);
		json_object_set_new(rootJ, "gateMode", gateModeJ);

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// running
		json_t *runningJ = json_object_get(rootJ, "running");
		if (runningJ)
			running = json_is_true(runningJ);

		// gates
		json_t *gatesJ = json_object_get(rootJ, "gates");
		if (gatesJ) {
			for (int i = 0; i < 16; i++) {
				json_t *gateJ = json_array_get(gatesJ, i);
				if (gateJ)
					gateState[i] = !!json_integer_value(gateJ);
			}
		}

	}

	



void process(const ProcessArgs &args) override {
	const float lightLambda = 0.075;
	// Run
	if (runningTrigger.process(params[RUN_PARAM].getValue())) {
		running = !running;
	}
	lights[RUNNING_LIGHT].setBrightness(running ? 1.0 : 0.0);

	bool nextStep = false;


	if (running) {
		if (inputs[EXT_CLOCK_INPUT].isConnected()) {
			// External clock
			if (clockTrigger.process(inputs[EXT_CLOCK_INPUT].getVoltage())) {
				phase = 0.0;
				nextStep = true;
			}
		}
		else {
			// Internal clock
			float clockTime = powf(2.0, params[CLOCK_PARAM].getValue() + inputs[CLOCK_INPUT].getVoltage());
			phase += clockTime * args.sampleTime;
			if (phase >= 1.0) {
				phase -= 1.0;
				nextStep = true;
			}
		}
	}

	// Reset
	if (resetTrigger.process(params[RESET_PARAM].getValue() + inputs[RESET_INPUT].getVoltage())) {
		phase = 0.0;
		index = 16;
		nextStep = true;
		resetLight = 1.0;
	}

	if (nextStep) {
		// Advance step
		int numSteps = clamp(static_cast<int>(round(params[STEPS_PARAM].getValue() + inputs[STEPS_INPUT].getVoltage())), 1, 16);

		index += 1;

		if (index >= numSteps) {
			index = 0;
		}

	if (!inputs[NOTESIN_INPUT].isConnected()) inputs[NOTESIN_INPUT].setVoltage(0);
	if (!inputs[CLEAR_INPUT].isConnected()) inputs[CLEAR_INPUT].setVoltage(0);
	if (params[NOTESIN_PARAM].getValue() or inputs[NOTESIN_INPUT].getVoltage()>0) gateState[index] = true;
	if (params[CLEAR_PARAM].getValue() or inputs[CLEAR_INPUT].getVoltage()>0) gateState[index] = false;

		stepLights[index] = 1.0;
		gatePulse.trigger(1e-3);
	}

	resetLight -= resetLight / lightLambda * args.sampleTime;

	bool pulse = gatePulse.process(1.0 * args.sampleTime);

	// Gate buttons
	for (int i = 0; i < 16; i++) {


		if (gateTriggers[i+16].process(inputs[GATE_INPUT + i].getVoltage())) {
			gateState[i] = !gateState[i];
		}

		if (gateTriggers[i].process(params[GATE_PARAM + i].getValue())) {
			gateState[i] = !gateState[i];
		}

		bool gateOn = (running && i == index && gateState[i]);
		if (gateMode == TRIGGER)
			gateOn = gateOn && pulse;
		else if (gateMode == RETRIGGER)
			gateOn = gateOn && !pulse;

		//outputs[GATE_OUTPUT + i].setVoltage(gateOn ? 10.0 : 0.0);
		stepLights[i] -= stepLights[i] / lightLambda * args.sampleTime;
		lights[GATE_LIGHTS + i].setBrightness(gateState[i] ? 1.0 - stepLights[i] : stepLights[i]);
	}

	// Rows

	bool gatesOn = (running && gateState[index]);
	if (gateMode == TRIGGER)
		gatesOn = gatesOn && pulse;
	else if (gateMode == RETRIGGER)
		gatesOn = gatesOn && !pulse;

	// Outputs

	outputs[GATES_OUTPUT].setVoltage(gatesOn ? 10.0 : 0.0);
	lights[RESET_LIGHT].setBrightness(resetLight);
	lights[GATES_LIGHT].setBrightness(gatesOn ? 1.0 : 0.0);
	}
};


//struct PadButton : SVGSwitch, MomentarySwitch {
//	PadButton() {
//		addFrame(loadSvg(assetPlugin(plugin, "res/PadButton.svg")));
//		addFrame(loadSvg(assetPlugin(plugin, "res/PadButtonDown.svg")));
//		sw->wrap();
//		box.size = sw->box.size;
//	}
//};

struct PadButton : app::SvgSwitch {
	PadButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PadButton.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PadButtonDown.svg")));
	}
};

struct trSEQWidget : ModuleWidget {
	trSEQWidget(trSEQ *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/trSEQ.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addParam(createParam<RoundBlackKnob>(Vec(18, 56), module, trSEQ::CLOCK_PARAM));
	addParam(createParam<LEDButton>(Vec(60, 61-1), module, trSEQ::RUN_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(64.4, 64.4), module, trSEQ::RUNNING_LIGHT));
	addParam(createParam<LEDButton>(Vec(99, 61-1), module, trSEQ::RESET_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(103.4, 64.4), module, trSEQ::RESET_LIGHT));
	addParam(createParam<RoundBlackSnapKnob>(Vec(132, 56), module, trSEQ::STEPS_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(289.4, 64.4), module, trSEQ::GATES_LIGHT));

	static const float portX[8] = {20, 58, 96, 135, 173, 212, 250, 289};


	addParam(createParam<PadButton>(Vec(portX[5]-26, 56), module, trSEQ::NOTESIN_PARAM));
	addParam(createParam<PadButton>(Vec(portX[6]-26, 56), module, trSEQ::CLEAR_PARAM));
	addInput(createInput<PJ301MPort>(Vec(portX[5]-24, 98), module, trSEQ::NOTESIN_INPUT));
	addInput(createInput<PJ301MPort>(Vec(portX[6]-24, 98), module, trSEQ::CLEAR_INPUT));


	addInput(createInput<PJ301MPort>(Vec(portX[0]-1, 98), module, trSEQ::CLOCK_INPUT));
	addInput(createInput<PJ301MPort>(Vec(portX[1]-1, 98), module, trSEQ::EXT_CLOCK_INPUT));
	addInput(createInput<PJ301MPort>(Vec(portX[2]-1, 98), module, trSEQ::RESET_INPUT));
	addInput(createInput<PJ301MPort>(Vec(portX[3]-1, 98), module, trSEQ::STEPS_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(portX[7]-6.5, 98), module, trSEQ::GATES_OUTPUT));


	for (int i = 0; i < 16; i++) {

		addParam(createParam<LEDButton>(Vec(i*19+12, 203-1), module, trSEQ::GATE_PARAM + i));
		addChild(createLight<MediumLight<BlueLight>>(Vec(i*19+16.4, 206.4), module, trSEQ::GATE_LIGHTS + i));
		addInput(createInput<PJ301MPort>(Vec(i*19+9, 247+ 40*(i%2)), module, trSEQ::GATE_INPUT + i));
	}

};
};


Model *modeltrSEQ = createModel<trSEQ, trSEQWidget>("trSEQ");
