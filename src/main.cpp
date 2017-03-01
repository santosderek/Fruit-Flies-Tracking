#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "MainWindow.h"

int main()
{
	
	MainWindow window(sf::Vector2f(1200.f,800.f)); 


	window.run();

	return EXIT_SUCCESS; 

}