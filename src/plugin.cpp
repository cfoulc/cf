#include "plugin.hpp"


Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;
	
	p->addModel(modelMETRO);
	p->addModel(modelEACH);
	p->addModel(modeltrSEQ);
	p->addModel(modelLEDSEQ);
	p->addModel(modelL3DS3Q);
	p->addModel(modelSLIDERSEQ);
	p->addModel(modelPLAYER);
	p->addModel(modelPLAY);
	p->addModel(modelMONO);
	p->addModel(modelSTEREO);
	p->addModel(modelSUB);
	p->addModel(modelMASTER);
	p->addModel(modelVARIABLE);
	p->addModel(modelALGEBRA);
	p->addModel(modelFUNKTION);
	p->addModel(modelCHOKE);
	p->addModel(modelFOUR);
	p->addModel(modelSTEPS);
	p->addModel(modelPEAK);
	p->addModel(modelCUTS);
	p->addModel(modelBUFFER);
	p->addModel(modelDISTO);
	p->addModel(modelCUBE);
	p->addModel(modelPATCH);
	p->addModel(modelLABEL);
	p->addModel(modelDAVE);

}
