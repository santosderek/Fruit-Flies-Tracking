#include "Vile.h"



Vile::Vile()
	: swarm()
{
	 

}


Vile::~Vile()
{
}

void Vile::analyze(Fly passedFly)
{
	
	swarm.interpretFly(passedFly);
	
	
}

void Vile::setBounds(float passedLeft, float passedRight)
{
	leftLimit = passedLeft;
	rightLimit = passedRight;
}

bool Vile::withinBounds(float passedPos)
{
	//std::cout << "Left Limit: " << leftLimit << ", passed: " << passedPos << ", rightLimit: " << rightLimit << std::endl;
	if (leftLimit <= passedPos && passedPos < rightLimit)
	{
		return true;
	}

	return false; 
	
}

void Vile::checkActive()
{
	swarm.CheckActive();
}

std::vector<Fly> Vile::activeFlies()
{
	return swarm.getTotalActiveFlies();
}

int Vile::getTotalActiveCount()
{
	return swarm.getTotalActiveCount();
}

int Vile::getTotalFlyCount()
{
	return swarm.size();
}

