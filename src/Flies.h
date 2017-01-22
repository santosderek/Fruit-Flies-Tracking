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
	cv::Point2f center; // The center of the detected fly
	

public:
	Fly(std::vector<cv::Point>, cv::Point2f); 

	void setContour(std::vector<cv::Point>); 
	void setCenter(cv::Point2f);
	double distanceApart(Fly);

	std::vector <cv::Point> getContour();
	cv::Point2f getCenter();

};

// Class for all the flies
class Swarm
{
private: 
	std::vector < Fly > flies; 

public:
	Swarm();

	void addFly(Fly);
	void replaceFly(int, Fly);
	int nearestFly(Fly); // Returns the position of the nearest fly to the passed in Fly
	
	// Get Distance is overloaded for 2 flies, or the position of the fly in the vector with a new fly
	double getDistance(Fly, Fly);
	double getDistance(int, Fly);

	int size();
};

#endif
