#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "cv.hpp"
#include "cvaux.hpp"

#include "settings.h"

class VideoSystem : public ofBaseUpdates {
public:	
	VideoSystem();
	virtual ~VideoSystem();
	
	VideoSystem& setup();
	void update();
	void draw();
	
	void destroy();
	
#ifdef _USE_LIVE_VIDEO
	ofVideoGrabber 		vidGrabber;
#else
	ofVideoPlayer 		vidPlayer;
#endif
	
	ofxCvColorImage			colorImg;
	
	ofxCvGrayscaleImage 	grayImg;
	ofxCvGrayscaleImage 	grayPrev;
	ofxCvGrayscaleImage 	grayBg;
	ofxCvGrayscaleImage 	grayDiff;
	
	struct VideoSettings {
		int		threshold;
		int		dv_min_error;
		int		dv_max_error;
		float	dv_scale;
		bool	bLearnBg;
		int		max_iter;
		int		win_size;
		float	eps;
		int		max_features;
		int		pyr_levels;
		bool	draw_flow;
		bool	calc_flow;
	} settings;
	
protected:
	cv::Size				img_sz;
	cv::Size				pyr_sz;
	
	vector<cv::KeyPoint>	cornersKeyPoints;
	vector<cv::Point2f>		corners;
	vector<cv::Point2f>		cornersPrev;
	
	vector<uchar>			features_found;
	vector<float>			feature_errors;
};
