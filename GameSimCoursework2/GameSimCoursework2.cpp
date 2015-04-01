// GameSimCoursework2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "PathPlanner.h"

using namespace std;

enum State
{
	EXPECTING_A,
	EXPECTING_B,
	PLAN_PATH,
	COMPLETE
};

void readInTiles(string fileName);
Tile createTriangle(int x, int y, TriangleType type);

const float _SPACE_BETWEEN_TRIANGLES_X = 70;
const float _SPACE_BETWEEN_TRIANGLES_Y = 70;
const float _X_AXIS_OFFSET = 30;
const float _Y_AXIS_OFFSET = 35;

sf::Font font;

State currentState;
int aTile = -1;
int bTile = -1;
Tile* tileGrid;
sf::Text* axis;
int tileGridSizeX;
int tileGridSizeY;


void readInTiles(string fileName)
{
	FILE* file;
	errno_t err = fopen_s(&file, fileName.c_str(), "r");
	if (err)
	{
		cout << "Impossible to open the file " << fileName << "\n";
		return;
	}

	
	fscanf_s(file, "%i,%i\n", &tileGridSizeX, &tileGridSizeY);

	int tileGridSize = tileGridSizeX * tileGridSizeY;
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

	float xPos = _X_AXIS_OFFSET +  (x * (_SPACE_BETWEEN_TRIANGLES_X / 2.f));
	float yPos = _Y_AXIS_OFFSET + (y * _SPACE_BETWEEN_TRIANGLES_Y);

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
		triangle.setFillColor(sf::Color(0,170,200));
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
	text.setColor(sf::Color::Black);
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
	currentState = State::EXPECTING_A;

	readInTiles("TileData.txt");
	axis = new sf::Text[tileGridSizeX + tileGridSizeY];
	for (int i = 0; i < tileGridSizeX; ++i)
	{
		axis[i] = sf::Text((char)(i + 97), font, 16);
		axis[i].setColor(sf::Color::White);
		axis[i].setPosition(sf::Vector2f((i * _SPACE_BETWEEN_TRIANGLES_X * .5f) + _X_AXIS_OFFSET + (_SPACE_BETWEEN_TRIANGLES_X * .4f), 0));
	}
	for (int i = tileGridSizeX; i < tileGridSizeX + tileGridSizeY; ++i)
	{
		stringstream s;
		int x = i - tileGridSizeX;
		s << x + 1;
		axis[i] = sf::Text(s.str(), font, 16);
		axis[i].setColor(sf::Color::White);
		axis[i].setPosition(sf::Vector2f(0, (x * _SPACE_BETWEEN_TRIANGLES_Y) + _Y_AXIS_OFFSET + (_SPACE_BETWEEN_TRIANGLES_Y * .3f)));
	}


	PathPlanner* pathPlanner = NULL;

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

		if (pathPlanner != NULL)
		{
			if (pathPlanner->step())
			{
				delete pathPlanner;
				pathPlanner = NULL;
			}
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		// draw everything here...
		for (int i = 0; i < tileGridSizeX + tileGridSizeY; ++i)
		{
			if (i == aTile || i == bTile)
				continue;
			window.draw(axis[i]);
		}
		for (int i = 0; i < tileGridSizeX * tileGridSizeY; ++i)
		{		
			if (i == aTile || i == bTile)
				continue;
			window.draw(tileGrid[i].triangle);
		}
		for (int i = 0; i < tileGridSizeX * tileGridSizeY; ++i)
		{
			if (i == aTile)
				continue;
			window.draw(tileGrid[i].text);
		}
		if (aTile != -1)
		{
			window.draw(tileGrid[aTile].triangle);
			window.draw(tileGrid[aTile].text);
		}
		if (bTile != -1)
		{
			window.draw(tileGrid[bTile].triangle);
			window.draw(tileGrid[bTile].text);
		}

		// end the current frame
		window.display();

		switch (currentState)
		{
		case State::COMPLETE:
			break;
		case State::EXPECTING_A:
			char aChar;
			int aNum;
			cout << "Please enter a start point letter: " << endl;
			cin >> aChar;
			cout << "Please enter a start point number: " << endl;
			cin >> aNum;
			aTile = ((aNum - 1) * tileGridSizeX) + (aChar - 97);
			tileGrid[aTile].triangle.setOutlineThickness(3);
			tileGrid[aTile].triangle.setOutlineColor(sf::Color::Red);
			currentState = State::EXPECTING_B;
			break;
		case State::EXPECTING_B:
			char bChar;
			int bNum;
			cout << "Please enter an end point letter: " << endl;
			cin >> bChar;
			cout << "Please enter an end point number: " << endl;
			cin >> bNum;
			bTile = ((bNum - 1) * tileGridSizeX) + (bChar - 97);
			tileGrid[bTile].triangle.setOutlineThickness(3);
			tileGrid[bTile].triangle.setOutlineColor(sf::Color::Red);
			currentState = State::PLAN_PATH;
			break;
		case State::PLAN_PATH:
			pathPlanner = new PathPlanner(tileGrid, tileGridSizeX, tileGridSizeY, aTile, bTile);
			currentState = State::COMPLETE;
			break;
		}
	}

	delete[] tileGrid;
	delete[] axis;

	return 0;
}



