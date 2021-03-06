// QuadTreeTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "QuadTree.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <forward_list>
#include <algorithm>

int main()
{
	const int x_max = 10000;
	const int y_max = 10000;
	const int num_points = 1000000;
	const int num_queries = 1000;

	int a;
	Rect bounds;

	std::srand(std::time(nullptr));

	std::vector<Point> points;
	std::cout << "Generating points...";
	int rank = 0;
	for (auto i = 0; i < num_points; ++i)
	{
		auto point = Point((float)(std::rand() % x_max), (float)(std::rand() % y_max), rank);
		points.push_back(point);
		bounds.Expand(point);
		++rank;
	}

	auto root = new QuadTree(bounds);

	std::cout << "...done" << std::endl;

	std::vector<Rect> queries;
	std::cout << "Generating queries...";
	for (auto i = 0; i < num_queries; ++i)
	{
		auto lx = rand() % x_max;
		auto hx = (rand() % (x_max - lx)) + lx;
		auto ly = rand() % y_max;
		auto hy = (rand() % (y_max - ly)) + ly;

		queries.push_back(Rect(lx, ly, hx, hy));
	}
	std::cout << "...done" << std::endl;

	auto t1 = std::chrono::high_resolution_clock::now();

	for (auto i = points.begin(); i != points.end(); ++i)
	{
		root->InsertPoint(&(*i));
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Loading time: "
	<< loadTime
	<< " milliseconds" << std::endl;

	int count = 20;
	Point* outPoints = new Point[count];

	std::cout << "Running queries..." << std::endl;

	t1 = std::chrono::high_resolution_clock::now();

	for (auto i = queries.begin(); i != queries.end(); ++i)
	{
		auto queryPoints = root->Query(*i);

		int32_t resCount = std::min<int32_t>((int32_t)queryPoints.size(), count);
		std::vector<Point*> smallest;
		while (smallest.size() < resCount)
		{
			auto lowest = queryPoints.begin();

			for (auto j = queryPoints.begin(); j != queryPoints.end(); ++j)
			{
				if ((*j)->rank < (*lowest)->rank)
				{
					if (smallest.empty() || (*j)->rank > smallest.back()->rank)
					{
						lowest = j;
					}
				}
			}

			smallest.push_back(*lowest);
		}

		/* std::cout << "Query: " << (*i) << ", Results: " << resCount << std::endl;
		for (auto j = smallest.begin(); j != smallest.end(); ++j)
		{
			std::cout << "\t" << (**j) << std::endl;
		} */
	}

	t2 = std::chrono::high_resolution_clock::now();

	std::cout << "...done" << std::endl;
	auto queryTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Total query time: "
		<< queryTime
		<< " milliseconds"
		<< ", avg per query: " << ((float)queryTime / (float)num_queries) << std::endl;

	std::cout << "highest level: " << root->GetDepth() << std::endl;

	std::cin >> a;

	delete root;

	std::cin >> a;

    return 0;
}

