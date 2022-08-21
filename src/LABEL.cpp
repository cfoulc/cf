#include "plugin.hpp"
#include "osdialog.h"
#include <dirent.h>


using namespace std;


struct LABEL : Module {
	enum ParamIds {
		NUM_PARAMS 
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	

		std::string fileDesc = "Right click to write";
	


LABEL() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		// lastPath
		json_object_set_new(rootJ, "lastPath", json_string(fileDesc.c_str()));	
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		// lastPath
		json_t *lastPathJ = json_object_get(rootJ, "lastPath");
		if (lastPathJ) {
			fileDesc = json_string_value(lastPathJ);
			//fileDesc = lastPath;
			
		}
	}



void process(const ProcessArgs &args) override {

}

};





struct LABELDisplay : TransparentWidget {
	LABEL *module;

	int frame = 0;
	

	LABELDisplay() {
		
	}
	
	void drawLayer(const DrawArgs &args, int layer) override {
if (layer ==1) {
shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, "res/LEDCalculator.ttf"));
std::string to_display= module ? module->fileDesc : "Right click to write";
		to_display.resize(20);
		nvgFontSize(args.vg, 24);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 0);
		nvgFillColor(args.vg, nvgRGBA(0x4c, 0xc7, 0xf3, 0xff));
		nvgRotate(args.vg, -M_PI / 2.0f);	
		nvgTextBox(args.vg, 5, 5,350, to_display.c_str(), NULL);
	}
Widget::drawLayer(args, layer);
}
};

struct LABELItem : MenuItem {
	LABEL *rm ;
	void onAction(const event::Action &e) override {
		
		char *path = osdialog_prompt(OSDIALOG_INFO, "Label :", "");
		if (path) {
			rm->fileDesc = std::string(path);
			free(path);
		}


	}

};

struct LABELWidget : ModuleWidget {
	LABELWidget(LABEL *module){
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LABEL.svg")));


	{
		LABELDisplay *ldisplay = new LABELDisplay();
		ldisplay->box.pos = Vec(18, 333);
		ldisplay->box.size = Vec(130, 250);
		ldisplay->module = module;
		addChild(ldisplay);
	}

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));


}

void appendContextMenu(Menu *menu) override {
		LABEL *module = dynamic_cast<LABEL*>(this->module);
		assert(module);

		menu->addChild(new MenuEntry);

		LABELItem *rootDirItem = new LABELItem;
		rootDirItem->text = "Enter label";
		rootDirItem->rm = module;
		menu->addChild(rootDirItem);

		}

};


Model *modelLABEL = createModel<LABEL, LABELWidget>("LABEL");

