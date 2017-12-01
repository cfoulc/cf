#include "cf.hpp"


Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = "cf";
#ifdef VERSION
	p->version = TOSTRING(VERSION);
#endif
	p->addModel(createModel<PLAYERWidget>("cf", "PLAYER", "Player", SAMPLER_TAG));
	p->addModel(createModel<STEPSWidget>("cf", "STEPS", "Steps", UTILITY_TAG));
    p->addModel(createModel<PEAKWidget>("cf", "PEAK", "Peak", UTILITY_TAG));
    p->addModel(createModel<FOURWidget>("cf", "FOUR", "Four", UTILITY_TAG));
p->addModel(createModel<CUBEWidget>("cf", "CUBE", "Cube", UTILITY_TAG));
	p->addModel(createModel<trSEQWidget>("cf", "trSEQ", "trSEQ", SEQUENCER_TAG));
	p->addModel(createModel<DAVEWidget>("cf", "DAVE", "Dave", BLANK_TAG));

}
