#include "plugin.hpp"
#include "osdialog.h"
//#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include <vector>
#include "cmath"
#include <dirent.h>
#include <libgen.h>
#include <algorithm> //----added by Joakim Lindbom


using namespace std;


struct PLAYER : Module {
	enum ParamIds {
		LSTART_PARAM,
		LSPEED_PARAM,
		TSTART_PARAM,
		TSPEED_PARAM,
		NEXT_PARAM,
		PREV_PARAM,
		OSC_PARAM,
		NUM_PARAMS 
	};
	enum InputIds {
		GATE_INPUT,
		POS_INPUT,
        SPD_INPUT,
	PREV_INPUT,
	NEXT_INPUT,
		TRIG_INPUT,
		VO_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		OSC_LIGHT,
		NUM_LIGHTS
	};
	
	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 totalSampleC;

	vector<vector<float>> playBuffer;
	bool loading = false;

	bool play = false;

	std::string lastPath = "";


	float samplePos = 0;
 	float startPos = 0;
	vector<double> displayBuff;

	std::string fileDesc;
	bool fileLoaded = false;

	dsp::SchmittTrigger playTrigger;
	dsp::SchmittTrigger playGater;
	dsp::SchmittTrigger nextTrigger;
	dsp::SchmittTrigger prevTrigger;
	dsp::SchmittTrigger nextinTrigger;
	dsp::SchmittTrigger previnTrigger;
	dsp::SchmittTrigger oscTrigger;

	vector <std::string> fichier;

	int sampnumber = 0;
	int retard = 0;
	bool reload = false ;
	bool oscState = false ;


PLAYER() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	configButton(PREV_PARAM, "Previous sample");
	configButton(NEXT_PARAM, "Next sample");
	configButton(OSC_PARAM, "Osc Mode");
	configParam(TSPEED_PARAM, -1.0f, 1.0f, 0.0f, "Speed trim");
	configParam(TSTART_PARAM, -1.0f, 1.0f, 0.0f, "Start trim");
	configParam(LSPEED_PARAM, -5.0f, 5.0f, 0.0f, "Speed");
	configParam(LSTART_PARAM, 0.0f, 10.0f, 0.0f, "Start");
	configInput(GATE_INPUT,"Gated sample play");
	configInput(POS_INPUT,"Start position control");
        configInput(SPD_INPUT,"Speed control");
	configInput(PREV_INPUT,"Previous sample trigger");
	configInput(NEXT_INPUT,"Next sample trigger");
	configInput(TRIG_INPUT,"Triggered sample play");
	configInput(VO_INPUT,"Volt/octave control");
	configOutput(OUT_OUTPUT,"Left");
	configOutput(OUT2_OUTPUT,"Right");
			playBuffer.resize(2);
			playBuffer[0].resize(0);
			playBuffer[1].resize(0); 
}


	
	// persistence
	
	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		// lastPath
		json_object_set_new(rootJ, "lastPath", json_string(lastPath.c_str()));	
json_object_set_new(rootJ, "oscstate", json_integer(oscState));
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// lastPath
		json_t *lastPathJ = json_object_get(rootJ, "lastPath");
		if (lastPathJ) {
			lastPath = json_string_value(lastPathJ);
			reload = true ;
			loadSample(lastPath);
			
		}
		json_t *oscstateJ = json_object_get(rootJ, "oscstate");
		if (oscstateJ) {
			oscState = json_integer_value(oscstateJ);
			lights[OSC_LIGHT].setBrightness(oscState);
		}
	}


void loadSample(std::string path) {

		loading = true;
		unsigned int c;
  		unsigned int sr;
  		drwav_uint64 tsc;
		float* pSampleData;
		pSampleData = drwav_open_and_read_file_f32(path.c_str(), &c, &sr, &tsc);

	if (pSampleData != NULL) {
		channels = c;
		sampleRate = sr;
		playBuffer[0].clear();
		playBuffer[1].clear();
		for (unsigned int i=0; i < tsc; i = i + c) {
			playBuffer[0].push_back(pSampleData[i]);
			if (channels == 2)
				playBuffer[1].push_back((float)pSampleData[i+1]);
			
		}
		totalSampleC = playBuffer[0].size();
		drwav_free(pSampleData);
loading = false;


		fileLoaded = true;
		vector<double>().swap(displayBuff);
		for (int i=0; i < floor(totalSampleC); i = i + floor(totalSampleC/130)) {
			displayBuff.push_back(playBuffer[0][i]);
		}
			char* pathDup = strdup(path.c_str());
			fileDesc = basename(pathDup);
			free(pathDup);
fileDesc += "\n";
		//fileDesc = rack::string::filename(path)+ "\n";
		fileDesc += std::to_string(sampleRate)+ " Hz" + "\n";
		fileDesc += std::to_string(channels)+ " channel(s)" + "\n";

		if (reload) {
			DIR* rep = NULL;
			struct dirent* dirp = NULL;
				char* pathDup = strdup(path.c_str());
				std::string dir = dirname(pathDup);
				free(pathDup);
			//std::string dir = path.empty() ? NULL : rack::string::directory(path);

			rep = opendir(dir.c_str());
			int i = 0;
			fichier.clear();
			while ((dirp = readdir(rep)) != NULL) {
				std::string name = dirp->d_name;

				std::size_t found = name.find(".wav",name.length()-5);
				if (found==std::string::npos) found = name.find(".WAV",name.length()-5);

  				if (found!=std::string::npos) {
					fichier.push_back(name);
					if ((dir + "/" + name)==path) {sampnumber = i;}
					i=i+1;
					}
				
				}

//----added by Joakim Lindbom
		sort(fichier.begin(), fichier.end());  // Linux needs this to get files in right order
            for (int o=0;o<int(fichier.size()-1); o++) {
                if ((dir + "/" + fichier[o])==path) {
                    sampnumber = o;
                }
            }
//---------------

			closedir(rep);
			reload = false;
		}
			lastPath = path;
	}
	else {
		
		fileLoaded = false;
	}
};


void process(const ProcessArgs &args) override {

	if (fileLoaded) {
		if (nextTrigger.process(params[NEXT_PARAM].getValue())+nextinTrigger.process(inputs[NEXT_INPUT].getVoltage()))
			{
				char* pathDup = strdup(lastPath.c_str());
				std::string dir = dirname(pathDup);
				free(pathDup);
			//std::string dir = lastPath.empty() ? NULL : rack::string::directory(lastPath);
			if (sampnumber < int(fichier.size()-1)) sampnumber=sampnumber+1; else sampnumber =0;
			loadSample(dir + "/" + fichier[sampnumber]);
			}
				
			
		if (prevTrigger.process(params[PREV_PARAM].getValue())+previnTrigger.process(inputs[PREV_INPUT].getVoltage()))
			{retard = 1000;
				char* pathDup = strdup(lastPath.c_str());
				std::string dir = dirname(pathDup);
				free(pathDup);
			//std::string dir = lastPath.empty() ? NULL : rack::string::directory(lastPath);
			if (sampnumber > 0) sampnumber=sampnumber-1; else sampnumber =int(fichier.size()-1);
			loadSample(dir + "/" + fichier[sampnumber]);
			} 
	} else fileDesc = "right click to load \n .wav sample \n :)";

if (oscTrigger.process(params[OSC_PARAM].getValue()))
			{oscState =!oscState;lights[OSC_LIGHT].setBrightness(oscState);}

	// Play
if (!oscState) {
    bool gated = inputs[GATE_INPUT].getVoltage() > 0;
    
    if (inputs[POS_INPUT].isConnected())
    startPos = clamp((params[LSTART_PARAM].getValue() + inputs[POS_INPUT].getVoltage() * params[TSTART_PARAM].getValue()),0.0f,10.0f)*totalSampleC/10;
    else {startPos = clamp((params[LSTART_PARAM].getValue()),0.0f,10.0f)*totalSampleC/10;
        inputs[POS_INPUT].setVoltage(0) ;
    }
    
    if (!inputs[TRIG_INPUT].isConnected()) {
	if (playGater.process(inputs[GATE_INPUT].getVoltage())) {
		play = true;
		samplePos = startPos;
		}
	} else {
	if (playTrigger.process(inputs[TRIG_INPUT].getVoltage())) {
		play = true;
		samplePos = startPos;
		}
	}
    
	if ((!loading) && (play) && ((floor(samplePos) < totalSampleC) && (floor(samplePos) >= 0))) {
		if (channels == 1) {
			outputs[OUT_OUTPUT].setVoltage(5 * playBuffer[0][floor(samplePos)]);
			outputs[OUT2_OUTPUT].setVoltage(5 * playBuffer[0][floor(samplePos)]);}
		else if (channels ==2) {
			outputs[OUT_OUTPUT].setVoltage(5 * playBuffer[0][floor(samplePos)]);
			outputs[OUT2_OUTPUT].setVoltage(5 * playBuffer[1][floor(samplePos)]);
        		}
		if (inputs[SPD_INPUT].isConnected())
        samplePos = samplePos+powf(2.0, inputs[VO_INPUT].getVoltage())+(params[LSPEED_PARAM].getValue() +inputs[SPD_INPUT].getVoltage() * params[TSPEED_PARAM].getValue()) /3;
        else {
            samplePos = samplePos+powf(2.0, inputs[VO_INPUT].getVoltage())+(params[LSPEED_PARAM].getValue()) /3;
            inputs[SPD_INPUT].setVoltage(0) ;}
	}
	else
	{ 
		play = false;
	    outputs[OUT_OUTPUT].setVoltage(0);outputs[OUT2_OUTPUT].setVoltage(0);
	}
       if (!inputs[TRIG_INPUT].isConnected()) {if (gated == false) {play = false; outputs[OUT_OUTPUT].setVoltage(0);outputs[OUT2_OUTPUT].setVoltage(0);}}
} else {
	
	if (((floor(samplePos) < totalSampleC) && (floor(samplePos) >= 0))) {
		if (playTrigger.process(inputs[TRIG_INPUT].getVoltage())) samplePos = 0;

		if (channels == 1) {
			outputs[OUT_OUTPUT].setVoltage(5 * playBuffer[0][floor(samplePos)]);
			outputs[OUT2_OUTPUT].setVoltage(5 * playBuffer[0][floor(samplePos)]);}
		else if (channels ==2) {
			outputs[OUT_OUTPUT].setVoltage(5 * playBuffer[0][floor(samplePos)]);
			outputs[OUT2_OUTPUT].setVoltage(5 * playBuffer[1][floor(samplePos)]);
        		}
		if (inputs[SPD_INPUT].isConnected())
        samplePos = samplePos+powf(2.0, inputs[VO_INPUT].getVoltage())+(params[LSPEED_PARAM].getValue() +inputs[SPD_INPUT].getVoltage() * params[TSPEED_PARAM].getValue()) /3;
        else {
            samplePos = samplePos+powf(2.0, inputs[VO_INPUT].getVoltage())+(params[LSPEED_PARAM].getValue()) /3;
            inputs[SPD_INPUT].setVoltage(0);}
	}
	else
	{ 
		samplePos=0;
	}

	}
}
};

struct upButton : app::SvgSwitch {
	upButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/upButton.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/upButtonDown.svg")));
	}
};

struct downButton : app::SvgSwitch {
	downButton() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/downButton.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/downButtonDown.svg")));
	};
};


struct PLAYERDisplay : TransparentWidget {
	PLAYER *module;
	int frame = 0;
	

	PLAYERDisplay() {
		

	}
	
	void drawLayer(const DrawArgs &args, int layer) override {
if (module) {
if (layer ==1) {
shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, "res/DejaVuSansMono.ttf"));
		nvgFontSize(args.vg, 12);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, -2);
		nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));	
		nvgTextBox(args.vg, 5, 5,120, module->fileDesc.c_str(), NULL);
		
		// Draw ref line
		nvgStrokeColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0x40));
		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, 0, 125);
			nvgLineTo(args.vg, 125, 125);
			nvgClosePath(args.vg);
		}
		nvgStroke(args.vg);
		
		if (module->fileLoaded) {
			// Draw play line
			nvgStrokeColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
            nvgStrokeWidth(args.vg, 0.8);
			{
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, floor(module->samplePos * 125 / module->totalSampleC) , 85);
				nvgLineTo(args.vg, floor(module->samplePos * 125 / module->totalSampleC) , 165);
				nvgClosePath(args.vg);
			}
			nvgStroke(args.vg);
            
            // Draw start line
			nvgStrokeColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
            nvgStrokeWidth(args.vg, 1.5);
			{
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, floor(module->startPos * 125 / module->totalSampleC) , 85);
				nvgLineTo(args.vg, floor(module->startPos * 125 / module->totalSampleC) , 165);
				nvgClosePath(args.vg);
			}
			nvgStroke(args.vg);
            
			
			// Draw waveform
			nvgStrokeColor(args.vg, nvgRGBA(0xe1, 0x02, 0x78, 0xc0));
			nvgSave(args.vg);
			Rect b = Rect(Vec(0, 85), Vec(125, 80));
			nvgScissor(args.vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
			nvgBeginPath(args.vg);
			for (unsigned int i = 0; i < module->displayBuff.size(); i++) {
				float x, y;
				x = (float)i / (module->displayBuff.size() - 1);
				y = module->displayBuff[i] / 2.0 + 0.5;
				Vec p;
				p.x = b.pos.x + b.size.x * x;
				p.y = b.pos.y + b.size.y * (1.0 - y);
				if (i == 0)
					nvgMoveTo(args.vg, p.x, p.y);
				else
					nvgLineTo(args.vg, p.x, p.y);
			}
			nvgLineCap(args.vg, NVG_ROUND);
			nvgMiterLimit(args.vg, 2.0);
			nvgStrokeWidth(args.vg, 1.5);
			nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
			nvgStroke(args.vg);			
			nvgResetScissor(args.vg);
			nvgRestore(args.vg);	
		}
	}

	}
Widget::drawLayer(args, layer);
}

};

struct PLAYERItem : MenuItem {
	PLAYER *rm ;
	void onAction(const event::Action &e) override {
		
		//std::string dir = module->lastPath.empty() ? NULL : rack::string::directory(module->lastPath); /////////////////////////////////////////
		char *path = osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL);        //////////dir.c_str(),
		if (path) {
			rm->play = false;
			rm->reload = true;
			rm->loadSample(path);

			rm->samplePos = 0;
			rm->lastPath = std::string(path);
			free(path);
		}


	}

};

struct PLAYERWidget : ModuleWidget {
	PLAYERWidget(PLAYER *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PLAYER.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));
	
	{
		PLAYERDisplay *display = new PLAYERDisplay();
		display->box.pos = Vec(5, 40);
		display->box.size = Vec(130, 250);
		display->module = module;
		addChild(display);
	}
		
	static const float portX0[4] = {10, 40, 70, 100};
	

	addParam(createParam<RoundLargeBlackKnob>(Vec(23, 235), module, PLAYER::LSTART_PARAM));
	addParam(createParam<RoundLargeBlackKnob>(Vec(73, 235), module, PLAYER::LSPEED_PARAM));
	addParam(createParam<Trimpot>(Vec(42, 278), module, PLAYER::TSTART_PARAM));
	addParam(createParam<Trimpot>(Vec(73, 278), module, PLAYER::TSPEED_PARAM));

	addInput(createInput<PJ301MPort>(Vec(portX0[0], 321), module, PLAYER::GATE_INPUT));
	addInput(createInput<PJ301MPort>(Vec(portX0[1], 321), module, PLAYER::POS_INPUT));
  	addInput(createInput<PJ301MPort>(Vec(portX0[2], 321), module, PLAYER::SPD_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(portX0[3], 275), module, PLAYER::OUT_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(portX0[3], 321), module, PLAYER::OUT2_OUTPUT));

	addInput(createInput<PJ301MPort>(Vec(portX0[0], 91), module, PLAYER::PREV_INPUT));
	addInput(createInput<PJ301MPort>(Vec(portX0[3], 91), module, PLAYER::NEXT_INPUT));
	addInput(createInput<PJ301MPort>(Vec(portX0[0], 275), module, PLAYER::TRIG_INPUT));
	addParam(createParam<upButton>(Vec(43, 95), module, PLAYER::PREV_PARAM));
	addParam(createParam<downButton>(Vec(73, 95), module, PLAYER::NEXT_PARAM));

	addParam(createParam<LEDButton>(Vec(104, 212), module, PLAYER::OSC_PARAM));
		addChild(createLight<MediumLight<BlueLight>>(Vec(108.4, 216.4), module, PLAYER::OSC_LIGHT));
	addInput(createInput<PJ301MPort>(Vec(portX0[0], 210), module, PLAYER::VO_INPUT));
}

void appendContextMenu(Menu *menu) override {
		PLAYER *module = dynamic_cast<PLAYER*>(this->module);
		assert(module);

		menu->addChild(new MenuEntry);

		PLAYERItem *rootDirItem = new PLAYERItem;
		rootDirItem->text = "Load Sample";
		rootDirItem->rm = module;
		menu->addChild(rootDirItem);

		}

};

Model *modelPLAYER = createModel<PLAYER, PLAYERWidget>("PLAYER");
