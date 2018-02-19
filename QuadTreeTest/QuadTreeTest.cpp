// QuadTreeTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "QuadTree.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <queue>

int main()
{
	const int x_max = 10000;
	const int y_max = 10000;
	const int num_points = 1000000;
	const int num_queries = 1000;

	auto root = new QuadTree(Rect(0, 0, x_max, y_max), 0);

	int a;

	std::srand(std::time(nullptr));

	std::vector<Point> points;
	std::cout << "Generating points..." << std::endl;
	int rank = 0;
	for (auto i = 0; i < num_points; ++i)
	{
		points.push_back(Point((float)(std::rand() % x_max), (float)(std::rand() % y_max), rank));
		++rank;
	}

	std::cout << "...done" << std::endl;

	std::vector<Rect> queries;
	std::cout << "Generating queries..." << std::endl;
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

		// Store all points that are in the search rect
		std::priority_queue<Point*> points;

		for (auto i = queryPoints.begin(); i != queryPoints.end(); ++i)
		{
			points.push(*i);
		}

		int32_t resCount = std::min<int32_t>((int32_t)points.size(), count);
		// std::cout << "Query: " << (*i) << ", Results: " << resCount << std::endl;
		for (auto i = outPoints; i != outPoints + resCount; ++i)
		{
			(*i) = *(points.top());
			points.pop();

			// std::cout << "\t" << (*i) << std::endl;
		}
	}

	t2 = std::chrono::high_resolution_clock::now();

	std::cout << "...done" << std::endl;
	auto queryTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Total query time: "
		<< queryTime
		<< " milliseconds"
		<< ", avg per query: " << ((float)queryTime / (float)num_queries) << std::endl;

	std::cin >> a;

	delete root;

	std::cin >> a;

    return 0;
}

