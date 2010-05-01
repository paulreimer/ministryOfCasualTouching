#pragma once

#include "ofxMSAFluidDrawer.h"
#include "ofxCvColorImage.h"

class FluidDrawer
: public ofxMSAFluidDrawer
{
public:
	FluidDrawer();

	void initGL(int NX, int NY);

	ofxMSAFluidSolver* setup(int NX = FLUID_DEFAULT_NX, int NY = FLUID_DEFAULT_NY);
	ofxMSAFluidSolver* setup(ofxMSAFluidSolver* f);

	void draw(float x=0., float y=0., float renderWidth=0., float renderHeight=0.);
	void drawVectorsMasked(float x, float y, float renderWidth, float renderHeight);
	void drawTexture(float x, float y, float renderWidth, float renderHeight);
	ofxCvColorImage* mask;

	float maxVelThreshold;
	float minVelThreshold;

protected:
	vector<GLfloat>	tex_coords;
	vector<GLfloat>	verts;
	vector<GLfloat>	normals;
	vector<GLuint>	indices;	
};
