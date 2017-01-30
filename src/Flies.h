/*
Author: Derek Santos


Explanation:
Detect flies and track. Explanation still in the works.

*/

#ifndef FLIES_H
#define FLIES_H

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string> 
#include "errors.h"
#include <math.h>
#include <chrono>


// Class for one fly
class Fly
{
private: 

	std::vector <cv::Point> contour; // list of points that make 1 contour of the said fly
	cv::Point2f center; // The center of the detected fly
	float radius; // Radius of the circle of the contour
	
	bool isAlive;

	std::chrono::high_resolution_clock::time_point timeCreated;
	std::chrono::high_resolution_clock::time_point lastMoved;

public:
	Fly(std::vector<cv::Point>, cv::Point2f, float); 

	void setContour(std::vector<cv::Point>); 
	void setCenter(cv::Point2f);
	void setRadius(float);

	double distanceApart(Fly);

	std::vector <cv::Point> getContour();
	cv::Point2f getCenter();
	float getRadius();

	void changeState();
	bool getState();
	int getLifeSpan(); 

	void flyMoved();
	int timeSinceMoved();

};

// Class for all the flies
class Swarm
{
private: 
	std::vector < Fly > flies; 
	int minLifeInSeconds;



public:
	Swarm(std::string);

	

	void addFly(Fly);
	void replaceFly(int, Fly);
	void changeState(int);
	void CheckActive();
	
	// Get Distance is overloaded for 2 flies, or the position of the fly in the vector with a new fly
	double getDistance(Fly, Fly);
	double getDistance(int, Fly);

	bool checkState(int); 
	
	int nearestFly(Fly); // Returns the position of the nearest fly to the passed in Fly
	int getTotalActive();
	int timeSinceMoved(int);
	int getTime(int);
	int size();



};

#endif
