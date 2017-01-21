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

using namespace cv;


namespace camera
{

    class Feed
    {
    private:
        void _createWindow();

        VideoCapture camera;

        /* An always ready variable for the normal frame to be altered */
        Mat normalFrame;

        /* These are functions to deliever the desired altered frame */
        /* After some thought, I decided to add an argument to each of the
        bellow functions since it will give an extra layer of portability and
        reuse if ever needed in another section of the program for future use.
        (Instead of just calling 'normalFrame' directly.) */
		Mat getFrame();
        Mat getHSVFrame(Mat);
        Mat getThresholdFrame(Mat);
        Mat getContourFrame(Mat);
        Mat getGrayFrame(Mat);



        /* if true, then getFrame() returns that Feed
            else, it moves on to find the correct one */
        bool _normalFeed;
        bool _hsvFeed;
		bool _thresholdFeed;
		bool _grayFeed;
		bool _contourFeed;

        /* These are the minimum and maximum threshold values that can
        dinamically change */
		int MAXTHRESH;
		int MINTHRESH;

        /* Name of the window for the current class */
        std::string windowName;


    public:
        Feed(std::string, int);

        void switchCameraFeed(int keyPressed);
        void showFrame();




    };

}

#endif
