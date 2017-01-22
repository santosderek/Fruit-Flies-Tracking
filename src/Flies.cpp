/*
Author: Derek Santos


Explanation:
Detect flies and track. Explanation still in the works.

*/

#include "Flies.h"
#include <math.h>
//Fly Functions

Fly::Fly(std::vector<cv::Point>passedContour, cv::Point2f passedCenter)
{
	contour = passedContour;
	center  = passedCenter; 
}

void Fly::setContour(std::vector<cv::Point> passedContour)
{
	contour = passedContour; 
}

cv::Point2f Fly::getCenter()
{
	return center; 
}

std::vector <cv::Point> Fly::getContour()
{
	return contour;
}








// TODO: CHECK IF THIS FUNCTION IS RIGHT.... IN ANOTHER TEST PROGRAM
double Fly::distanceApart(Fly otherFly)
{
	cv::Point2f oFlyCenter = otherFly.getCenter();

	// THE DISTANCE FORMULA ... TEST IF ITS RIGHT
	return (pow(oFlyCenter.x - this->center.x, 2) + pow(oFlyCenter.y - this->center.y, 2));
}

void Fly::setCenter(cv::Point2f passedCenter)
{
	center = passedCenter;
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

int Swarm::nearestFly(Fly passedFly)
{
	double closestDistance = 1000000; // Starts at a rediculus number
	int positionOfFly = 0; 

	for (int currentFly = 0; currentFly < flies.size(); ++currentFly)
	{
		if (flies[currentFly].distanceApart(passedFly) < closestDistance)
		{
			positionOfFly = currentFly; 
			closestDistance = flies[currentFly].distanceApart(passedFly);
		}
	}

	return positionOfFly; 
}

void Swarm::replaceFly(int pos, Fly passedFly)
{
	flies[pos].setContour(passedFly.getContour());
	flies[pos].setCenter(passedFly.getCenter());
}

double Swarm::getDistance(Fly originalFly, Fly passedFly)
{
	return originalFly.distanceApart(passedFly);
}

double Swarm::getDistance(int pos, Fly passedFly)
{
	return flies[pos].distanceApart(passedFly);
}
