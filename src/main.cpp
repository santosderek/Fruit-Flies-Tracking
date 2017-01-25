/*
	Author: Derek Santos


	Explanation:
		Detect flies and track. Explanation still in the works. 

*/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string.h>
#include <iostream>
#include "camera_feed.h"
#include "Flies.h"

using namespace cv;
using namespace std;

/* Camera '0' is the default   camera */
/* Camera '1' is the secondary camera */
/* Camera 'x' is etc...*/

/* Maximum Tracking Objects */
const int MAX_FLIES = 12;

int main()
{


	camera::Feed camera_one("Camera One", 2);
	Swarm swarm;

	int key = 0;
	while (true)
	{
		try
		{

			

			camera_one.showFrame();

			/* waitKey(30) will display a frame for 30ms then be automatically closed. */
			/* (If you put it in a loop to read videos, it will display the video frame-by-frame) */
			/* 27 is the esc key */
			key = waitKey(30);
			if (key == 27)
			{
				break;
			}
			else
			{
				//Commented Line Bellow used to check key numbers

				/*
				if (key != 255 && key != -1)
				{
					std::cout << key << std::endl;

				}
				*/
				camera_one.evaluateContours(swarm);
				camera_one.switchCameraFeed(key);
			}
		}
		/* If an error occurs, I will catch it and the program will safely exit */
		catch (Exception &e)
		{
			std::cout << e.what() << std::endl;
			break;
		}
	}

	/* This destroyAllWindows is ALWAYS needed here for when the program
	closes. destroyAllWindows neatly cleans all background elements without
	causing an error / leak. */
	destroyAllWindows();
	return 0;
}
