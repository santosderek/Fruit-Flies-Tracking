#pragma once


#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>

#include "Swarm.h"

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

	int currentCameraNumber;

	int minThresh;
	int maxThresh;

	int minRadius; 
	int maxRadius; 

	int minDistance;

	int maxFlies; 

	CameraMode currentMode; 

	cv::VideoCapture captureDevice;
	cv::Mat normalFrame;

	cv::Mat grayscaleFrame();
	cv::Mat hsvFrame();
	cv::Mat thresholdFrame();
	cv::Mat contourFrame();


	Swarm swarm;

public:
	Camera();
	Camera(int);
	~Camera();

	void openCamera(int); 
	void getFrame();
	void processContours();
	void setMode(CameraMode);

	cv::Mat frame(); 
};



