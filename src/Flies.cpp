/*
Author: Derek Santos


Explanation:
Detect flies and track. Explanation still in the works.

*/

#include "Flies.h"
#include <math.h>

/* TODO: CHANGE THIS


BUT



FOR NOW


*/
#include <chrono>

/* We are going to use


...


http://en.cppreference.com/w/cpp/chrono/high_resolution_clock

from <chrono>


....


if it becomes a problem later then


use Boost's chrono module

http://stackoverflow.com/questions/3220477/how-to-use-clock-in-c

*/





/*

TODO: ANOTHER THING 


I think we should use 

bool state = true; 


to show if a fly is active or inactive


and if the fly is inactive 


then we don't track the fly anymore 


so when checking the distance from object to last known 

position, it will look for the closest tracked fly, and the closest 

active contour*/








#include <iostream>


using namespace std::chrono;








//Fly Functions

Fly::Fly(std::vector<cv::Point>passedContour, cv::Point2f passedCenter, float passedRadius)
{
	contour = passedContour;
	center  = passedCenter; 
	radius  = passedRadius; 
	isAlive   = true; 

	timeCreated = std::chrono::high_resolution_clock::now();
	lastMoved = std::chrono::high_resolution_clock::now();

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

float Fly::getRadius()
{
	return radius;
}

int Fly::getLifeSpan()
{
	std::chrono::high_resolution_clock::time_point timeNow = std::chrono::high_resolution_clock::now();
	
	duration<double> time_span = duration_cast<std::chrono::seconds>(timeNow - this->timeCreated);
	
	return time_span.count();

}

void Fly::changeState()
{
	if (isAlive)
	{
		isAlive = false; 
	}
}

bool Fly::getState()
{
	return isAlive; 
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

void Fly::setRadius(float passedRadius)
{
	radius = passedRadius;
}

void Fly::flyMoved()
{
	lastMoved = std::chrono::high_resolution_clock::now();
}

int Fly::timeSinceMoved()
{
	std::chrono::high_resolution_clock::time_point timeNow = std::chrono::high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(timeNow - lastMoved);
	return time_span.count();
}



//Swarm Functions

Swarm::Swarm()
{
	minLifeInSeconds = 5;
	maxFlies = 12; 
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
	flies[pos].setRadius(passedFly.getRadius());
	flies[pos].flyMoved();
}

double Swarm::getDistance(Fly originalFly, Fly passedFly)
{
	return originalFly.distanceApart(passedFly);
}

double Swarm::getDistance(int pos, Fly passedFly)
{
	return flies[pos].distanceApart(passedFly);
}

bool Swarm::checkState(int passedPos)
{
	return flies[passedPos].getState();
}

void Swarm::changeState(int passedPosition)
{
	flies[passedPosition].changeState(); 
}

int Swarm::getTime(int passedPosition)
{
	return flies[passedPosition].getLifeSpan();
}

int Swarm::getTotalActive()
{
	int count = 0;
	for (Fly& currentFly : flies)
	{
		if (currentFly.getState())
		{
			++count;
		}
	}

	return count;
}

int Swarm::timeSinceMoved(int passedPos)
{
	return flies[passedPos].timeSinceMoved();
}

void Swarm::CheckActive()
{

	for (Fly& currentFly : flies)
	{
		if (currentFly.timeSinceMoved() > 5)
		{
			currentFly.changeState();
		}
	}
}

int Swarm::getMaxFlies()
{
	return maxFlies;
}