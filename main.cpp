/*
	Author: Derek Santos

	Example of Motion Tracking

	Explanation:
		Draw dots on a notecard and the project will track it for you.

*/

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string.h>
#include <iostream>
#include "camera_feed.h"

using namespace cv;
using namespace std;

/* Camera '0' is the default   camera */
/* Camera '1' is the secondary camera */

/* Maximum Tracking Objects */
const int MAX_FLIES = 12;






int main()
{

	/* I believe we are tricking setMouseCallback into thinking that we are */
	/* ...sending a Mouse object, where rectangleMovement is                */
	//cv::setMouseCallback(videoFeedWindow, rectangleMovement, &normalFrame);

	camera::Feed camera_one("Camera One", 0);

	int key = 0;

	while (true)
	{
		try
		{

			/* Convert normal frames, into the HSV Frame variable */
			//cvtColor(normalFrame, hsvFrame, COLOR_BGR2HSV);
			//cvtColor(tFrame, thsv, COLOR_BGR2HSV);
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
				//std::cout << key << std::endl;
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

	destroyAllWindows();
	return 0;
}
