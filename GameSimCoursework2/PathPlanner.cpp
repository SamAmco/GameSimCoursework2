#include "stdafx.h"
#include "PathPlanner.h"


PathPlanner::PathPlanner(Tile* tileGrid, tgui::ChatBox::Ptr& outBox, int tileGridSizeX,
	int tileGridSizeY, int aTile, int bTile)
	: tileGrid(tileGrid), outBox(outBox), tileGridSizeX(tileGridSizeX), tileGridSizeY(tileGridSizeY)
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
	deleteList.push_back(startPoint);
	deleteList.push_back(endPoint);
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

int PathPlanner::getPointInList(vector<Point*> list, int x, int y)
{
	for (int i = 0; i < list.size(); ++i)
	{
		if (list[i]->x == x && list[i]->y == y)
			return i;
	}
	return -1;
}

Point* PathPlanner::getPoint(int x, int y, Point* parent)
{
	TriangleType type = tileGrid[(y * tileGridSizeX) + x].type;
	if (type == TriangleType::L && parent->parent != NULL)
	{
		TriangleType parent1Type = tileGrid[(parent->y * tileGridSizeX) + parent->x].type;
		TriangleType parent2Type = tileGrid[(parent->parent->y * tileGridSizeX) + parent->parent->x].type;

		if (parent1Type == TriangleType::L && parent2Type == TriangleType::L)
			return NULL;
	}

	int cost = _TILE_COSTS[(int)type];
	if (cost == -1)
		return NULL;

	Point* point = new Point();
	deleteList.push_back(point);
	point->x = x;
	point->y = y;
	point->parent = parent;
	point->cost = cost;

	int distY = abs(endPoint->y - y);
	int distX = abs(endPoint->x - x);

	int yNotInX = ((distY - distX)+1) / 2;
	yNotInX = yNotInX < 0 ? 0 : yNotInX;
	int H = (yNotInX * 3) + distX + (distY - yNotInX);


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
	{
		outBox->addLine("No path could be found", sf::Color::White);
		return true;
	}

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
	Point* currentPoint = openList[index];
	openList.erase(openList.begin() + index);

	if (getTileAtPoint(*currentPoint)->type != TriangleType::L)
		closedList.push_back(currentPoint);


	getTileAtPoint(*currentPoint)->triangle.setFillColor(sf::Color::Blue);

	if (currentPoint->x == endPoint->x && currentPoint->y == endPoint->y)
	{
		stringstream s;
		s << "Path found with distance " << currentPoint->G;
		outBox->addLine(s.str(), sf::Color::White);
		while (currentPoint != NULL)
		{
			getTileAtPoint(*currentPoint)->triangle.setFillColor(sf::Color::Magenta);
			currentPoint = currentPoint->parent;
		}

		return true;
	}

	vector<Point*> availablePoints = findReachablePoints(currentPoint);
	for (int i = 0; i < availablePoints.size(); ++i)
	{
		Point* p = availablePoints[i];
		int openListIndex = getPointInList(openList, p->x, p->y);
		if (openListIndex != -1)
		{
			int newG = calculateG(p);

			if (openList[openListIndex]->G > newG)
				openList[openListIndex] = p;
		}
		else
			openList.push_back(p);
	}

	return false;
}

PathPlanner::~PathPlanner()
{
	for (int i = 0; i < deleteList.size(); ++i)
		delete deleteList[i];
}
