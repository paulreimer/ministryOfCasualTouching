#pragma once

#include "ofxMSAFluidDrawer.h"
#include "ofxCvColorImage.h"

class FluidDrawer
: public ofxMSAFluidDrawer
{
public:
	FluidDrawer();

	void draw(float x=0., float y=0., float renderWidth=0., float renderHeight=0.);
	void drawVectorsMasked(float x, float y, float renderWidth, float renderHeight);
	ofxCvColorImage* mask;

	float maxVelThreshold;
	float minVelThreshold;

//protected:
};
