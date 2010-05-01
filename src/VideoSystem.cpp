#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

VideoSystem::VideoSystem()
{
	printf("VideoSystem::VideoSystem()\n");
	settings.threshold		= 20,
	settings.dv_min_error	= 0,
	settings.dv_max_error	= 550,
	settings.dv_scale		= 1.0,
	settings.bLearnBg		= true,	
	settings.max_iter		= 20,
	settings.win_size		= 10,
	settings.eps			= 0.03;
	settings.max_features	= 50;
	settings.pyr_levels		= 5;
	settings.draw_flow		= true;
	settings.calc_flow		= true;
}

VideoSystem::~VideoSystem()
{
	printf("VideoSystem::~VideoSystem()\n");
	destroy();
}

VideoSystem& VideoSystem::setup()
{
#ifdef _USE_LIVE_VIDEO
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(VIDEO_SIZE);
#else
	vidPlayer.loadMovie("fingers.mov");
	vidPlayer.play();
#endif
	
    colorImg.allocate(VIDEO_SIZE);
	grayImg.allocate(VIDEO_SIZE);
	grayPrev.allocate(VIDEO_SIZE);
	grayBg.allocate(VIDEO_SIZE);
	grayDiff.allocate(VIDEO_SIZE);
	
	img_sz = cvGetSize( grayImg.getCvImage() );	
	pyr_sz = cvSize( grayImg.getCvImage()->width+8, grayPrev.getCvImage()->height/3 ); 
	
	myApp->physicsSystem.fluidDrawer.mask = &colorImg;
}

void VideoSystem::update()
{
	bool bNewFrame = false;
	
#ifdef _USE_LIVE_VIDEO
	vidGrabber.grabFrame();
	bNewFrame = vidGrabber.isFrameNew();
#else
	vidPlayer.idleMovie();
	bNewFrame = vidPlayer.isFrameNew();
#endif
	
	if (bNewFrame)
	{
#ifdef _USE_LIVE_VIDEO
		colorImg.setFromPixels(vidGrabber.getPixels(), VIDEO_SIZE);
#else
		colorImg.setFromPixels(vidPlayer.getPixels(), VIDEO_SIZE);
#endif
		colorImg.mirror( false, true );
        grayImg = colorImg;
		if (settings.bLearnBg == true)
		{
			grayBg = grayImg;
			settings.bLearnBg = false;
		}
		
		// take the abs value of the difference between background and incoming and then threshold:
//		grayDiff.absDiff(grayBg, grayImg);
//		grayDiff.threshold(settings.threshold);

		if (settings.calc_flow)
		{
			cv::FAST(grayImg.getCvImage(), cornersKeyPoints, settings.threshold, true);

			corners.resize(cornersKeyPoints.size());
			for (int i=0; i<cornersKeyPoints.size(); i++)
				corners[i] = cornersKeyPoints[i].pt;
			
			cv::calcOpticalFlowPyrLK(grayImg.getCvImage(),
									 grayPrev.getCvImage(),
									 corners,
									 cornersPrev,
									 features_found,
									 feature_errors,
									 cvSize(settings.win_size,settings.win_size),
									 settings.pyr_levels,
									 cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,
													  settings.max_iter,
													  settings.eps ),
									 0.5,
									 0//CV_LKFLOW_PYR_B_READY | CV_LKFLOW_INITIAL_GUESSES
									 );
			float x,y,  dx,dy;
			for(int i=0; i<corners.size(); i++)
			{
				if(features_found[i]
				   && feature_errors[i]>settings.dv_min_error
				   && feature_errors[i]<settings.dv_max_error)
				{
					x = corners[i].x/VIDEO_WIDTH;
					y = corners[i].y/VIDEO_HEIGHT;
					dx = x - cornersPrev[i].x/VIDEO_WIDTH;
					dy = y - cornersPrev[i].y/VIDEO_HEIGHT;
					myApp->physicsSystem.fluidSolver.addForceAtPos(x, y,
																   dx*settings.dv_scale,
																   dy*settings.dv_scale);
				}
			}
			
			copy(corners.begin(), corners.end(), cornersPrev.begin());
		}
		grayPrev = grayImg;
	}
}

void VideoSystem::draw()
{}

void VideoSystem::destroy()
	{
	printf("VideoSystem::destroy()\n");
}
