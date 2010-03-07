#pragma once

#define	USE_OSC
#undef	USE_WEB_SOCKETS
#define	USE_PHYSICS
#define	USE_VIDEO
#undef	USE_GUI

#ifdef USE_OSC

#define OSC_IN_PORT			9000
#define OSC_OUT_PORT		9001
#define OSC_HOST			"localhost"

#endif

#ifdef USE_VIDEO

#define _USE_LIVE_VIDEO

#define VIDEO_WIDTH			160
#define VIDEO_HEIGHT		120
#define VIDEO_SIZE			VIDEO_WIDTH,VIDEO_HEIGHT

#endif
