#include "FluidDrawer.h"
#include "settings.h"


//--------------------------------------------------------------
FluidDrawer::FluidDrawer()
{
	mask = NULL;
	maxVelThreshold = FLT_MAX;
	minVelThreshold = 0.0;
}

//--------------------------------------------------------------
void
FluidDrawer::draw(float x, float y, float renderWidth, float renderHeight)
{
	switch(drawMode) {
		case FLUID_DRAW_COLOR:
			drawColor(x, y, renderWidth, renderHeight);
			break;
			
		case FLUID_DRAW_MOTION:
			drawMotion(x, y, renderWidth, renderHeight);
			break;
			
		case FLUID_DRAW_SPEED:
			drawSpeed(x, y, renderWidth, renderHeight);
			break;

		case FLUID_DRAW_VECTORS:
			if (mask != NULL)
				drawVectorsMasked(x, y, renderWidth, renderHeight);
			else
				drawVectors(x, y, renderWidth, renderHeight);
			break;
	}
}

void FluidDrawer::drawVectorsMasked(float x, float y, float renderWidth, float renderHeight)
{
	unsigned char* mask_pix;

	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();
	
	//	int xStep = renderWidth / 10;		// every 10 pixels
	//	int yStep = renderHeight / 10;		// every 10 pixels
	
	if (mask==NULL)
		return;

	mask_pix = mask->getPixels();

	float vx,vy,vw=VIDEO_WIDTH,vh=VIDEO_HEIGHT;
	
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(renderWidth/(fw-2), renderHeight/(fh-2), 1.0);
	
	float velMult = 50000;
	float maxVel = 5./20000;
	
	ofPoint vel;
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1);
	for (int j=0; j<fh-2; j++ ){
		for (int i=0; i<fw-2; i++ ){
			_fluidSolver->getInfoAtCell(i+1, j+1, &vel, NULL);
			float d2 = vel.x * vel.x + vel.y * vel.y;
			if(d2 > maxVel * maxVel) {
				float mult = maxVel * maxVel/ d2;
				//				float mult = (d2 - maxVel * maxVel) / d2;
				vel.x *= mult;
				vel.y *= mult;
			}
			vel *= velMult;
			
			if(d2 < minVelThreshold || d2 > maxVelThreshold)
				continue;
#ifndef TARGET_OPENGLES
			float oldLineWidth = 0;
			glGetFloatv(GL_LINE_WIDTH, &oldLineWidth);

			glLineWidth(5.);

			glBegin(GL_LINES);
			
			vx = floor(i * vw / fw);
			vy = floor(j * vh / fh);

			glColor3f(mask_pix[(int)(3*(vy*vw + vx)+0)] / 345.,
					  mask_pix[(int)(3*(vy*vw + vx)+1)] / 345.,
					  mask_pix[(int)(3*(vy*vw + vx)+2)] / 345.);
			glVertex2f(i, j);
			
			glColor3f(mask_pix[(int)(3*(vy*vw + vx)+0)] / 255.,
					  mask_pix[(int)(3*(vy*vw + vx)+1)] / 255.,
					  mask_pix[(int)(3*(vy*vw + vx)+2)] / 255.);
			glVertex2f(i + vel.x, j + vel.y);
			
			glEnd();
			
			glLineWidth(oldLineWidth);
#endif
		}
	}
	glPopMatrix();
	
}

