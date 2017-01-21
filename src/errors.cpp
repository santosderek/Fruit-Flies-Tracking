/*
Author: Derek Santos


Explanation:
	Detect flies and track. Explanation still in the works.

*/

#include "errors.h"

/* Just a namespace of any errors that we might encounter */
/* A quicker way for me to depoly generic errors */
namespace err
{
	void errorMesg(std::string message)
	{
		// Display Error Mesage
		std::cout << message << std::endl;

		// Destory Any and Every Window
		cv::destroyAllWindows();

		// Make Sure User Can See message
		system("pause");

		// Exit With Error Code One
		exit(1);
	}
}
