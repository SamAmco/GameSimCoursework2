#include "stdafx.h"
#include "PathPlanner.h"


PathPlanner::PathPlanner(Tile* tileGrid, int tileGridSizeX,
	int tileGridSizeY, int aTile, int bTile)
	: tileGrid(tileGrid), tileGridSizeX(tileGridSizeX), tileGridSizeY(tileGridSizeY)
{
	Point* startPoint = new Point();
	startPoint->y = aTile / tileGridSizeX;
	startPoint->x = aTile % tileGridSizeX;

	endPoint = new Point();
	endPoint->y = bTile / tileGridSizeX;
	endPoint->x = bTile % tileGridSizeX;

	closedList = vector<Point*>();
	openList = vector<Point*>();
	openList.push_back(startPoint);
};


bool PathPlanner::isPointInList(vector<Point*> list, int x, int y)
{
	for (auto point : list)
	{
		if (point->x == x && point->y == y)
			return true;
	}
	return false;
}

Point* PathPlanner::getPoint(int x, int y, Point* parent)
{
	int cost = _TILE_COSTS[(int)(tileGrid[(y * tileGridSizeX) + x].type)];
	if (cost == -1)
		return NULL;

	Point* point = new Point();
	point->x = x;
	point->y = y;
	point->parent = parent;
	point->cost = cost;

	int distY = abs(endPoint->y - y);
	int distX = abs(endPoint->x - x);
	int H = ((distY - distX) * 3) + distX + (distY - (distY - distX));


	int G = calculateG(point);

	point->H = H;
	point->G = G;
	point->F = G + H;

	return point;
}

vector<Point*> PathPlanner::findReachablePoints(Point* p)
{
	vector<Point*> points = vector<Point*>();

	int x = p->x;
	int y = p->y;

	//check left and right
	if (x > 0 && !isPointInList(closedList, x - 1, y))
	{
		Point* left = getPoint(x - 1, y, p);

		if (left != NULL)
			points.push_back(left);
	}
	if (x < tileGridSizeX - 1 && !isPointInList(closedList, x + 1, y))
	{
		Point* right = getPoint(x + 1, y, p);
		
		if (right != NULL)
			points.push_back(right);
	}

	//Check up or down
	if ((x + y) % 2 == 0)
	{
		if (y > 0 && !isPointInList(closedList, x, y - 1))
		{
			Point* up = getPoint(x, y - 1, p);
			
			if (up != NULL)
				points.push_back(up);
		}
	}
	else
	{
		if (y < tileGridSizeY - 1 && !isPointInList(closedList, x, y + 1))
		{
			Point* down = getPoint(x, y + 1, p);

			if (down != NULL)
				points.push_back(down);
		}
	}

	return points;
}

Tile* PathPlanner::getTileAtPoint(Point p)
{
	return &tileGrid[(p.y * tileGridSizeX) + p.x];
}

int PathPlanner::calculateG(Point* p)
{
	int G = 0;
	Point* currentPoint = p;
	while (currentPoint->parent != NULL)
	{
		G += currentPoint->cost;
		currentPoint = currentPoint->parent;
	}
	return G;
}

bool PathPlanner::step()
{
	if (openList.size() < 1)
		return true;

	int lowestF = numeric_limits<int>::max();
	int index = 0;
	for (int i = 0; i < openList.size(); ++i)
	{
		if (openList[i]->F <= lowestF)
		{
			lowestF = openList[i]->F;
			index = i;
		}
	}
	currentPoint = openList[index];
	openList.erase(openList.begin() + index);
	closedList.push_back(currentPoint);

	getTileAtPoint(*currentPoint)->triangle.setFillColor(sf::Color::Blue);

	if (currentPoint->x == endPoint->x && currentPoint->y == endPoint->y)
	{
		cout << currentPoint->G;
		while (currentPoint != NULL)
		{
			getTileAtPoint(*currentPoint)->triangle.setFillColor(sf::Color::Magenta);
			currentPoint = currentPoint->parent;
		}

		return true;
	}

	vector<Point*> availablePoints = findReachablePoints(currentPoint);
	for (auto p : availablePoints)
	{
		if (isPointInList(openList, p->x, p->y))
		{
			Point* temp = p->parent;
			p->parent = currentPoint;
			int newG = calculateG(p);

			if (p->G < newG)
				p->parent = temp;
			else
				p->G = newG;
		}
		else
			openList.push_back(p);
	}

	return false;
}


PathPlanner::~PathPlanner()
{
	for (int i = 0; i < openList.size(); ++i)
		delete openList[i];
	for (int i = 0; i < closedList.size(); ++i)
		delete closedList[i];

	delete endPoint;
}
