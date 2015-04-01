#pragma once
#include <SFML/Graphics.hpp>

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


class PathPlanner
{
public:
	PathPlanner(Tile* tileGrid) : tileGrid(tileGrid){};

	

	~PathPlanner();

private:
	Tile* tileGrid;
	int tileGridSizeX;
	int tileGridSizeY;
};

