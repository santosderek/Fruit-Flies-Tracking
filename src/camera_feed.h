/*
	Author: Derek Santos


	Explanation:
		Detect flies and track. Explanation still in the works.

*/


#ifndef CAMERA_FEED_H
#define CAMERA_FEED_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>
#include "Flies.h"

using namespace cv;


namespace camera
{

    class Feed
    {
    private:

        VideoCapture camera;

        /* An always ready variable for the normal frame to be altered */
		Mat normalFrame;
		Mat contourFrame;

        /* These are functions to deliever the desired altered frame */
        /* After some thought, I decided to add an argument to each of the
        bellow functions since it will give an extra layer of portability and
        reuse if ever needed in another section of the program for future use.
        (Instead of just calling 'normalFrame' directly.) */
		Mat getFrame();
        Mat getHSVFrame(Mat&);
        Mat getThresholdFrame(Mat&);
        Mat getContourFrame(Mat&);
        Mat getGrayFrame(Mat&);




        /* if true, then getFrame() returns that Feed
            else, it moves on to find the correct one */
        bool _normalFeed;
        bool _hsvFeed;
		bool _thresholdFeed;
		bool _grayFeed;
		bool _contourFeed;

        /* Name of the window for the current class */
        std::string windowName;

		/* These are the minimum and maximum threshold values that can
		dinamically change */

		int maxThresh;
		int minThresh;

		/* These values need to be static. If not then createTrackbar will break. Will give more info later. */
		static void changeMinThreshold(int, void*);
		static void changeMaxThreshold(int, void*);

    public:
        Feed(std::string, int);

        void switchCameraFeed(int keyPressed);
        void showFrame();
		void evaluateContours(Swarm&); 

    };

}

#endif
