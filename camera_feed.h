/*
Author: Derek Santos

*/


#ifndef CAMERA_FEED_H
#define CAMERA_FEED_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vectors>

using namespace cv;


namespace camera
{

    class Feed
    {
    private:
        void _createWindow();
        void findContours();

        VideoCapture camera;

        /* List of frames collected */
        Mat normalFrame;
        //Mat hsvFrame;
        //Mat thresholdFrame;

		Mat getFrame();


        /* if true, then getFrame() returns that Feed
            else, it moves on to find the correct one */
        bool _normalFeed;
        bool _hsvFeed;
		bool _thresholdFeed;
		bool _grayFeed;

        std::string windowName;

    public:
        Feed(std::string, int);

		// TODO : setCameraNumber isn't implemented... Check What You Can do later
        void setCameraNumber(int);
        void switchCameraFeed(int keyPressed);
        void showFrame();




    };

}

#endif
