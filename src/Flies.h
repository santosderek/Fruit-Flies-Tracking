/*
Author: Derek Santos


Explanation:
Detect flies and track. Explanation still in the works.

*/

#ifndef FLIES_H
#define FLIES_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string> 

// Class for one fly
class Fly
{
private: 

	// Find its position


	std::vector <cv::Point> contour;
	

public:
	Fly(std::vector<cv::Point>); 

	void setContour(std::vector<cv::Point>); 
};

// Class for all the flies
class Swarm
{
private: 
	std::vector < Fly > flies; 

public:
	Swarm();

	void addFly(Fly);
	int size();
};

#endif
