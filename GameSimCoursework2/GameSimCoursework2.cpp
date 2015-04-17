// GameSimCoursework2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <string> 
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "PathPlanner.h"


using namespace std;

//all functions in this class are forward declared here
void readInTiles(string fileName);
Tile createTriangle(int x, int y, TriangleType type);
void loadGUI(tgui::Gui& gui);
int clamp(int x, int max, int min);
void setTriangleColor(sf::ConvexShape& triangle, TriangleType type);

//variables for displaying tiles in the gui
const float _SPACE_BETWEEN_TRIANGLES_X = 70;
const float _SPACE_BETWEEN_TRIANGLES_Y = 70;
const float _X_AXIS_OFFSET = 30;
const float _Y_AXIS_OFFSET = 35;

//only using one font for all text
sf::Font font;

//the positions in the tile grid array of the start and end points
int aTile = -1;
int bTile = -1;
//the array of tiles read in from the text file
Tile* tileGrid;
//the axis displayed
sf::Text* axis;
//these values are set when the tile grid is read in
int tileGridSizeX;
int tileGridSizeY;

//we need to store these buttons so that we can edit them or read their values later
tgui::Button::Ptr submitButton;
tgui::Tab::Ptr typeTab;
tgui::ChatBox::Ptr outBox;


int clamp(int x, int max, int min)
{
	if (x < min)
		x = min;
	if (x > max)
		x = max;
	return x;
}

//set the colour of a triangular tile based on its type
void setTriangleColor(sf::ConvexShape& triangle, TriangleType type)
{
	switch (type)
	{
	case TriangleType::F:
		triangle.setFillColor(sf::Color(0, 100, 0));
		break;
	case TriangleType::H:
		triangle.setFillColor(sf::Color::Green);
		break;
	case TriangleType::M:
		triangle.setFillColor(sf::Color::White);
		break;
	case TriangleType::L:
		triangle.setFillColor(sf::Color(0, 170, 200));
		break;
	case TriangleType::R:
		triangle.setFillColor(sf::Color(50, 50, 50));
		break;
	default:
		break;
	}
	//in some cases after setting the colour we change the outline, e.g. start and end tiles. 
	//This always needs to be set back
	triangle.setOutlineThickness(1);
	triangle.setOutlineColor(sf::Color::Black);
}

//read a text file in and populate the tileGrid
void readInTiles(string fileName)
{
	FILE* file;
	errno_t err = fopen_s(&file, fileName.c_str(), "r");
	if (err)
	{
		cout << "Impossible to open the file " << fileName << "\n";
		return;
	}

	//the first line of the file contains the x and y sizes of the grid.
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

		//Create a triangle of the type read in.
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

//create a triangle graphic in sfml and return it along with it's type in a Tile object
Tile createTriangle(int x, int y, TriangleType type)
{
	//create a convex shape	
	sf::ConvexShape triangle;
	//resize it to 3 points
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

	setTriangleColor(triangle, type);

	//set the text in the triangle
	stringstream s;
	s << (char)(x + 97) << ", " << y + 1;
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
	// create the window and gui
	sf::RenderWindow window(sf::VideoMode(800, 800), "My window");
	tgui::Gui gui(window);
	
	//load and set the font
	if (!font.loadFromFile("arial.ttf"))
	{
		return EXIT_FAILURE;
	}
	gui.setGlobalFont(font);

	//read in the tile data file
	readInTiles("TileData.txt");

	//set the x axis text
	axis = new sf::Text[tileGridSizeX + tileGridSizeY];
	for (int i = 0; i < tileGridSizeX; ++i)
	{
		axis[i] = sf::Text((char)(i + 97), font, 16);
		axis[i].setColor(sf::Color::White);
		axis[i].setPosition(sf::Vector2f((i * _SPACE_BETWEEN_TRIANGLES_X * .5f) + _X_AXIS_OFFSET + (_SPACE_BETWEEN_TRIANGLES_X * .4f), 0));
	}

	//set the y axis text
	for (int i = tileGridSizeX; i < tileGridSizeX + tileGridSizeY; ++i)
	{
		stringstream s;
		int x = i - tileGridSizeX;
		s << x + 1;
		axis[i] = sf::Text(s.str(), font, 16);
		axis[i].setColor(sf::Color::White);
		axis[i].setPosition(sf::Vector2f(0, (x * _SPACE_BETWEEN_TRIANGLES_Y) + _Y_AXIS_OFFSET + (_SPACE_BETWEEN_TRIANGLES_Y * .3f)));
	}

	loadGUI(gui);
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

			gui.handleEvent(event);
		}

		//if we are currently planning a path, then step through the algorithm
		if (pathPlanner != NULL)
		{
			if (pathPlanner->step())
			{
				delete pathPlanner;
				pathPlanner = NULL;
			}
		}
		//otherwise respond to gui input
		else
		{
			// The callback loop
			tgui::Callback callback;
			while (gui.pollCallback(callback))
			{
				//if there is a gui event, check the id of the gui element
				if (callback.id == 1)
				{
					//submit button was clicked

					//find the aTile and bTile in tileGrid
					tgui::EditBox::Ptr startXBox = gui.get("startX");
					tgui::EditBox::Ptr startYBox = gui.get("startY");
					int startX = clamp((int)((string)startXBox->getText())[0] - 97, tileGridSizeX - 1, 0);
					int startY = clamp(stoi((string)startYBox->getText()) - 1, tileGridSizeY - 1, 0);

					tgui::EditBox::Ptr endXBox = gui.get("endX");
					tgui::EditBox::Ptr endYBox = gui.get("endY");
					int endX = clamp((int)((string)endXBox->getText())[0] - 97, tileGridSizeX - 1, 0);
					int endY = clamp(stoi((string)endYBox->getText()) - 1, tileGridSizeY - 1, 0);

					aTile = (startY * tileGridSizeX) + startX;
					bTile = (endY * tileGridSizeX) + endX;
					
					//Set the gui to reflect the tiles selected
					tileGrid[aTile].triangle.setOutlineThickness(3);
					tileGrid[aTile].triangle.setOutlineColor(sf::Color::Red);
					tileGrid[bTile].triangle.setOutlineThickness(3);
					tileGrid[bTile].triangle.setOutlineColor(sf::Color::Red);
					
					//begin the path planner
					pathPlanner = new PathPlanner(tileGrid, outBox, tileGridSizeX, tileGridSizeY, aTile, bTile);
					
					//the user must reset the tile grid before they can submit a new path
					submitButton->disable();
					submitButton->hide();
				}
				if (callback.id == 2)
				{
					//the reset button was clicked

					//reset all tile graphics and re-enable the submit button
					for (int i = 0; i < tileGridSizeX * tileGridSizeY; ++i)
					{
						setTriangleColor(tileGrid[i].triangle, tileGrid[i].type);
					}
					submitButton->show();
					submitButton->enable();
				}
				if (callback.id == 3)
				{
					//the change tile button was clicked
					tgui::EditBox::Ptr startXBox = gui.get("tileX");
					tgui::EditBox::Ptr startYBox = gui.get("tileY");
					int tileX = clamp((int)((string)startXBox->getText())[0] - 97, tileGridSizeX - 1, 0);
					int tileY = clamp(stoi((string)startYBox->getText()) - 1, tileGridSizeY - 1, 0);
					int index = (tileGridSizeX * tileY) + tileX;
					tileGrid[index].type = (TriangleType)typeTab->getSelectedIndex();
					setTriangleColor(tileGrid[index].triangle, tileGrid[index].type);
				}
			}
		}


		

		// clear the window with black color
		window.clear(sf::Color::Black);

		// draw the axis
		for (int i = 0; i < tileGridSizeX + tileGridSizeY; ++i)
		{
			if (i == aTile || i == bTile)
				continue;
			window.draw(axis[i]);
		}
		//draw all tiles except aTile and bTile
		for (int i = 0; i < tileGridSizeX * tileGridSizeY; ++i)
		{		
			if (i == aTile || i == bTile)
				continue;
			window.draw(tileGrid[i].triangle);
		}
		//draw the text for all tiles except aTile and bTile
		for (int i = 0; i < tileGridSizeX * tileGridSizeY; ++i)
		{
			if (i == aTile || i == bTile)
				continue;
			window.draw(tileGrid[i].text);
		}
		//draw aTile and bTile over the top because they have thicker outlines that must overlay the surrounding tiles.
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

		gui.draw();

		// end the current frame
		window.display();
	}

	delete[] tileGrid;
	delete[] axis;
	if (pathPlanner != NULL)
		delete pathPlanner;

	return EXIT_SUCCESS;
}

//initialize the gui elements
void loadGUI(tgui::Gui& gui)
{
	//the output box that gives information about the path finding algorithm
	outBox = tgui::ChatBox::Ptr(gui);
	outBox->load("widgets/Black.conf");
	outBox->setSize(540, 180);
	outBox->setTextSize(20);
	outBox->setPosition(5, 605);

	//these values can be changed to move the start, end and submit elements of the interface
	int changeStartEndAnchorX = 550;
	int changeStartEndAnchorY = 605;

	tgui::Label::Ptr labelEnterStartEnd(gui);
	labelEnterStartEnd->setText("Please enter a start and end point. ");
	labelEnterStartEnd->setPosition(changeStartEndAnchorX, changeStartEndAnchorY);
	labelEnterStartEnd->setTextSize(14);

	tgui::Label::Ptr labelStart(gui);
	labelStart->setText("Start:");
	labelStart->setPosition(changeStartEndAnchorX, changeStartEndAnchorY + 30);
	labelStart->setTextSize(14);

	tgui::Label::Ptr labelEnd(gui);
	labelEnd->setText("End:");
	labelEnd->setPosition(changeStartEndAnchorX, changeStartEndAnchorY + 60);
	labelEnd->setTextSize(14);

	tgui::EditBox::Ptr editBoxStartX(gui, "startX");
	editBoxStartX->load("widgets/BabyBlue.conf");
	editBoxStartX->setSize(25, 25);
	editBoxStartX->setPosition(changeStartEndAnchorX + 40, changeStartEndAnchorY + 20);
	editBoxStartX->setTextColor(sf::Color::Black);
	editBoxStartX->setMaximumCharacters(1);

	tgui::EditBox::Ptr editBoxStartY(gui, "startY");
	editBoxStartY->load("widgets/BabyBlue.conf");
	editBoxStartY->setSize(25, 25);
	editBoxStartY->setPosition(changeStartEndAnchorX + 80, changeStartEndAnchorY + 20);
	editBoxStartY->setTextColor(sf::Color::Black);
	editBoxStartY->setMaximumCharacters(1);

	tgui::EditBox::Ptr editBoxEndX(gui, "endX");
	editBoxEndX->load("widgets/BabyBlue.conf");
	editBoxEndX->setSize(25, 25);
	editBoxEndX->setPosition(changeStartEndAnchorX + 40, changeStartEndAnchorY + 50);
	editBoxEndX->setTextColor(sf::Color::Black);
	editBoxEndX->setMaximumCharacters(1);

	tgui::EditBox::Ptr editBoxEndY(gui, "endY");
	editBoxEndY->load("widgets/BabyBlue.conf");
	editBoxEndY->setSize(25, 25);
	editBoxEndY->setPosition(changeStartEndAnchorX + 80, changeStartEndAnchorY + 50);
	editBoxEndY->setTextColor(sf::Color::Black);
	editBoxEndY->setMaximumCharacters(1);

	submitButton = tgui::Button::Ptr(gui);
	submitButton->load("widgets/Black.conf");
	submitButton->setSize(70, 20);
	submitButton->setPosition(changeStartEndAnchorX + 120, changeStartEndAnchorY + 25);
	submitButton->setText("Submit");
	submitButton->bindCallback(tgui::Button::LeftMouseClicked);
	submitButton->setCallbackId(1);

	tgui::Button::Ptr button(gui);
	button->load("widgets/Black.conf");
	button->setSize(70, 20);
	button->setPosition(changeStartEndAnchorX + 120, changeStartEndAnchorY + 55);
	button->setText("Reset");
	button->bindCallback(tgui::Button::LeftMouseClicked);
	button->setCallbackId(2);


	//these values can be changed to move the change tile elements of the interface
	int changeTileAnchorX = 550;
	int changeTileAnchorY = 705;

	tgui::Label::Ptr labelEnterChangeTile(gui);
	labelEnterChangeTile->setText("Please enter a tile to change. ");
	labelEnterChangeTile->setPosition(changeTileAnchorX, changeTileAnchorY);
	labelEnterChangeTile->setTextSize(14);

	tgui::Label::Ptr whichTile(gui);
	whichTile->setText("Tile:");
	whichTile->setPosition(changeTileAnchorX, changeTileAnchorY + 50);
	whichTile->setTextSize(14);

	tgui::EditBox::Ptr tileX(gui, "tileX");
	tileX->load("widgets/BabyBlue.conf");
	tileX->setSize(25, 25);
	tileX->setPosition(changeTileAnchorX + 40, changeTileAnchorY + 40);
	tileX->setTextColor(sf::Color::Black);
	tileX->setMaximumCharacters(1);

	tgui::EditBox::Ptr tileY(gui, "tileY");
	tileY->load("widgets/BabyBlue.conf");
	tileY->setSize(25, 25);
	tileY->setPosition(changeTileAnchorX + 80, changeTileAnchorY + 40);
	tileY->setTextColor(sf::Color::Black);
	tileY->setMaximumCharacters(1);

	tgui::Button::Ptr changeButton(gui);
	changeButton->load("widgets/Black.conf");
	changeButton->setSize(70, 20);
	changeButton->setPosition(changeTileAnchorX + 120, changeTileAnchorY + 45);
	changeButton->setText("Change");
	changeButton->bindCallback(tgui::Button::LeftMouseClicked);
	changeButton->setCallbackId(3);

	typeTab = tgui::Tab::Ptr(gui);
	typeTab->load("widgets/Black.conf");
	typeTab->setTabHeight(20);
	typeTab->setPosition(changeTileAnchorX, changeTileAnchorY + 15);
	typeTab->add("R");
	typeTab->add("L");
	typeTab->add("H");
	typeTab->add("F");
	typeTab->add("M");
}


