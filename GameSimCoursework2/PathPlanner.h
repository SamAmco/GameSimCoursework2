#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <limits>

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
struct Point
{
	int x;
	int y;
	Point* parent = NULL;
	int cost;

	int F;
	int G;
	int H;
};
const int _TILE_COSTS[5] = { 1, 1, 2, 3, -1 };

using namespace std;

class PathPlanner
{
public:
	PathPlanner(Tile* tileGrid, int tileGridSizeX,
		int tileGridSizeY, int aTile, int bTile);

	bool step();

	~PathPlanner();

private:
	vector<Point*> findReachablePoints(Point* p);
	Point* getPoint(int x, int y, Point* parent);
	bool isPointInList(vector<Point*> list, int x, int y);
	Tile* getTileAtPoint(Point p);
	int calculateG(Point* p);

	Tile* tileGrid;
	int tileGridSizeX;
	int tileGridSizeY;
	Point* endPoint;
	Point* currentPoint;
	vector<Point*> openList;
	vector<Point*> closedList;
};

