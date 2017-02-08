#pragma once

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string> 
#include <math.h>
#include <chrono>


// Class for one fly
class Fly
{
private:
	std::vector <cv::Point> contour; // list of points that make 1 contour of the said fly
	cv::Point2f center; // The center of the detected fly

	bool isAlive;

	float radius; // Radius of the circle of the contour

	std::chrono::high_resolution_clock::time_point timeCreated;
	std::chrono::high_resolution_clock::time_point lastMoved;

public:
	Fly(std::vector<cv::Point>, cv::Point2f, float);
	~Fly();

	int positionInSwarm;

	void setContour(std::vector<cv::Point>);
	void setCenter(cv::Point2f);
	void setRadius(float);
	void changeState();
	void flyMoved();

	double distanceApart(Fly);
	float getRadius();

	std::vector <cv::Point> getContour();
	cv::Point2f getCenter();

	bool getState();

	int getLifeSpan();
	int timeSinceMoved();

};

