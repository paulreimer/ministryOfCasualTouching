#include "FluidDrawer.h"
#include "settings.h"


//--------------------------------------------------------------
FluidDrawer::FluidDrawer()
{
	mask = NULL;
	maxVelThreshold = FLT_MAX;
	minVelThreshold = 0.;
}

//--------------------------------------------------------------
void
FluidDrawer::draw(float x, float y, float renderWidth, float renderHeight)
{
	switch(drawMode)
	{
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
			drawVectors(x, y, renderWidth, renderHeight);
			break;

		case FLUID_DRAW_VECTORS_MASKED:
			drawVectorsMasked(x, y, renderWidth, renderHeight);
			break;
			
		case FLUID_DRAW_TEXTURE:
			drawTexture(x, y, renderWidth, renderHeight);
			break;
	}
}

ofxMSAFluidSolver* FluidDrawer::setup(int NX, int NY)
{
	initGL(NX, NY);
	return ofxMSAFluidDrawer::setup(NX, NY);
}

ofxMSAFluidSolver* FluidDrawer::setup(ofxMSAFluidSolver* f)
{
	initGL(f->getWidth() - 2, f->getHeight() - 2);
	return ofxMSAFluidDrawer::setup(f);
}

void FluidDrawer::initGL(int NX, int NY)
{
	if (mask==NULL)
		return;

	int cnt = (NX-1)*(NY-1);
	indices.resize(4*cnt);

	int n = NX*NY;
	verts.resize(3*n);
	tex_coords.resize(2*n);
	normals.resize(3*n);

#define IDX(i,j) (i*NX+j)
	for (int i=0; i<NY; i++)
		for (int j=0; j<NX; j++)
		{
			ofPoint p = mask->getTextureReference().getCoordFromPercent(j/(float)NX, i/(float)NY);
			tex_coords[2*IDX(i,j)+0] = p.x;
			tex_coords[2*IDX(i,j)+1] = p.y;
		}

	for (int i=0; i<(NY-1); i++)
		for (int j=0; j<(NX-1); j++)
		{
			indices[4*IDX(i,j)+0] = IDX(i,j);
			indices[4*IDX(i,j)+1] = IDX(i,j) + 1;
			indices[4*IDX(i,j)+2] = IDX(i,j) + NX + 1;
			indices[4*IDX(i,j)+3] = IDX(i,j) + NX;
		}
#undef IDX
}

void FluidDrawer::drawVectorsMasked(float x, float y, float renderWidth, float renderHeight)
{
	if (mask==NULL)
		return;

	unsigned char* mask_pix;
	
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();
	
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

#define IDX(vy,vw) (vy*vw + vx)
			glColor3f(mask_pix[(int)(3*IDX(vy,vw)+0)] / (1.5 * 255.),
					  mask_pix[(int)(3*IDX(vy,vw)+1)] / (1.5 * 255.),
					  mask_pix[(int)(3*IDX(vy,vw)+2)] / (1.5 * 255.));
			glVertex2f(i, j);
			
			glColor3f(mask_pix[(int)(3*IDX(vy,vw)+0)] / 255.,
					  mask_pix[(int)(3*IDX(vy,vw)+1)] / 255.,
					  mask_pix[(int)(3*IDX(vy,vw)+2)] / 255.);
			glVertex2f(i + vel.x, j + vel.y);
			
			glEnd();
			
			glLineWidth(oldLineWidth);
#undef IDX
#endif
		}
	}
	glPopMatrix();	
}

void FluidDrawer::drawTexture(float x, float y, float renderWidth, float renderHeight)
{
	if (mask==NULL)
		return;

	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();
	
	unsigned char* mask_pix = mask->getPixels();
	float vx,vy,vw=VIDEO_WIDTH,vh=VIDEO_HEIGHT;
	
	float velMult = 50000;
	float maxVel = 5./20000;
	ofPoint vel;
	
	for (int j=0; j<fh-2; j++ )
	{
		for (int i=0; i<fw-2; i++ )
		{
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
			
			ofPoint vertex(i + vel.x,
						   j + vel.y,
						   vel.x*vel.x + vel.y+vel.y);
#define IDX(x,y) (y*(fw-2) + x)
			verts[3*IDX(i,j)+0]		= vertex.x;
			verts[3*IDX(i,j)+1]		= vertex.y;
			verts[3*IDX(i,j)+2]		= vertex.z;

			normals[3*IDX(i,j)+0]	= vertex.x;
			normals[3*IDX(i,j)+1]	= vertex.y;
			normals[3*IDX(i,j)+2]	= vertex.z;
#undef IDX
		}
	}

	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(renderWidth/(fw-2), renderHeight/(fh-2), 1.0);

	ofTexture& tex = mask->getTextureReference();
	tex.loadData(mask->getPixels(), mask->width, mask->height, GL_RGB);
	tex.bind();
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		{
			glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords[0]);
			glNormalPointer(GL_FLOAT, 0, &normals[0]);
			glVertexPointer(3, GL_FLOAT, 0, &verts[0]);
			glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, &indices[0]);
		}
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	tex.unbind();

	glPopMatrix();
}
