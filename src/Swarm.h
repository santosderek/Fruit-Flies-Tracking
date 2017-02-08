#pragma once

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

#include "Fly.h"

// Class for all the flies
class Swarm
{
private:
	std::vector < Fly > flies;
	int minLifeInSeconds;



public:
	Swarm();

	void addFly(Fly);
	void replaceFly(int, Fly);
	void changeState(int);
	void CheckActive();

	// Get Distance is overloaded for 2 flies, or the position of the fly in the vector with a new fly
	double getDistance(Fly, Fly);
	double getDistance(int, Fly);

	bool getState(int);

	int nearestFly(Fly); // Returns the position of the nearest fly to the passed in Fly
	int getTotalActiveCount();
	int timeSinceMoved(int);
	int getTime(int);
	int size();
	std::vector<Fly> getTotalActiveFlies();

	Fly Swarm::getFly(int);

};
