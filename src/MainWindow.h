#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <assert.h>
#include "Camera.h"

class MainWindow
{
private:
	sf::RenderWindow window;
	sf::Vector2f windowSize;

	sf::Font font;

	sf::Text commandLoaded;
	sf::Text statsOfFlies;

	sf::Image finalFrame;
	sf::Texture finalTexture;
	sf::Sprite finalSprite; 


	// Current Active Camera
	Camera activeCamera;

	void processUserEvents(sf::Event); 
	void processFrame(); 
	void getFrame();
	void display();


	


public:
	MainWindow(sf::Vector2f);
	~MainWindow();


	void run(); 

};

