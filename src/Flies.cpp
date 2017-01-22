/*
Author: Derek Santos


Explanation:
Detect flies and track. Explanation still in the works.

*/

#include "Flies.h"

//Fly Functions

Fly::Fly(std::vector<cv::Point>passedContour)
{
	contour = passedContour;
}

void Fly::setContour(std::vector<cv::Point> passedContour)
{
	contour = passedContour; 
}




//Swarm Functions

Swarm::Swarm()
{
	
}

void Swarm::addFly(Fly passedFly)
{
	flies.push_back(passedFly);
}

int Swarm::size()
{
	return flies.size();
}