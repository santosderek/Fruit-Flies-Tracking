/*
Author: Derek Santos

*/

#include "camera_feed.h"
#include "errors.h"
#include <vector>
using namespace camera;

Feed::Feed(std::string winName, int cameraNum)
: camera(cameraNum)
{
    if (!camera.isOpened())
    {
        err::errorMesg("ERROR: Camera could not be opened...");
    }

    windowName = winName;

	//MINTHRESH = 0;

	//ABOVE WILL BE THE STARTING THRESH
	//BELLOW MINTHRESH WILL BE WHAT I THINK IS A GOOD DEFAULT 
	MINTHRESH = 135;

	//DONT NEED TO CHANGE
	MAXTHRESH = 255;

}

/* One window where you can switch feeds with number keys */
void Feed::_createWindow()
{
    cv::namedWindow(windowName, WINDOW_AUTOSIZE);
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
			_grayFeed = false;
			_contourFeed = false;

			break;

        case 110:
            //switch Camera to Normal
            _hsvFeed       = false;
            _normalFeed    = true;
            _thresholdFeed = false;
			_grayFeed = false;
			_contourFeed = false;

			break;

		case 116:
			//switch Camera to Threshold
			_hsvFeed = false;
			_normalFeed = false;
			_thresholdFeed = true;
			_grayFeed = false;
			_contourFeed = false;

			break;

		case 103:
			//switch Camera to Threshold
			_hsvFeed = false;
			_normalFeed = false;
			_thresholdFeed = false;
			_grayFeed = true;
			_contourFeed = false;

			break;

		case 99:
			//switch Camera to Threshold
			_hsvFeed = false;
			_normalFeed = false;
			_thresholdFeed = false;
			_grayFeed = true;
			_contourFeed = true;
			break;

		/* The cases bellow is for testing of the thresholds for contours*/
		case 120:
			MINTHRESH += 5;
			std::cout << "MIN-THRESH: " << MINTHRESH << std::endl;
			std::cout << "MAX-THRESH: " << MAXTHRESH << std::endl;
			std::cout << std::endl;
			break;

		case 122:
			MINTHRESH -= 5;
			std::cout << "MIN-THRESH: " << MINTHRESH << std::endl;
			std::cout << "MAX-THRESH: " << MAXTHRESH << std::endl;
			std::cout << std::endl;

			break;

		case 46:
			MAXTHRESH += 5;
			std::cout << "MIN-THRESH: " << MINTHRESH << std::endl;
			std::cout << "MAX-THRESH: " << MAXTHRESH << std::endl;
			std::cout << std::endl;

			break;

		case 44:
			MAXTHRESH -= 5;
			std::cout << "MIN-THRESH: " << MINTHRESH << std::endl;
			std::cout << "MAX-THRESH: " << MAXTHRESH << std::endl;
			std::cout << std::endl;

			break;

    }
}

Mat Feed::getFrame()
{

	/* TODO: Put all of these if statments as their own functions at somepoint */


	// Camera will grab the picture as BGR
    camera >> normalFrame;


    // Returns Normal Frame if wanted
    if (_normalFeed)
    {
        return normalFrame;
    }

    // Returns Normal Frame if wanted
    if (_hsvFeed)
    {
        Mat hsvFrame;
		cvtColor(normalFrame, hsvFrame, CV_BGR2HSV);

        return hsvFrame;
    }

    // Returns Normal Frame if wanted
    if (_thresholdFeed)
    {
		Mat thresh;
		Mat changedIMG;

		cv::cvtColor(normalFrame, changedIMG, CV_BGR2GRAY);

		threshold(changedIMG, thresh, MINTHRESH, MAXTHRESH, THRESH_BINARY_INV);

		return thresh;

    }

	if (_contourFeed)
	{

		/* THE PERFECT EXPLANATION OF THRESHOLDS */
		/* http://docs.opencv.org/2.4/doc/tutorials/imgproc/threshold/threshold.html */
		Mat thresh;
		Mat changedIMG;

		cv::cvtColor(normalFrame, changedIMG, CV_BGR2GRAY);

		
		//TODO: UNCOMMENT THIS FOR TESTING THRESHOLD
		//threshold(changedIMG, thresh, MINTHRESH, MAXTHRESH, THRESH_BINARY_INV);
		threshold(changedIMG, thresh, MINTHRESH, MAXTHRESH, THRESH_BINARY_INV);


		std::vector< std::vector<Point> > contours;
		std::vector<Vec4i> hierarchy;

		cv::findContours(thresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		// Making a new Frame called Drawing made out of the size of the threshold image
		Mat drawing = Mat::zeros(thresh.size(), CV_8UC3);

		//for every point in the contour of the vector, draw a red dot
		for (int count = 0; count < contours.size(); ++count)
		{
			drawContours(drawing, contours, count, Scalar(255, 0, 0), 2, 8, hierarchy, 0, Point());

		}


		// New Section of the program (experimental)
		// Trying to differentiate each set of contours

		// Probably set a radius of the maximum a detected fly can be

		//I need to comment what everything does.
		// Add Some track bars

		std::vector< std::vector<Point> > contours_poly(contours.size());
		std::vector<Rect>    boundRect(contours.size());
		std::vector<Point2f> center(contours.size());
		std::vector<float>   radius(contours.size());

		for (int currentCont = 0; currentCont < contours.size(); currentCont++)
		{
			approxPolyDP(Mat(contours[currentCont]), contours_poly[currentCont], 3, true);
			minEnclosingCircle((Mat)contours_poly[currentCont], center[currentCont], radius[currentCont]);
		}


		// Draw polygonal contour + bonding rects + circles
		Scalar color = Scalar(0, 255, 0);

		for (int currentCont = 0; currentCont < contours.size(); currentCont++)
		{
			circle(drawing, center[currentCont], (int)radius[currentCont], color, 2, 8, 0);
		}





		// DO THIS: http://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/bounding_rects_circles/bounding_rects_circles.html#bounding-rects-circles










		return drawing;

	}

	// Returns Normal Frame if wanted
	if (_grayFeed)
	{
		Mat grayIMG;
		cv::cvtColor(normalFrame, grayIMG, CV_BGR2GRAY);

		return grayIMG;
	}

    return normalFrame;
}

void Feed::showFrame()
{

    imshow(this->windowName, this->getFrame());

}

void Feed::setCameraNumber(int number)
{
	try
	{
		camera.open(number);
	}
	catch (Exception &e)
	{
		std::cout << e.what() << std::endl;
		std::cout << std::endl << "Please Select A Number Again" << std::endl;
	}
}

void Feed::findContours()
{
    // A vector of countours from our image feed
    std::vector <std::vector<Point> > countour;

    // A vector giving more information of the contours in our image feed
    std::vector <Vec4i> hierarchy;

    // A search of the input image for the all contours found
    Mat grayIMG;
    cv::cvtColor (normalFrame, grayIMG, CV_BGR2GRAY);
    //cv::findContours(grayIMG, );
}
