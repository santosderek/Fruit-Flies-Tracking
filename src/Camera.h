#pragma once


#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string.h>

#include "Swarm.h"
#include "Vile.h"
enum CameraMode {
	NORMAL,
	HSV,
	THRESHOLD,
	CONTOURS, 
	GRAYSCALE
};

class Camera
{
private: 

	void restoreToDefault();
	void getFrame();

	int currentCameraNumber;
	int minThresh;
	int maxThresh;
	int minRadius; 
	int maxRadius; 
	int minDistance;
	int maxFlies; 

	Vile activeViles[4]; 

	CameraMode currentMode; 

	cv::VideoCapture captureDevice;

	cv::Mat normalFrame;
	cv::Mat contourFrame;
	cv::Mat grayscaleFrame();
	cv::Mat hsvFrame();
	cv::Mat thresholdFrame();

public:
	Camera();
	Camera(int);
	~Camera();

	std::string giveInformation();

	void setMode(CameraMode);
	void processContours();
	void openCamera(int); 

	cv::Mat frame(); 
};






