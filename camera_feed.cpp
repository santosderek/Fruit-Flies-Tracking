/*
Author: Derek Santos

*/

#include "camera_feed.h"
#include "errors.h"

using namespace camera;

Feed::Feed(std::string winName, int cameraNum)
: camera(cameraNum)
{
    if (!camera.isOpened())
    {
        err::errorMesg("ERROR: Camera could not be opened...");

    }

    windowName = winName;
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
        case 104:
            //switch Camera to HSV
            _hsvFeed       = true;
            _normalFeed    = false;
            _thresholdFeed = false;
			_grayFeed = false;
			break;

        case 110:
            //switch Camera to Normal
            _hsvFeed       = false;
            _normalFeed    = true;
            _thresholdFeed = false;
			_grayFeed = false;
			break;

		case 116:
			//switch Camera to Threshold
			_hsvFeed = false;
			_normalFeed = false;
			_thresholdFeed = true;
			_grayFeed = false;
			break;

		case 103:
			//switch Camera to Threshold
			_hsvFeed = false;
			_normalFeed = false;
			_thresholdFeed = false;
			_grayFeed = true;
			break;
    }
}

Mat Feed::getFrame()
{
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
		/* Un-Comment If color is gray. */
		//cvtColor(normalFrame, hsvFrame, CV_GRAY2BGR);
        Mat hsvFrame;

		cvtColor(normalFrame, hsvFrame, CV_BGR2HSV);

        return hsvFrame;
    }

    // Returns Normal Frame if wanted
    if (_thresholdFeed)
    {
		//cvtColor(normalFrame, hsvFrame, COLOR_RGB2HSV);
		//cvtColor(normalFrame, hsvFrame, COLOR_BGR2HSV);
		//cvtColor(normalFrame, hsvFrame, COLOR_RGB2HSV);
		Mat thresh;
		Mat changedIMG;

		cv::cvtColor(normalFrame, changedIMG, CV_BGR2GRAY);

		/* Documentation : */
		/* http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold */
		/* I have just found out the 100, 255, THRESH_BINARY is perfect right now. */
		/* Though you have to have a black and white background and black dots scatter around */
		threshold(changedIMG, thresh, 100, 255, THRESH_BINARY);

		return changedIMG;

    }

	// Returns Normal Frame if wanted
	if (_grayFeed)
	{
		//cvtColor(normalFrame, hsvFrame, COLOR_RGB2HSV);
		//cvtColor(normalFrame, hsvFrame, COLOR_BGR2HSV);
		//cvtColor(normalFrame, hsvFrame, COLOR_RGB2HSV);
		Mat grayIMG;

		cv::cvtColor(normalFrame, grayIMG, CV_BGR2GRAY);
		//cv::cvtColor(grayIMG,     grayIMG, CV_GRAY2BGR);

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
    cv::findContours(grayIMG, );
}
