#include "QuadTree.h"

#include <ostream>
#include <algorithm>

bool comparePoints(Point* a, Point* b)
{
	return a->rank < b->rank;
}

// For debug printing a Point object
std::ostream & operator<<(std::ostream & stream, const Point & point)
{
	stream << std::fixed;
	// stream << "id: " << point.id << ", rank: " << point.rank << ", x: " << point.x << ", y: " << point.y;
	stream << "rank: " << point.rank << ", x: " << point.x << ", y: " << point.y;
	// stream << "(" << point.x << ", " << point.y << ")";
	return stream;
}

// Returns the center point
Point Rect::GetCenter() const
{
	return Point((lx + hx) / 2.0f, (ly + hy) / 2.0f);
}

// Returns true if the point is within these bounds
bool Rect::Contains(const Point& point) const
{
	return point.x >= lx &&
		point.x <= hx &&
		point.y >= ly &&
		point.y <= hy;
}

// Returns true if the given rect is entirely within this one
bool Rect::Contains(const Rect& rect) const
{
	return rect.lx > lx &&
		rect.ly > ly &&
		rect.hx < hx &&
		rect.hy < hy;
}

// Returns true if the rect intersects this one
bool Rect::Intersects(const Rect& rect) const
{
	return !((lx > rect.hx) ||		// To the right
		(ly > rect.hy) ||			// Above
		(hx < rect.lx) ||			// To the left
		(hy < rect.ly));			// Below
}

// Expand the bounds of this quad to contain the given point
void Rect::Expand(const Point& point)
{
	// left
	if (point.x < lx)
	{
		lx = point.x - quadExpansionPadding;
	}

	// right
	if (point.x > hx)
	{
		hx = point.x + quadExpansionPadding;
	}

	// top
	if (point.y > hy)
	{
		hy = point.y + quadExpansionPadding;
	}

	// bottom
	if (point.y < ly)
	{
		ly = point.y - quadExpansionPadding;
	}
}

// For debug printing a Rect object
std::ostream& operator << (std::ostream& stream, const Rect& rect)
{
	stream << std::fixed;
	stream << "(" << rect.lx << ", " << rect.ly << "), (" << rect.hx << ", " << rect.hy << ")";
	return stream;
}

// Compute tree depth
int QuadTree::GetDepth() const
{
	if (IsLeaf())
	{
		return level;
	}

	std::vector<int> childValues;
	childValues.push_back(topLeft->GetDepth());
	childValues.push_back(topRight->GetDepth());
	childValues.push_back(bottomLeft->GetDepth());
	childValues.push_back(bottomRight->GetDepth());

	int max = childValues[0];
	for (auto i = 0; i < childValues.size(); ++i)
	{
		if (childValues[i] > max)
		{
			max = childValues[i];
		}
	}

	return max;
}

// Returns whether the given point falls within our bounds
bool QuadTree::CanContain(const Point& point) const
{
	return bounds.Contains(point);
}

// Check whether this is a leaf node
bool QuadTree::IsLeaf() const
{
	// If one child exists, they all do
	return topLeft == nullptr;
}

// Add a point to the tree, subdividing as needed
void QuadTree::InsertPoint(Point* point)
{
	if (!bounds.Contains(*point))
	{
		throw std::exception("InsertPoint called with an out-of-bounds point.");
	}

	if (IsLeaf())
	{
		if (points.size() < maxPointsPerQuad)
		{
			// Easy case. Just add the point to this quad

			// std::cout << "InsertPoint: " << point << " at level: " << level << std::endl;
			points.push_back(point);
		}
		else
		{
			// Quad is now overcrowded. We need to subdivide
			Subdivide();
		}
	} 
	else
	{
		if (topLeft->CanContain(*point))
		{
			topLeft->InsertPoint(point);
		}
		else if (topRight->CanContain(*point))
		{
			topRight->InsertPoint(point);
		}
		else if (bottomLeft->CanContain(*point))
		{
			bottomLeft->InsertPoint(point);
		}
		else if (bottomRight->CanContain(*point))
		{
			bottomRight->InsertPoint(point);
		}
		else
		{
			throw std::exception("Bounds of child quads have not been correctly initialized");
		}
	}
}

// Create four child trees from this one, placing the point within the appropriate one
void QuadTree::Subdivide()
{
	// std::cout << "Subdivide!" << std::endl;

	if (!IsLeaf())
	{
		throw std::exception("Cannot subdivide a non-leaf node since it has already been subdivided");
	}

	// Create children
	auto center = bounds.GetCenter();
	topLeft = new QuadTree(Rect(bounds.lx, center.y, center.x, bounds.hy), level + 1);
	topRight = new QuadTree(Rect(center.x, center.y, bounds.hx, bounds.hy), level + 1);
	bottomLeft = new QuadTree(Rect(bounds.lx, bounds.ly, center.x, center.y), level + 1);
	bottomRight = new QuadTree(Rect(center.x, bounds.ly, bounds.hx, center.y), level + 1);

	// Remove all points and place them within the appropriate children
	while (!points.empty())
	{
		auto point = points.back();

		if (topLeft->CanContain(*point))
		{
			topLeft->InsertPoint(point);
		}
		else if (topRight->CanContain(*point))
		{
			topRight->InsertPoint(point);
		}
		else if (bottomLeft->CanContain(*point))
		{
			bottomLeft->InsertPoint(point);
		}
		else if (bottomRight->CanContain(*point))
		{
			bottomRight->InsertPoint(point);
		}
		else
		{
			throw std::exception("Failed to subdivide");
		}

		points.pop_back();
	}
}

// Return all points in the quad tree located within the specified bounds
std::vector<Point*> QuadTree::Query(const Rect& rect) const
{
	auto ret = std::vector<Point*>();

	if (rect.Intersects(bounds))
	{
		if (IsLeaf())
		{
			if (rect.Contains(bounds))
			{
				// This entire rectangle is inside the query rect. No need to individually check points.
				return points;
				/* for (auto i = points.begin(); i != points.end(); ++i)
				{
					ret.push_back(*i);
				} */
			}
			else
			{
				for (auto i = points.begin(); i != points.end(); ++i)
				{
					if (rect.Contains(**i))
					{
						ret.push_back(*i);
					}
				}
			}
		}
		else
		{
			// This is a subdivided quad. Gather points from children
			auto topLeftPoints = topLeft->Query(rect);
			auto topRightPoints = topRight->Query(rect);
			auto bottomLeftPoints = bottomLeft->Query(rect);
			auto bottomRightPoints = bottomRight->Query(rect);

			ret.reserve(topLeftPoints.size() + topRightPoints.size()
				+ bottomLeftPoints.size() + bottomRightPoints.size());

			ret.insert(ret.end(), topLeftPoints.begin(), topLeftPoints.end());
			ret.insert(ret.end(), topRightPoints.begin(), topRightPoints.end());
			ret.insert(ret.end(), bottomLeftPoints.begin(), bottomLeftPoints.end());
			ret.insert(ret.end(), bottomRightPoints.begin(), bottomRightPoints.end());
		}
	}

	return ret;
}