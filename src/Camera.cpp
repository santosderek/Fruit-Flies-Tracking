#include "Camera.h"

using namespace cv;

Camera::Camera()
	: captureDevice()
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

void Camera::restoreToDefault()
{
	//Minimum and Maximum Threshold to filter different contours out
	minThresh = 46;
	maxThresh = 255;

	//Minimum and Maximum Radius' for the circle closing in.
	minRadius = 1;
	maxRadius = 90;

	currentMode = CameraMode::NORMAL; 

	int leftBound = 0;
	int rightBound = (480.f / 4.f);
	int count = 0;

	for (Vile& eachVile : activeViles)
	{
		//std::cout << "Vile " << ++count << ": Left Bound: " << leftBound << ", Right Bound: " << rightBound << std::endl;
		
		eachVile.setBounds(leftBound, rightBound);
		leftBound += (480.f / 4.f);
		rightBound += (480.f / 4.f);

	}
	
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
	cv::Mat tempFrame; 
	
	cv::cvtColor(normalFrame, tempFrame, cv::COLOR_BGR2HSV);

	return tempFrame;
}

cv::Mat Camera::thresholdFrame()
{
	cv::Mat thresh; 

	cv::threshold(grayscaleFrame(), thresh, minThresh, maxThresh, THRESH_BINARY_INV);

	return thresh;

}

void Camera::processContours()
{
	for (Vile& eachVile : activeViles)
	{
		eachVile.checkActive();
	}

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


		if (poly_contour_radius[currentCount] >= minRadius && poly_contour_radius[currentCount] <= maxRadius)
		{

			Fly tempFly(contours[currentCount], poly_contour_center[currentCount], poly_contour_radius[currentCount]);
			for (Vile& eachVile : activeViles)
			{

				if (eachVile.withinBounds(tempFly.getCenter().x))
				{
					eachVile.analyze(tempFly);
				}
			}
			
		}
	}
	
	for (int count = 0; count < contours.size(); ++count)
	{
		cv::drawContours(contourFrame, contours, count, Scalar(128, 255, 0), 2, 8, hierarchy, 0, Point());

	}
	
	for (Vile eachVile : activeViles)
	{
		for (Fly& eachFly : eachVile.activeFlies())
		{

			circle(contourFrame, eachFly.getCenter(), (int)eachFly.getRadius(), Scalar(0, 255, 0), 2, 8, 0);
			putText(contourFrame, std::to_string(eachFly.positionInSwarm), eachFly.getCenter(), FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0), 1, 8, false);

		}
	}
	

}

std::string Camera::giveInformation()
{
	int totalActiveFlies = 0;

	for (Vile& eachVile : activeViles)
	{
		totalActiveFlies += eachVile.activeFlyCount();
		//std::cout << eachVile.activeFlyCount() << " ";

	}
	
	int totalFlies = 0; 
	int eachVileTotal[4] = {0,0,0,0};
	int count = 0;

	for (Vile& eachVile : activeViles)
	{
		
		totalFlies += eachVile.getTotalFlyCount();
		eachVileTotal[count] = eachVile.getTotalFlyCount();
		//std::cout << eachVile.getTotalFlyCount() << " ";
		count++;
	}
	//std::cout << std::endl; 

	std::string information =
		(std::string) "Total Active Flies: " + std::to_string(totalActiveFlies) + "\n" +
		(std::string) "Total Number of Flies: " + std::to_string(totalFlies)    + "\n" +
		(std::string) "Vile 1: " + std::to_string(eachVileTotal[0]) + "\n" +
		(std::string) "Vile 2: " + std::to_string(eachVileTotal[1]) + "\n" +
		(std::string) "Vile 3: " + std::to_string(eachVileTotal[2]) + "\n" +
		(std::string) "Vile 4: " + std::to_string(eachVileTotal[3]);
								

	return information;

}


cv::Mat Camera::frame()
{
	cv::Mat tempFrame; 

	switch (currentMode)
	{
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
	
	default:
		cv::cvtColor(normalFrame, tempFrame, cv::COLOR_BGR2RGBA);
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
