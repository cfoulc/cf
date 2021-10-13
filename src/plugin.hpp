#include "rack.hpp"

using namespace rack;


extern Plugin *pluginInstance;

struct cfBigKnob : RoundKnob {
	cfBigKnob() {
		setSvg(Svg::load(asset::plugin(pluginInstance,"res/cfBigKnob.svg")));
		bg->setSvg(Svg::load(asset::plugin(pluginInstance,"res/cfBigKnob-bg.svg")));
	}

};

struct cfTrimpot : RoundKnob {
	cfTrimpot() {
		setSvg(Svg::load(asset::plugin(pluginInstance,"res/cfTrimpot.svg")));
		bg->setSvg(Svg::load(asset::plugin(pluginInstance,"res/cfTrimpot-bg.svg")));
	}

};

////////////////////
// module widgets
////////////////////

extern Model *modelMETRO;
extern Model *modelEACH;
extern Model *modeltrSEQ;
extern Model *modelLEDSEQ;
extern Model *modelL3DS3Q;
extern Model *modelSLIDERSEQ;
extern Model *modelPLAYER;
extern Model *modelPLAY;
extern Model *modelMONO;
extern Model *modelSTEREO;
extern Model *modelSUB;
extern Model *modelMASTER;
extern Model *modelVARIABLE;
extern Model *modelALGEBRA;
extern Model *modelFUNKTION;
extern Model *modelCHOKE;
extern Model *modelFOUR;
extern Model *modelSTEPS;
extern Model *modelPEAK;
extern Model *modelCUTS;
extern Model *modelBUFFER;
extern Model *modelDISTO;
extern Model *modelCUBE;
extern Model *modelPATCH;
extern Model *modelLABEL;
extern Model *modelDAVE;
