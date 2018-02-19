#pragma once

#include <iostream>
#include <vector>

// Constants
const float quadExpansionPadding = 0.1f;
const int maxPointsPerQuad = 1000;

/* The following structs are packed with no padding. */
#pragma pack(push, 1)

struct Point
{
	int8_t id;
	int32_t rank;
	float x;
	float y;

	Point()
		:id(0),
		rank(0),
		x(0),
		y(0)
	{}

	Point(const Point& p)
		:id(p.id),
		rank(p.rank),
		x(p.x),
		y(p.y)
	{}

	Point(float _x, float _y)
		:id(0),
		rank(0),
		x(_x),
		y(_y)
	{}

	Point(float _x, float _y, int32_t _rank)
		:id(0),
		rank(_rank),
		x(_x),
		y(_y)
	{}

	friend std::ostream& operator <<(std::ostream& stream, const Point& point);

	bool operator < (const Point& rhs) const
	{
		return rank > rhs.rank;
	}
};

struct Rect
{
	float lx;
	float ly;
	float hx;
	float hy;

	Rect()
		:lx(0.0f), ly(0.0f),
		hx(0.0f), hy(0.0f)
	{}

	Rect(float _lx, float _ly, float _hx, float _hy)
		:lx(_lx), ly(_ly),
		hx(_hx), hy(_hy)
	{}

	// Returns the center point
	Point GetCenter() const;

	// Returns true if the point is within these bounds
	bool Contains(const Point& point) const;

	// Returns true if the given rect is entirely within this one
	bool Contains(const Rect& rect) const;

	// Returns true if the rect intersects this one
	bool Intersects(const Rect& rect) const;

	// Expand the bounds of this quad to contain the given point
	void Expand(const Point& point);

	friend std::ostream& operator << (std::ostream& stream, const Rect& rect);
};

#pragma pack(pop)

class QuadTree
{
	// How many levels deep are we? (0-indexed)
	int level;

	Rect bounds;
	std::vector<Point*> points;

	// Child trees, if they exist
	QuadTree* topLeft;
	QuadTree* topRight;
	QuadTree* bottomLeft;
	QuadTree* bottomRight;

public:

	QuadTree(const Rect& _bounds, int _level = 0)
		:topLeft(nullptr),
		topRight(nullptr),
		bottomLeft(nullptr),
		bottomRight(nullptr),
		bounds(_bounds),
		level(_level)
	{
		// std::cout << "Create quad: " << bounds << ", level: " << level << std::endl;
	}

	// Returns whether the given point falls within our bounds
	bool CanContain(const Point& point) const;

	// Check whether this is a leaf node
	bool IsLeaf() const;

	// Add a point to the tree, subdividing as needed
	void InsertPoint(Point* point);

	// Create four child trees from this one, placing the point within the appropriate one
	void Subdivide();

	// Return all points in the quad tree located within the specified bounds
	std::vector<Point*> Query(const Rect& rect) const;

	// Destructor. Clean up child trees
	~QuadTree()
	{
		if (topLeft != nullptr)
		{
			delete topLeft;
		}

		if (topRight != nullptr)
		{
			delete topRight;
		}

		if (bottomLeft != nullptr)
		{
			delete bottomLeft;
		}

		if (bottomRight != nullptr)
		{
			delete bottomRight;
		}
	}
};