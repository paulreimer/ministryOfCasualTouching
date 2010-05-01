#pragma once

#include "ofMain.h"
#include "settings.h"

#ifdef USE_PHYSICS
#include "PhysicsSystem.h"
#endif

#ifdef USE_OSC
#include "ofxOsc.h"
#endif

#ifdef USE_VIDEO
#include "VideoSystem.h"
#endif

#ifdef USE_WEB_SOCKETS
#include "UIWebSocketHandler.h"
#include "WebSocketServer.h"
#endif

#include "settings.h"

class testApp : public ofSimpleApp
{
public:
#ifdef USE_WEB_SOCKETS
	WebSocketServer<UIWebSocketHandler> gui;
#endif

#ifdef USE_PHYSICS 
	PhysicsSystem physicsSystem;
#endif

#ifdef USE_VIDEO
	VideoSystem videoSystem;
#endif
	
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);

	void windowResized(int w, int h);

#ifdef USE_OSC
	ofxOscReceiver	osc_in;
	ofxOscSender	osc_out;
#endif
	ofPoint pos;
	
	int pmouseX, pmouseY;

	// cache these for slightly better performance
	struct WindowSettings
	{
		int		width;
		int		height;
		float	invWidth;
		float	invHeight;
		float	aspectRatio;
		float	aspectRatio2;
	} window;
};
