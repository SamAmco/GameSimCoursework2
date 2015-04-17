#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <iostream>
#include <vector>
#include <limits>

enum TriangleType
{
	R, L, H, F, M
};

//The tile data is stored seperately from the node data below

//this struct is used for graphical output of the tiles mainly, but also storing the tile types
struct Tile
{
	TriangleType type;
	sf::ConvexShape triangle;
	sf::Text text;
};

//this struct is used by the path finding algorithm its self
struct Node
{
	int x;
	int y;
	Node* parent = NULL;
	int cost;

	int F;
	int G;
	int H;
};

//these can be indexed by the integer values of the TriangleType enumerator.
//Mountains have a special value of -1 to indicate that they're un-walkable
const int _TILE_COSTS[5] = { 1, 1, 2, 3, -1 };

using namespace std;

class PathPlanner
{
public:

	//this constructor will initialize the path finding between two nodes
	PathPlanner(Tile* tileGrid, tgui::ChatBox::Ptr& outBox, int tileGridSizeX,
		int tileGridSizeY, int aTile, int bTile);

	//this function will step the algorithm to the next node, and return true when the algorithm 
	//is complete (i.e. found the path or determined it is impossible)
	bool step();

	//delete all nodes on the deleteList
	~PathPlanner();

private:
	//Find all nodes that are reachable by a particular node. This takes account of river crossing
	vector<Node*> findReachableNodes(Node* p);
	//Construct a node with the relevant values for a given position. If this node can not be reached, NULL is returned
	Node* getNode(int x, int y, Node* parent);
	//For example checking if a node is in the open or closed list
	bool isNodeInList(vector<Node*> list, int x, int y);
	//retreive a node with the same x and y values from a list. Returns NULL if none is found
	int getNodeInList(vector<Node*> list, int x, int y);
	//retreive the tile from tileGrid with the same position as p
	Tile* getTileAtNode(Node p);
	//calculate the cost of getting to a given node by following its heritage and summing the costs of the parents
	int calculateG(Node* p);

	//a pointer to the tile grid loaded in 
	Tile* tileGrid;
	//a pointer to the tgui ChatBox that information should be printed to
	tgui::ChatBox::Ptr& outBox;
	//the size of the tileGrid in x and y
	int tileGridSizeX;
	int tileGridSizeY;
	//we need to store the node that we are trying to reach from the start node, so that we can reference it
	//e.g. for calculating the heuristic 
	Node* endNode;
	//These are standard for A*
	vector<Node*> openList;
	vector<Node*> closedList;
	//This makes it easier to manage deletion of Node data and therefore keeps the code more concise
	vector<Node*> deleteList;
};

