#include "cf.hpp"


Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = "cf";
#ifdef VERSION
	p->version = TOSTRING(VERSION);
#endif
	p->addModel(createModel<PLAYERWidget>("cf", "PLAYER", "PLAYER"));
	p->addModel(createModel<STEPSWidget>("cf", "STEPS", "STEPS"));
p->addModel(createModel<DAVEWidget>("cf", "DAVE", "DAVE"));
}
