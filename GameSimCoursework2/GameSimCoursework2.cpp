// GameSimCoursework2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>

using namespace std;


enum TriangleType
{
	R, L, H, F, M
};

struct Tile
{
	TriangleType type;
	sf::ConvexShape triangle;
	sf::Text text;
};

Tile createTriangle(int x, int y, TriangleType type);

const float _SPACE_BETWEEN_TRIANGLES_X = 70;
const float _SPACE_BETWEEN_TRIANGLES_Y = 70;

sf::Font font;

Tile* tileGrid;
int tileGridSize;

void readInTiles(string fileName)
{
	FILE* file;
	errno_t err = fopen_s(&file, fileName.c_str(), "r");
	if (err)
	{
		cout << "Impossible to open the file " << fileName << "\n";
		return;
	}

	int maxWidth;
	int maxHeight;
	fscanf_s(file, "%i,%i\n", &maxWidth, &maxHeight);

	tileGridSize = maxHeight * maxWidth;
	tileGrid = new Tile[tileGridSize];


	//Loop through the lines
	for (int i = 0; i < tileGridSize; ++i)
	{		
		int x = 0;
		int y = 0;
		char t = ' ';
		int res = fscanf_s(file, "%i,%i,%c\n", &x, &y, &t);

		if (res == EOF)
			break;

		//R, L, H, F, M
		TriangleType type;
		switch (t)
		{
		case 'R':
			type = TriangleType::R;
			break;
		case 'L':
			type = TriangleType::L;
			break;
		case 'H':
			type = TriangleType::H;
			break;
		case 'F':
			type = TriangleType::F;
			break;
		case 'M':
			type = TriangleType::M;
			break;
		default:
			break;
		}

		tileGrid[i] = createTriangle(x,y,type);
	}
}


Tile createTriangle(int x, int y, TriangleType type)
{

	float radius = _SPACE_BETWEEN_TRIANGLES_Y / 2.f;

	sf::ConvexShape triangle;
	// resize it to 5 points
	triangle.setPointCount(3);

	float xPos = x * (_SPACE_BETWEEN_TRIANGLES_X / 2.f);
	float yPos = y * _SPACE_BETWEEN_TRIANGLES_Y;

	// define the points
	if ((x + y) % 2 == 0)
	{
		triangle.setPoint(0, sf::Vector2f(xPos, yPos));
		triangle.setPoint(1, sf::Vector2f(xPos + _SPACE_BETWEEN_TRIANGLES_X, yPos));
		triangle.setPoint(2, sf::Vector2f(xPos + (_SPACE_BETWEEN_TRIANGLES_X / 2.f), yPos + _SPACE_BETWEEN_TRIANGLES_Y));
	}
	else
	{
		triangle.setPoint(0, sf::Vector2f(xPos + (_SPACE_BETWEEN_TRIANGLES_X / 2.f), yPos));
		triangle.setPoint(1, sf::Vector2f(xPos, yPos + _SPACE_BETWEEN_TRIANGLES_Y));
		triangle.setPoint(2, sf::Vector2f(xPos + _SPACE_BETWEEN_TRIANGLES_X, yPos + _SPACE_BETWEEN_TRIANGLES_Y));
	}

	switch (type)
	{
	case TriangleType::F:
		triangle.setFillColor(sf::Color(0,100,0));
		break;
	case TriangleType::H:
		triangle.setFillColor(sf::Color::Green);
		break;
	case TriangleType::M:
		triangle.setFillColor(sf::Color::White);
		break;
	case TriangleType::L:
		triangle.setFillColor(sf::Color::Blue);
		break;
	case TriangleType::R:
		triangle.setFillColor(sf::Color(50,50,50));
		break;
	default:
		break;
	}
	triangle.setOutlineThickness(1);
	triangle.setOutlineColor(sf::Color::Black);

	stringstream s;
	s << x << ", " << y;
	sf::Text text = sf::Text(s.str(), font, 16);
	text.setColor(sf::Color::Red);
	if ((x + y) % 2 == 0)
		text.setPosition(sf::Vector2f(xPos + (_SPACE_BETWEEN_TRIANGLES_X * .25f), yPos + (_SPACE_BETWEEN_TRIANGLES_Y * .15f)));
	else
		text.setPosition(sf::Vector2f(xPos + (_SPACE_BETWEEN_TRIANGLES_X * .25f), yPos + (_SPACE_BETWEEN_TRIANGLES_Y * .6f)));


	Tile tile;
	tile.type = type;
	tile.triangle = triangle;
	tile.text = text;

	return tile;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(800, 600), "My window");

	if (!font.loadFromFile("arial.ttf"))
	{
		return -1;
	}

	readInTiles("TileData.txt");
	
	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		// draw everything here...
		for (int i = 0; i < tileGridSize; i++)
		{
			window.draw(tileGrid[i].triangle);
		}
		for (int i = 0; i < tileGridSize; i++)
		{
			window.draw(tileGrid[i].text);
		}

		// end the current frame
		window.display();
	}

	return 0;
}



