#include "Swarm.h"


//Swarm Functions

Swarm::Swarm()
{
	minLifeInSeconds = 10;

	flies.clear();

	maxFlies = 12;

	// 60000 pixels will be the default
	//minDistance = 6000;
	minDistance = 6000;

}

void Swarm::addFly(Fly passedFly)
{
	passedFly.positionInSwarm = flies.size();
	flies.push_back(passedFly);
	
}

int Swarm::size()
{
	return flies.size();
}

int Swarm::nearestFly(Fly passedFly)
{
	// Default was 70
	float closestDistance = minDistance; // Starts at a rediculus number
	int positionOfClosestFly = 0;
	int currentFly = 0;

	for (Fly eachFly : flies)
	{
		int distance = eachFly.distanceApart(passedFly); 

		if (distance < closestDistance  && distance < minDistance)
		{
			positionOfClosestFly = currentFly;
			closestDistance = eachFly.distanceApart(passedFly);
		}
		currentFly++;
	}

	return positionOfClosestFly;
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

bool Swarm::getState(int passedPos)
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

Fly Swarm::getFly(int passedPosition)
{
	return flies[passedPosition];
}

int Swarm::getTotalActiveCount()
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

std::vector<Fly> Swarm::getTotalActiveFlies()
{
	std::vector<Fly> tempList; 

	int count = 0;
	for (Fly currentFly : flies)
	{
		if (currentFly.getState())
		{
			tempList.push_back(currentFly);
		}
	}

	return tempList;
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



void Swarm::interpretFly(Fly passedFly)
{
	int closestFlyPos = nearestFly(passedFly); 


	if (this->size() > 0 && getDistance(closestFlyPos, passedFly) <= minDistance)
	{

		// If the state of the selected fly is true, then proceed
		if (getState(closestFlyPos))
		{
			replaceFly(closestFlyPos, passedFly);
		}

	}
	else
	{

		if (getTotalActiveCount() < maxFlies)
		{
			addFly(passedFly);
		}
	}

}