#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include "Swarm.h"

class Vile
{
private: 
	float leftLimit; 
	float rightLimit; 

	Swarm swarm; 



public:
	Vile();
	~Vile();

	void analyze(Fly);
	void setBounds(float, float);
	
	bool withinBounds(float);

	void checkActive();

	std::vector<Fly> activeFlies();
	int activeFlyCount();
	int getTotalFlyCount();


};

