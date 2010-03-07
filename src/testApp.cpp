#include "testApp.h"

#pragma mark App callbacks

//--------------------------------------------------------------
void
testApp::setup()
{
	// initialize stuff according to current window size
	windowResized(ofGetWidth(), ofGetHeight());	
	
	ofBackground(0, 0, 0);
	ofSetBackgroundAuto(true);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

#ifdef USE_VIDEO
	videoSystem.setup();
#endif

#ifdef USE_PHYSICS
	physicsSystem.setup();
#endif

#ifdef USE_OSC
	osc_in.setup(OSC_IN_PORT);
	osc_out.setup(OSC_HOST, OSC_OUT_PORT);
#endif

#ifdef USE_GUI
	gui.setup();	
	//	gui.addPage("Fluid");

	gui.addToggle("Draw Fluid",
				  physicsSystem.settings.drawFluid);
	gui.addSlider("fs.viscocity",
				  physicsSystem.fluidSolver.viscocity, 0.0, 0.0002, 0.5);
	gui.addSlider("fs.colorDiffusion",
				  physicsSystem.fluidSolver.colorDiffusion, 0.0, 0.0003, 0.5);
	gui.addSlider("fs.fadeSpeed",
				  physicsSystem.fluidSolver.fadeSpeed, 0.0, 0.1, 0.5);
	gui.addSlider("fs.solverIterations",
				  physicsSystem.fluidSolver.solverIterations, 1, 20);
	gui.addSlider("fd.drawMode",
				  physicsSystem.fluidDrawer.drawMode, 0, FLUID_DRAW_MODE_COUNT-1);
	gui.addToggle("fs.doRGB",
				  physicsSystem.fluidSolver.doRGB);
	gui.addToggle("fs.doVorticityConfinement",
				  physicsSystem.fluidSolver.doVorticityConfinement);
	gui.addToggle("drawFluid",
				  physicsSystem.settings.drawFluid);
	gui.addToggle("drawParticles",
				  physicsSystem.settings.drawParticles); 
	gui.addToggle("renderUsingVA",
				  physicsSystem.settings.renderUsingVA); 

	gui.addPage("Camera");
	gui.addContent("Color Image",
				   videoSystem.colorImg, (float)-1);
	gui.addContent("Gray Diff",
				   videoSystem.grayDiff, (float)-1);
	
	gui.addSlider("Threshold",
				  videoSystem.settings.threshold, 0, 255);
	gui.addButton("Capture Bg",
				  videoSystem.settings.bLearnBg);
	
	gui.addPage("Optical Flow");
	gui.addToggle("Calc. Flow",
				  videoSystem.settings.calc_flow);
	gui.addToggle("Draw Flow",
				  videoSystem.settings.draw_flow);
	gui.addSlider("Max Features",
				  videoSystem.settings.max_features, 0, 500);
	gui.addSlider("Dist. Vec. Scale",
				  videoSystem.settings.dv_scale, 0.0, 10.0, 0.1);
	gui.addSlider("Dist. Vec. Threshold",
				  videoSystem.settings.dv_min_error, 0, 512);
	gui.addSlider("Dist. Vec. Threshold",
				  videoSystem.settings.dv_max_error, 512, 1024);	
	
	gui.addContent("Contours", videoSystem.contourFinder);
#endif

#ifdef USE_WEB_SOCKETS
	gui.setup();
#endif
}


//--------------------------------------------------------------
void
testApp::update()
{
#ifdef USE_VIDEO
	videoSystem.update();
#endif
	
#ifdef USE_PHYSICS
	physicsSystem.update();
#endif

#ifdef USE_OSC
	string mAddr;
	while(osc_in.hasWaitingMessages())
	{
		// get the next message
		ofxOscMessage m;
		osc_in.getNextMessage(&m);
		
		mAddr = m.getAddress();

		if		(mAddr == "/physics/settings/drawFluid")
			physicsSystem.settings.drawFluid = (m.getArgAsInt32(0) != 0);
		else if	(mAddr == "/physics/settings/drawParticles")
			physicsSystem.settings.drawParticles = (m.getArgAsInt32(0) != 0);
		else if	(mAddr == "/physics/settings/renderUsingVA")
			physicsSystem.settings.renderUsingVA = (m.getArgAsInt32(0) != 0);
		else if	(mAddr == "/physics/fluidDrawer/drawMode")
			physicsSystem.fluidDrawer.drawMode = m.getArgAsInt32(0);
		else if	(mAddr == "/physics/fluidDrawer/minVelThreshold")
			physicsSystem.fluidDrawer.minVelThreshold = m.getArgAsFloat(0);
		else if	(mAddr == "/physics/fluidDrawer/maxVelThreshold")
			physicsSystem.fluidDrawer.maxVelThreshold = m.getArgAsFloat(0);

		else if	(mAddr == "/physics/fluidSolver/viscosity")
			physicsSystem.fluidSolver.viscocity = m.getArgAsFloat(0);
		else if	(mAddr == "/physics/fluidSolver/colorDiffusion")
			physicsSystem.fluidSolver.colorDiffusion = m.getArgAsFloat(0);
		else if	(mAddr == "/physics/fluidSolver/fadeSpeed")
			physicsSystem.fluidSolver.fadeSpeed = m.getArgAsFloat(0);
		else if	(mAddr == "/physics/fluidSolver/solverIterations")
			physicsSystem.fluidSolver.solverIterations = m.getArgAsInt32(0);
		else if	(mAddr == "/physics/fluidSolver/doRGB")
			physicsSystem.fluidSolver.doRGB = (m.getArgAsInt32(0) != 0);
		else if	(mAddr == "/physics/fluidSolver/doVorticityConfinement")
			physicsSystem.fluidSolver.doVorticityConfinement = (m.getArgAsInt32(0) != 0);

		else if	(mAddr == "/video/settings/threshold")
			videoSystem.settings.threshold = m.getArgAsInt32(0);
		else if	(mAddr == "/video/settings/max_features")
			videoSystem.settings.max_features = m.getArgAsInt32(0);
		else if	(mAddr == "/video/settings/dv_scale")
			videoSystem.settings.dv_scale = m.getArgAsFloat(0);
		else if	(mAddr == "/video/settings/dv_min_error")
			videoSystem.settings.dv_min_error = m.getArgAsInt32(0);
		else if	(mAddr == "/video/settings/dv_max_error")
			videoSystem.settings.dv_max_error = m.getArgAsInt32(0);
		else if	(mAddr == "/video/settings/bLearnBg")
			videoSystem.settings.bLearnBg = (m.getArgAsInt32(0) != 0);
		else if	(mAddr == "/video/settings/calc_flow")
			videoSystem.settings.calc_flow = (m.getArgAsInt32(0) != 0);
		else if	(mAddr == "/video/settings/draw_flow")
			videoSystem.settings.draw_flow = (m.getArgAsInt32(0) != 0);
		
		else if	(mAddr == "/video/settings/sync")
		{
			ofxOscMessage m;
			m.setAddress( "/video/settings/state" );

			m.addIntArg(videoSystem.settings.threshold);
			m.addIntArg(videoSystem.settings.max_features);
			m.addFloatArg(videoSystem.settings.dv_scale);
			m.addIntArg(videoSystem.settings.dv_min_error);
			m.addIntArg(videoSystem.settings.dv_max_error);
			m.addIntArg(videoSystem.settings.bLearnBg? 1:0);
			m.addIntArg(videoSystem.settings.calc_flow? 1:0);
			m.addIntArg(videoSystem.settings.draw_flow? 1:0);

			osc_out.sendMessage(m);
		}
		else if	(mAddr == "/physics/settings/sync")
		{
			ofxOscMessage m;
			m.setAddress( "/physics/settings/state" );
			
			m.addIntArg(physicsSystem.settings.renderUsingVA? 1:0);
			m.addIntArg(physicsSystem.settings.drawParticles? 1:0);
			m.addIntArg(physicsSystem.settings.drawFluid? 1:0);
			m.addIntArg(physicsSystem.fluidDrawer.drawMode);
			m.addFloatArg(physicsSystem.fluidDrawer.minVelThreshold);
			m.addFloatArg(physicsSystem.fluidDrawer.maxVelThreshold);
			
			osc_out.sendMessage(m);
		}
		else if	(mAddr == "/physics/fluidSolver/sync")
		{
			ofxOscMessage m;
			m.setAddress( "/physics/fluidSolver/state" );
			
			m.addFloatArg(physicsSystem.fluidSolver.viscocity);
			m.addFloatArg(physicsSystem.fluidSolver.colorDiffusion);
			m.addFloatArg(physicsSystem.fluidSolver.fadeSpeed);
			m.addIntArg(physicsSystem.fluidSolver.solverIterations);
			m.addIntArg(physicsSystem.fluidSolver.doRGB? 1:0);
			m.addIntArg(physicsSystem.fluidSolver.doVorticityConfinement? 1:0);
			
			osc_out.sendMessage(m);
		}
	}
#endif

	// save old mouse position (openFrameworks doesn't do this automatically like processing does)
	pmouseX = mouseX;
	pmouseY = mouseY;
}

//--------------------------------------------------------------
void
testApp::draw()
{
#ifdef USE_PHYSICS
	physicsSystem.draw();
#endif

#ifdef USE_GUI
	gui.draw();
#endif

#ifdef USE_VIDEO
	videoSystem.draw();
#endif
}

void
testApp::windowResized(int w, int h)
{
	printf("TEST windowResized(%i, %i)\n", w, h);
	window.width		= w;
	window.height		= h;
	
	window.invWidth		= 1.0f/window.width;
	window.invHeight	= 1.0f/window.height;
	window.aspectRatio	= window.width * window.invHeight;
	window.aspectRatio2 = window.aspectRatio * window.aspectRatio;
}


#pragma mark Input callbacks

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
    switch(key) {
#ifdef USE_GUI
		case ' ':
			gui.toggleDraw();
			glClear(GL_COLOR_BUFFER_BIT);
			break;
#endif			
		case 'f':
			ofToggleFullscreen();
			break;
		case 's':
			static char fileNameStr[255];
			sprintf(fileNameStr, "output_%0.4i.png", ofGetFrameNum());
			static ofImage imgScreen;
			imgScreen.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			printf("Saving file: %s\n", fileNameStr);
			imgScreen.saveImage(fileNameStr);
			break;
		case '1':
			videoSystem.vidGrabber.videoSettings();
			break;
	}
}

//--------------------------------------------------------------
void
testApp::mouseMoved(int x, int y)
{
	float mouseNormX = x * window.invWidth;
    float mouseNormY = y * window.invHeight;
    float mouseVelX = (x - pmouseX) * window.invWidth;
    float mouseVelY = (y - pmouseY) * window.invHeight;

    physicsSystem.addToFluid(mouseNormX, mouseNormY, mouseVelX, mouseVelY, true);
}

//--------------------------------------------------------------
void
testApp::mouseDragged(int x, int y, int button)
{
	float mouseNormX = x * window.invWidth;
    float mouseNormY = y * window.invHeight;
    float mouseVelX = (x - pmouseX) * window.invWidth;
    float mouseVelY = (y - pmouseY) * window.invHeight;
	
	physicsSystem.addToFluid(mouseNormX, mouseNormY, mouseVelX, mouseVelY, false);
}
