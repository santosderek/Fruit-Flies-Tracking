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

	int currentCameraNumber;
	double minThresh;
	double maxThresh;
	double minRadius;
	double maxRadius;
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
	void getFrame();

	Camera();
	Camera(int);
	~Camera();

	std::string giveInformation();

	void setMode(CameraMode);
	void processContours();
	void openCamera(int); 

	void minUp()
	{
		minThresh += 0.5; 
		std::cout << "Minimum Threshold: " << minThresh << std::endl;
	}

	void minDown()
	{
		minThresh -= 0.5; 
		std::cout << "Minimum Threshold: " << minThresh << std::endl;

	}

	void maxUp()
	{
		maxThresh += 0.5;
		std::cout << "Maximum Threshold: " << maxThresh << std::endl;

	}

	void maxDown()
	{
		maxThresh -= 0.5;
		std::cout << "Maximum Threshold: " << maxThresh << std::endl;


	}

	cv::Mat frame(); 
};






