/*
Author: Derek Santos


Explanation:
Detect flies and track. Explanation still in the works.

*/

#include "Fly.h"

using namespace std::chrono;

//Fly Functions

Fly::Fly(std::vector<cv::Point>passedContour, cv::Point2f passedCenter, float passedRadius)
{
	contour = passedContour;
	center = passedCenter;
	radius = passedRadius;
	isAlive = true;

	timeCreated = std::chrono::high_resolution_clock::now();
	lastMoved = std::chrono::high_resolution_clock::now();

}

Fly::~Fly()
{

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

