#include "Camera.h"

// Probably Need To Put This Under A Namespace



using namespace cv;


Camera::Camera()
	: captureDevice(),
	swarm()
{
	restoreToDefault();
}

Camera::Camera(int cameraNumber)
	: captureDevice(cameraNumber)
{
	if (!captureDevice.isOpened())
	{
		exit(1);
	}

	restoreToDefault();

	getFrame();
}

//Since we have many constructors ... I made this void to make sure everything is initized.
// Yes I know that's what constructors are for...
void Camera::restoreToDefault()
{
	//Minimum and Maximum Threshold to filter different contours out
	minThresh = 46;
	maxThresh = 255;

	//Minimum and Maximum Radius' for the circle closing in.
	minRadius = 1;
	maxRadius = 90;

	// 60000 pixels will be the default
	minDistance = 60000;
	maxFlies = 12;
	
}

Camera::~Camera()
{
}


void Camera::getFrame()
{
	captureDevice >> normalFrame;
}

cv::Mat Camera::grayscaleFrame()
{
	cv::Mat grayscale; 
	cv::cvtColor(normalFrame, grayscale, cv::COLOR_BGR2GRAY);
	return grayscale; 
}

cv::Mat Camera::hsvFrame()
{
	// Frame
	cv::Mat tempFrame; 
	
	cv::cvtColor(normalFrame, tempFrame, cv::COLOR_BGR2HSV);

	return tempFrame;
}

cv::Mat Camera::thresholdFrame()
{
	// Frame
	cv::Mat thresh; 
	

	cv::threshold(grayscaleFrame(), thresh, minThresh, maxThresh, THRESH_BINARY_INV);

	
	return thresh;

}



void Camera::processContours()
{
	swarm.CheckActive();

	cv::Mat thresh;

	cv::threshold(grayscaleFrame(), thresh, minThresh, maxThresh, THRESH_BINARY_INV);

	contourFrame = Mat::zeros(thresh.size(), CV_8UC3);

	std::vector< std::vector<cv::Point> > contours;
	std::vector<Vec4i> hierarchy;

	cv::findContours(thresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	std::vector< std::vector<Point> > tracked_contours;
	std::vector< std::vector<Point> > contours_poly(contours.size());
	std::vector<Point2f> poly_contour_center(contours.size());
	std::vector<float>   poly_contour_radius(contours.size());

	// Obtaining information for the currently collected contours such as, minimum values for a circle
	for (int currentCount = 0; currentCount < contours.size(); currentCount++)
	{
		approxPolyDP(Mat(contours[currentCount]), contours_poly[currentCount], 3, true);
		minEnclosingCircle((Mat)contours_poly[currentCount], poly_contour_center[currentCount], poly_contour_radius[currentCount]);
	}

	for (int currentCount = 0; currentCount < contours.size(); currentCount++)
	{
		int closestFlyPos = 0;


		if (poly_contour_radius[currentCount] >= minRadius && poly_contour_radius[currentCount] <= maxRadius)
		{
			Fly tempFly(contours[currentCount], poly_contour_center[currentCount], poly_contour_radius[currentCount]);

			closestFlyPos = swarm.nearestFly(tempFly);

			if (swarm.size() > 0 && swarm.getDistance(closestFlyPos, tempFly) <= minDistance)
			{

				// If the state of the selected fly is true, then proceed
				if (swarm.getState(closestFlyPos))
				{
					swarm.replaceFly(closestFlyPos, tempFly);
				}

			}
			else
			{

				if (swarm.getTotalActiveCount() < maxFlies)
				{
					swarm.addFly(tempFly);
				}
			}
		}
	}

	for (int count = 0; count < contours.size(); ++count)
	{
		cv::drawContours(contourFrame, contours, count, Scalar(128, 255, 0), 2, 8, hierarchy, 0, Point());

	}
	
	std::cout << swarm.getTotalActiveCount() << std::endl;

	for (Fly tempFly : swarm.getTotalActiveFlies())
	{

		circle(contourFrame, tempFly.getCenter(), (int)tempFly.getRadius(), Scalar(0, 255, 0), 2, 8, 0);
		putText(contourFrame, std::to_string(tempFly.positionInSwarm), tempFly.getCenter(), FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0), 1, 8, false);

	}

}


cv::Mat Camera::frame()
{
	cv::Mat tempFrame; 

	switch (currentMode)
	{
	case CameraMode::NORMAL:
		cv::cvtColor(normalFrame, tempFrame, cv::COLOR_BGR2RGBA);
		break;

	case CameraMode::GRAYSCALE:
		cv::cvtColor(grayscaleFrame(), tempFrame, cv::COLOR_GRAY2RGBA);
		break;

	case CameraMode::HSV:
		tempFrame = hsvFrame();
		cv::cvtColor(tempFrame, tempFrame, cv::COLOR_BGR2RGBA);
		break;

	case CameraMode::CONTOURS:
		cv::cvtColor(contourFrame, tempFrame, cv::COLOR_BGR2RGBA);
		break;
	}

	// Needs to be sent out as a RGBA frame
	return tempFrame;
}

void Camera::openCamera(int passedNum)
{
	captureDevice.open(passedNum);

	if (!captureDevice.isOpened())
	{
		std::cout << "Could Not Find Camera" << std::endl;

		captureDevice.open(currentCameraNumber);
	}
	else
	{
		currentCameraNumber = passedNum;
	}
}

void Camera::setMode(CameraMode passedEnum)
{

	currentMode = passedEnum;

}
