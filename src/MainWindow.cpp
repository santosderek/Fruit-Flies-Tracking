#include "MainWindow.h"



MainWindow::MainWindow() :
	window(sf::VideoMode(1200, 800), "SFML TEST PROJECT", sf::Style::Default),
	activeCamera()
{
	// Set Framerate to 60 Frames Per Second
	// If framerate is giving trouble, control the framerate yourself. 
	window.setFramerateLimit(60);

	// Load Font

	if (!font.loadFromFile("arial.ttf"))
	{
		// If Font not found, ignore command.
		std::cout << "ERROR: FONT arial.ttf NOT FOUND" << std::endl;
		exit(1);
	}

	// Command Loaded
	commandLoaded.setFont(font);
	commandLoaded.setCharacterSize(14);
	commandLoaded.setFillColor(sf::Color::Green);
	commandLoaded.setPosition(2.0f, 0.0f);


	// Statistics of the Flies
	statsOfFlies.setFont(font);
	statsOfFlies.setCharacterSize(14);
	statsOfFlies.setFillColor(sf::Color::Green);
	statsOfFlies.setPosition(0.0f, 40.0f);

	commandLoaded.setString("Normal Frame Set!");
	activeCamera.openCamera(0);

	windowSize.x = 1200; 
	windowSize.y = 800;

}


MainWindow::~MainWindow()
{

}

void MainWindow::processUserEvents(sf::Event event)
{

	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
			
		// Gets The Desired Frame
		case (sf::Keyboard::N):
			activeCamera.setMode(CameraMode::NORMAL);
			commandLoaded.setString("Normal Frame Set!");
			break;
		case (sf::Keyboard::H):
			activeCamera.setMode(CameraMode::HSV);
			commandLoaded.setString("HSV Frame Set!");

			break;
		case (sf::Keyboard::T):
			activeCamera.setMode(CameraMode::THRESHOLD);
			commandLoaded.setString("THRESHOLD Frame Set!");

			break;
		case (sf::Keyboard::C):
			activeCamera.setMode(CameraMode::CONTOURS);
			commandLoaded.setString("CONTOURS Frame Set!");
			break;
		case (sf::Keyboard::G):
			activeCamera.setMode(CameraMode::GRAYSCALE);
			commandLoaded.setString("GRAYSCALE Frame Set!");
			break;

		// Open The Selected Camera
		case( sf::Keyboard::Num1 ):
			 activeCamera.openCamera(0);
			 break;
		case( sf::Keyboard::Num2 ):
			 activeCamera.openCamera(1);
			 break;
		case( sf::Keyboard::Num3 ):
			 activeCamera.openCamera(2);
			 break;
		case( sf::Keyboard::Num4 ):
			 activeCamera.openCamera(3);
			 break;
		case( sf::Keyboard::Num5 ):
			 activeCamera.openCamera(4);
			 break;


		}



	}


}

void MainWindow::getFrame()
{
	cv::Mat frameRGBA;
	frameRGBA = activeCamera.frame();
	
	if (frameRGBA.empty())
	{
		std::cout << "WARNING! NO FRAME FOUND!" << std::endl; 
		return;
	}

	finalFrame.create(frameRGBA.cols, frameRGBA.rows, frameRGBA.ptr());

	if (!finalTexture.loadFromImage(finalFrame))
	{
		std::cout << "ERROR TEXTURE COULD NOT LOAD" << std::endl;

	}

	finalSprite.setTexture(finalTexture);
	finalSprite.setScale(windowSize.x / finalSprite.getLocalBounds().width, 
						 windowSize.y / finalSprite.getLocalBounds().height);


}

void MainWindow::display()
{
	window.clear(sf::Color::White);
	
	// TODO: Move getFrame() to a processing void
	getFrame();
	window.draw(finalSprite);
	window.draw(commandLoaded);
	window.draw(statsOfFlies);
	window.display();
}
void MainWindow::processFrame()
{
	activeCamera.getFrame();
	activeCamera.processContours();
	statsOfFlies.setString(activeCamera.giveInformation());
}

void MainWindow::run()
{

	try
	{

		while (window.isOpen())
		{
			sf::Event event;

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}

				if (event.type == sf::Event::Resized)
				{
					windowSize = window.getView().getSize();
				}

				if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
				{
					processUserEvents(event);
				}

			}
			
			processFrame();
			display();

		}

	}
	catch (std::exception e)
	{
		std::cout << std::endl << e.what() << std::endl;
	}

}
