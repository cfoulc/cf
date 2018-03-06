


#include "cf.hpp"


struct DAVE : Module {
	enum ParamIds {
		
		NUM_PARAMS
	};
	enum InputIds {

		NUM_INPUTS
	};
	enum OutputIds {

		NUM_OUTPUTS
	};


	DAVE() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;

};


void DAVE::step() {

}


DAVEWidget::DAVEWidget() {
	DAVE *module = new DAVE();
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/DAVE.svg")));
		addChild(panel);
	}



}
