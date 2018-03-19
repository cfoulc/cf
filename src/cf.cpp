#include "cf.hpp"


Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
#ifdef VERSION
	p->version = TOSTRING(VERSION);
#endif
	p->website = "https://github.com/cfoulc/cf";
	p->manual = "https://github.com/cfoulc/cf/blob/master/README.md";

	p->addModel(modeltrSEQ);
	p->addModel(modelLEDSEQ);
	p->addModel(modelL3DS3Q);
	p->addModel(modelPLAYER);
	p->addModel(modelSTEPS);
	p->addModel(modelMETRO);
	p->addModel(modelEACH);
	p->addModel(modelFOUR);
	p->addModel(modelPEAK);
	p->addModel(modelMONO);
	p->addModel(modelSTEREO);
	p->addModel(modelMASTER);
	p->addModel(modelSUB);
	p->addModel(modelCUBE);
	p->addModel(modelPATCH);
	p->addModel(modelLEDS);
	p->addModel(modelDAVE);

}
