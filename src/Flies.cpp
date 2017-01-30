/*
Author: Derek Santos


Explanation:
Detect flies and track. Explanation still in the works.

*/

#include "Flies.h"



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

Swarm::Swarm(std::string windowName)
{
	minLifeInSeconds = 10;




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
	double closestDistance = 70; // Starts at a rediculus number
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

