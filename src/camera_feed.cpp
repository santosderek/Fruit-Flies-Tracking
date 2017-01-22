/*
	Author: Derek Santos

	Explanation:
		Detect flies and track. Explanation still in the works.
*/

#include "camera_feed.h"
#include "errors.h"
#include <vector>
#include <string>
#include "Flies.h"
using namespace camera;

Feed::Feed(std::string winName, int cameraNum)
: camera(cameraNum)
{
    if (!camera.isOpened())
    {
        err::errorMesg("ERROR: Camera could not be opened...");
    }

    windowName = winName;

    // Minimum value and Maximum value for the threshold frame.
	minThresh = 135;
	maxThresh = 255;

	/*Setting the Normal Feed as Default*/
    _normalFeed    = true;
    _hsvFeed       = false;
    _thresholdFeed = false;
    _grayFeed      = false;
    _contourFeed   = false;


	minRadius = 7.0;
	maxRadius = 70.0;

    cv::namedWindow(windowName, CV_WINDOW_NORMAL);

	resizeWindow(windowName, 800, 600);

	createTrackbar("Min Threshold", windowName, &minThresh, 255, changeMinThreshold);
	createTrackbar("Max Threshold", windowName, &maxThresh, 255, changeMaxThreshold);
	createTrackbar("Min Radius", windowName, &minRadius, 255, changeMaxThreshold);
	createTrackbar("Max Radius", windowName, &maxRadius, 255, changeMaxThreshold);

	
}

void Feed::changeMinThreshold(int passedValue, void*)
{
	//Function that gets excecuted when using MinThreshold trackbar
	//Feed::MINTHRESH = passedValue;
}

void Feed::changeMaxThreshold(int passedValue, void*)
{
	//Function that gets excecuted when using MaxThreshold trackbar
	//Feed::MAXTHRESH = passedValue;
}

void Feed::changeMinRadius(int passedValue, void*)
{
	//Function that gets excecuted when using MinThreshold trackbar
	//Feed::MINTHRESH = passedValue;
}

void Feed::changeMaxRadius(int passedValue, void*)
{
	//Function that gets excecuted when using MaxThreshold trackbar
	//Feed::MAXTHRESH = passedValue;
}

void Feed::switchCameraFeed(int keyPressed)
{
    switch (keyPressed)
    {
        /* if true, then getFrame() returns that Feed
            else, it moves on to find the correct one */

		/*
			TODO:
				Later make the variables bellow default false in the constructor,
				and then make a pointer called bool* active to be the current (pointed) key
				true; bool active = _hsvFeed; active* = false; active = _normalfeed;

				Something like that...
		*/
        case 104:
            //switch Camera to HSV
            _hsvFeed       = true;
            _normalFeed    = false;
            _thresholdFeed = false;
            _grayFeed      = false;
            _contourFeed   = false;
			break;

        case 110:
            //switch Camera to Normal
            _hsvFeed       = false;
            _normalFeed    = true;
            _thresholdFeed = false;
			_grayFeed      = false;
            _contourFeed   = false;
			break;

		case 116:
			//switch Camera to Threshold
			_hsvFeed       = false;
			_normalFeed    = false;
			_thresholdFeed = true;
			_grayFeed      = false;
			_contourFeed   = false;
			break;

		case 103:
			//switch Camera to Threshold
			_hsvFeed       = false;
			_normalFeed    = false;
			_thresholdFeed = false;
			_grayFeed      = true;
			_contourFeed   = false;
			break;

		case 99:
			//switch Camera to Threshold
			_hsvFeed       = false;
			_normalFeed    = false;
			_thresholdFeed = false;
			_grayFeed      = false;
			_contourFeed   = true;
			break;

		/* The cases bellow is for testing of the thresholds for contours*/
		case 120:
			minThresh += 5;
			std::cout << "MIN-THRESH: " << minThresh << std::endl;
			std::cout << "MAX-THRESH: " << maxThresh << std::endl;
			std::cout << std::endl;
			break;

		case 122:
			minThresh -= 5;
			std::cout << "MIN-THRESH: " << minThresh << std::endl;
			std::cout << "MAX-THRESH: " << maxThresh << std::endl;
			std::cout << std::endl;
			break;

		case 46:
			maxThresh += 5;
			std::cout << "MIN-THRESH: " << minThresh << std::endl;
			std::cout << "MAX-THRESH: " << maxThresh << std::endl;
			std::cout << std::endl;
			break;

		case 44:
			maxThresh -= 5;
			std::cout << "MIN-THRESH: " << minThresh << std::endl;
			std::cout << "MAX-THRESH: " << maxThresh << std::endl;
			std::cout << std::endl;
			break;

    }
}

Mat Feed::getHSVFrame(Mat &currentFrame)
{
    Mat hsvFrame;
    cvtColor(currentFrame, hsvFrame, CV_BGR2HSV);

    return hsvFrame;
}

Mat Feed::getThresholdFrame(Mat &currentFrame)
{
    Mat thresh;

    /*
    Allows MINTHRESH, MAXTHRESH threshold values to be changed by keybinding.
    THRESH_BINARY_INV focuses on the darker color than light.
        -> Does this by focusing on the minimum value than the Maximum.
    */

	// MinThresh will break program under these conditions
	if (minThresh < 5 )
	{
		minThresh = 5; 
	}

	else if (minThresh > 255)
	{
		minThresh = 5;
	}

	// MaxThresh will break program under these conditions
	if (maxThresh < 5)
	{
		maxThresh = 5;
	}

	else if (maxThresh > 255)
	{
		maxThresh = 255;
	}

    threshold(getGrayFrame(currentFrame), thresh, minThresh, maxThresh, THRESH_BINARY_INV);

    return thresh;
}

Mat Feed::getGrayFrame(Mat &currentFrame)
{
    Mat changedIMG;

    cv::cvtColor(currentFrame, changedIMG, CV_BGR2GRAY);

    return changedIMG;
}

Mat Feed::getContourFrame(Mat &currentFrame)
{
    return contourFrame;
}

void Feed::evaluateContours(Swarm& swarm)
{

	Mat thresh;

	thresh = getThresholdFrame(normalFrame);

	/*
	Creates a vector of another vector of points.
	The higher vector called Contours consits of a collection of individual
	contours.
	*/
	std::vector< std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;


	cv::findContours(thresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Making a new Frame called contourFrame made out of the size of the threshold image
	contourFrame = Mat::zeros(thresh.size(), CV_8UC3);

	//for every point in the contour of the vector, draw a red dot
	for (int count = 0; count < contours.size(); ++count)
	{
		drawContours(contourFrame, contours, count, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point());

	}

	// TODO: http://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/bounding_rects_circles/bounding_rects_circles.html#bounding-rects-circles

	// New Section of the program (experimental)
	// Trying to differentiate each set of contours



	std::vector< std::vector<Point> > tracked_contours;
	std::vector< std::vector<Point> > contours_poly(contours.size());
	std::vector<Point2f> poly_contour_center(contours.size());
	std::vector<float>   poly_contour_radius(contours.size()); // could possibily use this later in the program to set a radius limit to not track bigger objects


															   // Obtaining information for the currently collected contours such as, minimum values for a circle
	for (int currentCount = 0; currentCount < contours.size(); currentCount++)
	{
		approxPolyDP(Mat(contours[currentCount]), contours_poly[currentCount], 3, true);
		minEnclosingCircle((Mat)contours_poly[currentCount], poly_contour_center[currentCount], poly_contour_radius[currentCount]);
	}




	/*



	TODO:

	Goal for bellow.
	1. Put every contour in its own "Fly" class - done - 
	2. Put every "Fly" in its own "TotalFlys" class - done - 
	3. Within the "TotalFlys" class make it so that it does all the calculations for us - kinda but still doing - 
	a. Total number of flys - done - 
	b. give all the radius(s) for all the flys - done - 
	c. give information about that fly:
	Time
	Lifespan of said fly
	etc.
	d. velocity, etc.
	4. create sliders for the min and max radius of a fly
	5. Within the "TotalFly" class make it so that it knows when the fly is immobile




	*/


	/* Fun Fact: Apparently initiallizing in a loop is actually fine. I thought it wasn't 
	http://stackoverflow.com/questions/7959573/declaring-variables-inside-loops-good-practice-or-bad-practice-2-parter

	... Compilers are smart ...
	*/

	for (int currentCount = 0; currentCount < contours.size(); currentCount++)
	{
		double minimumDistance = 1000.0;
		int closestFlyPos = 0;
		// TODO: Make min and max radius variables to later manipulate with sliders
		if (poly_contour_radius[currentCount] > minRadius && poly_contour_radius[currentCount] < maxRadius)
		{
			Fly tempFly(contours[currentCount], poly_contour_center[currentCount], poly_contour_radius[currentCount]);

			closestFlyPos = swarm.nearestFly(tempFly);
			if ( swarm.size() > 0 && swarm.getDistance(closestFlyPos, tempFly) < minimumDistance)
			{
				


				/*Now it can track relatively well. I just need to tell it to compare to its last known 
				possition (to the closest dot) for when it has been lost and picked back up. (probably add 
				a timer to the last know position as well to time out if it hasnt been found in that region. 
				*/


				//putText(Frame, Text_to_Put (Fly's Position in Swarm), At center of the fly's circle, Font, Scale, Color, thickness, Linetype, Botomleft_is_orgin
				putText(contourFrame, std::to_string(closestFlyPos), poly_contour_center[currentCount], FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0), 1, 8, false);

				swarm.replaceFly(closestFlyPos, tempFly);
			}
			else
			{
				//putText(Frame, Text_to_Put (last Fly #), At center of the fly's circle, Font, Scale, Color, thickness, Linetype, Botomleft_is_orgin
				putText(contourFrame, std::to_string(swarm.size()), poly_contour_center[currentCount], FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0), 1, 8, false);
				swarm.addFly(tempFly);
			}
			circle(contourFrame, poly_contour_center[currentCount], (int)poly_contour_radius[currentCount], Scalar(0, 255, 0), 2, 8, 0);
			

		}
	}
	

	std::cout << "# of SWARM: " << swarm.size() << std::endl;

}

Mat Feed::getFrame()
{

	/* Note to developer: Camera will grab the picture as BGR */
    camera >> normalFrame;

    // Returns Normal Frame if wanted
    if (_normalFeed)
    {
        return normalFrame;
    }

    // Returns Normal Frame if wanted
    else if (_hsvFeed)
    {
        return getHSVFrame(normalFrame);
    }

    // Returns Normal Frame if wanted
    else if (_thresholdFeed)
    {
        return getThresholdFrame(normalFrame);
    }

	else if (_contourFeed)
	{
        return getContourFrame(normalFrame);
    }

	// Returns Normal Frame if wanted
	else if (_grayFeed)
	{
		return getGrayFrame(normalFrame);
	}

    else
    {
        return normalFrame;
    }
}

void Feed::showFrame()
{
    imshow(this->windowName, this->getFrame());
}
