#include "plugin.hpp"
#include "osdialog.h"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include <vector>
#include "cmath"
#include <dirent.h>
#include <algorithm> //----added by Joakim Lindbom

using namespace std;


struct PLAY : Module {
	enum ParamIds {
		PREV_PARAM,
		NEXT_PARAM,
		LSPEED_PARAM,
		NUM_PARAMS 
	};
	enum InputIds {
		TRIG_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	

	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 totalSampleCount;

	vector<vector<float>> playBuffer;
	bool loading = false;

	bool run = false;

	std::string lastPath = "";
	float samplePos = 0;
	std::string fileDesc;
	bool fileLoaded = false;
	bool reload = false ;
	int sampnumber = 0;
	vector <std::string> fichier;
	dsp::SchmittTrigger loadsampleTrigger;
	dsp::SchmittTrigger playTrigger;
	dsp::SchmittTrigger nextTrigger;
	dsp::SchmittTrigger prevTrigger;


PLAY() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	configParam(PREV_PARAM, 0.f, 1.f, 0.f);
	configParam(NEXT_PARAM, 0.f, 1.f, 0.f);
	configParam(LSPEED_PARAM, -5.0f, 5.0f, 0.0f, "Speed");
		playBuffer.resize(1);
		playBuffer[0].resize(0); 
}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		// lastPath
		json_object_set_new(rootJ, "lastPath", json_string(lastPath.c_str()));	
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
	}



void loadSample(std::string path) {
loading = true;

		unsigned int c;
  		unsigned int sr;
  		drwav_uint64 sc;
		float *pSampleData;
		pSampleData = drwav_open_and_read_file_f32(path.c_str(), &c, &sr, &sc);

	if (pSampleData != NULL) {
		channels = c;
		sampleRate = sr;
		playBuffer[0].clear();
		for (unsigned int i=0; i < sc; i = i + c) {
			playBuffer[0].push_back(pSampleData[i]);
			
		}
		totalSampleCount = playBuffer[0].size();
		drwav_free(pSampleData);
loading = false;

		fileLoaded = true;
		
		fileDesc = rack::string::filename(path);

		if (reload) {
			DIR* rep = NULL;
			struct dirent* dirp = NULL;
			std::string dir = path.empty() ? NULL : rack::string::directory(path); /////////////////////////////////////////

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
		if (nextTrigger.process(params[NEXT_PARAM].getValue()))
			{
			std::string dir = lastPath.empty() ? NULL : rack::string::directory(lastPath); ///////////////////////assetLocal()
			if (sampnumber < int(fichier.size()-1)) sampnumber=sampnumber+1; else sampnumber =0;
			 loadSample(dir + "/" + fichier[sampnumber]);
			}
				
			
		if (prevTrigger.process(params[PREV_PARAM].getValue()))
			{
			std::string dir = lastPath.empty() ? NULL : rack::string::directory(lastPath); /////////////////////////////////////////
			if (sampnumber > 0) sampnumber=sampnumber-1; else sampnumber =int(fichier.size()-1);
			loadSample(dir + "/" + fichier[sampnumber]);
			} 
	} else fileDesc = "R.Click to load";



////////////////////////////////////////////////////////////////// Play   
	if (inputs[TRIG_INPUT].isConnected()) {
		if (playTrigger.process(inputs[TRIG_INPUT].getVoltage())) 
			{
			run = true;
			samplePos = 0;
			}
		}
    
	if ((!loading) && (run) && ((abs(floor(samplePos)) < totalSampleCount))) 
		{ if (samplePos>=0) 
			outputs[OUT_OUTPUT].setVoltage(5 * playBuffer[0][floor(samplePos)]);
		  else outputs[OUT_OUTPUT].setVoltage(5 * playBuffer[0][floor(totalSampleCount-1+samplePos)]);
		samplePos = samplePos+1+(params[LSPEED_PARAM].getValue()) /3;
		}
		else
		{ 
		run = false;
		outputs[OUT_OUTPUT].setVoltage(0);
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



struct PLAYDisplay : TransparentWidget {
	PLAY *module;

	int frame = 0;
	shared_ptr<Font> font;

	PLAYDisplay() {
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/LEDCalculator.ttf"));
	}
	
	void draw(const DrawArgs &args) override {
std::string fD= module ? module->fileDesc : "load sample";
		std::string to_display = "";
		for (int i=0; i<14; i++) to_display = to_display + fD[i];
		nvgFontSize(args.vg, 24);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 0);
		nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0xff));
		nvgRotate(args.vg, -M_PI / 2.0f);	
		nvgTextBox(args.vg, 5, 5,350, to_display.c_str(), NULL);
	}
};

struct PLAYItem : MenuItem {
	PLAY *rm ;
	void onAction(const event::Action &e) override {
		
		//std::string dir = module->lastPath.empty() ? NULL : rack::string::directory(module->lastPath); /////////////////////////////////////////
		char *path = osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL);        //////////dir.c_str(),
		if (path) {
			rm->run = false;
			rm->reload = true;
			rm->loadSample(path);

			rm->samplePos = 0;
			rm->lastPath = std::string(path);
			free(path);
		}


	}

};

struct PLAYWidget : ModuleWidget {
	PLAYWidget(PLAY *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PLAY.svg")));


	{
		PLAYDisplay *gdisplay = new PLAYDisplay();
		gdisplay->box.pos = Vec(18, 253);
		gdisplay->box.size = Vec(130, 250);
		gdisplay->module = module;
		addChild(gdisplay);
	}

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addParam(createParam<Trimpot>(Vec(6, 298), module, PLAY::LSPEED_PARAM));
	addInput(createInput<PJ301MPort>(Vec(3, 31), module, PLAY::TRIG_INPUT));
	addOutput(createOutput<PJ301MPort>(Vec(3, 321), module, PLAY::OUT_OUTPUT));

	addParam(createParam<upButton>(Vec(6, 276), module, PLAY::PREV_PARAM));
	addParam(createParam<downButton>(Vec(6, 256), module, PLAY::NEXT_PARAM));
}

void appendContextMenu(Menu *menu) override {
		PLAY *module = dynamic_cast<PLAY*>(this->module);
		assert(module);

		menu->addChild(new MenuEntry);

		PLAYItem *rootDirItem = new PLAYItem;
		rootDirItem->text = "Load Sample";
		rootDirItem->rm = module;
		menu->addChild(rootDirItem);

		}

};


Model *modelPLAY = createModel<PLAY, PLAYWidget>("PLAY");
