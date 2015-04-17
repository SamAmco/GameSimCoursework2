#include "stdafx.h"
#include "PathPlanner.h"



PathPlanner::PathPlanner(Tile* tileGrid, tgui::ChatBox::Ptr& outBox, int tileGridSizeX,
	int tileGridSizeY, int aTile, int bTile)
	: tileGrid(tileGrid), outBox(outBox), tileGridSizeX(tileGridSizeX), tileGridSizeY(tileGridSizeY)
{
	Node* startNode = new Node();
	startNode->y = aTile / tileGridSizeX;
	startNode->x = aTile % tileGridSizeX;

	endNode = new Node();
	endNode->y = bTile / tileGridSizeX;
	endNode->x = bTile % tileGridSizeX;

	//in A* we always start by adding the start node to the open list
	closedList = vector<Node*>();
	openList = vector<Node*>();
	openList.push_back(startNode);

	//whenever a node is created on the heap, it must be pushed to the delete list
	deleteList.push_back(startNode);
	deleteList.push_back(endNode);
};

bool PathPlanner::isNodeInList(vector<Node*> list, int x, int y)
{
	for (auto point : list)
	{
		if (point->x == x && point->y == y)
			return true;
	}
	return false;
}

int PathPlanner::getNodeInList(vector<Node*> list, int x, int y)
{
	for (int i = 0; i < list.size(); ++i)
	{
		if (list[i]->x == x && list[i]->y == y)
			return i;
	}
	return -1;
}

Node* PathPlanner::getNode(int x, int y, Node* parent)
{
	//First check the type of the node
	TriangleType type = tileGrid[(y * tileGridSizeX) + x].type;

	//if the type is lake, and the parent, and parents parent are also lakes, then this node is not reachable
	if (type == TriangleType::L && parent->parent != NULL)
	{
		TriangleType parent1Type = tileGrid[(parent->y * tileGridSizeX) + parent->x].type;
		TriangleType parent2Type = tileGrid[(parent->parent->y * tileGridSizeX) + parent->parent->x].type;

		if (parent1Type == TriangleType::L && parent2Type == TriangleType::L)
			return NULL;
	}

	//if the tile cost is -1, then the tile is unwalkable 
	int cost = _TILE_COSTS[(int)type];
	if (cost == -1)
		return NULL;

	//otherwise create the node 
	Node* point = new Node();
	deleteList.push_back(point);
	point->x = x;
	point->y = y;
	point->parent = parent;
	point->cost = cost;

	//this is the heuristic function. The heuristic should always return the closest value to the cost of traversing from
	//this node to the end node, whilst NEVER over-estimating that cost. The closer this value to the actual cost, the more 
	//efficient it will be, however, if the value is an over-estimate, A* ceases to assure that the shortest path will be returned.

	//distY and distX are simply the absolute distance from this node to the end node ond their respective axis
	int distY = abs(endNode->y - y);
	int distX = abs(endNode->x - x);

	//for every time we move in the y axis, that we do not also move in the x axis, we must traverse 3 nodes
	int yNotInX = ((distY - distX)+1) / 2;
	yNotInX = yNotInX < 0 ? 0 : yNotInX;
	int H = (yNotInX * 3) + distX + (distY - yNotInX);


	int G = calculateG(point);

	point->H = H;
	point->G = G;
	point->F = G + H;

	return point;
}

vector<Node*> PathPlanner::findReachableNodes(Node* p)
{
	//Add nodes immediately connected to this one, assuming certain criteria are met
	vector<Node*> points = vector<Node*>();

	int x = p->x;
	int y = p->y;

	//check left and right
	if (x > 0 && !isNodeInList(closedList, x - 1, y))
	{
		Node* left = getNode(x - 1, y, p);

		if (left != NULL)
			points.push_back(left);
	}
	if (x < tileGridSizeX - 1 && !isNodeInList(closedList, x + 1, y))
	{
		Node* right = getNode(x + 1, y, p);
		
		if (right != NULL)
			points.push_back(right);
	}

	//Check up or down
	if ((x + y) % 2 == 0)
	{
		if (y > 0 && !isNodeInList(closedList, x, y - 1))
		{
			Node* up = getNode(x, y - 1, p);
			
			if (up != NULL)
				points.push_back(up);
		}
	}
	else
	{
		if (y < tileGridSizeY - 1 && !isNodeInList(closedList, x, y + 1))
		{
			Node* down = getNode(x, y + 1, p);

			if (down != NULL)
				points.push_back(down);
		}
	}

	return points;
}

Tile* PathPlanner::getTileAtNode(Node p)
{
	return &tileGrid[(p.y * tileGridSizeX) + p.x];
}

int PathPlanner::calculateG(Node* p)
{
	int G = 0;
	Node* currentNode = p;
	while (currentNode->parent != NULL)
	{
		G += currentNode->cost;
		currentNode = currentNode->parent;
	}
	return G;
}

bool PathPlanner::step()
{
	//if there are no more possible paths to check, there is no path
	if (openList.size() < 1)
	{
		outBox->addLine("No path could be found", sf::Color::White);
		return true;
	}

	//go through the open list and find the node with the lowest f value
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
	//add this node to the closed list (UNLESS IT IS A LAKE NODE)
	Node* currentNode = openList[index];
	openList.erase(openList.begin() + index);

	if (getTileAtNode(*currentNode)->type != TriangleType::L)
		closedList.push_back(currentNode);


	getTileAtNode(*currentNode)->triangle.setFillColor(sf::Color::Blue);

	//check if we've found our way to the end node
	if (currentNode->x == endNode->x && currentNode->y == endNode->y)
	{
		stringstream s;
		s << "Path found with distance " << currentNode->G;
		outBox->addLine(s.str(), sf::Color::White);
		while (currentNode != NULL)
		{
			getTileAtNode(*currentNode)->triangle.setFillColor(sf::Color::Magenta);
			currentNode = currentNode->parent;
		}

		return true;
	}

	//add all nodes connected to this, that are not on the open list, to the open list
	//if they are on the open list, update their G value (assuming we have found a shorter path to them)
	vector<Node*> availableNodes = findReachableNodes(currentNode);
	for (int i = 0; i < availableNodes.size(); ++i)
	{
		Node* p = availableNodes[i];
		int openListIndex = getNodeInList(openList, p->x, p->y);
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
