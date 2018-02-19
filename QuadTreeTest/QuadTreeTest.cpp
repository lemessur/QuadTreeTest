// QuadTreeTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "QuadTree.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>

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
	for (auto i = 0; i < num_points; ++i)
	{
		points.push_back(Point(std::rand() % x_max, std::rand() % y_max));

		// std::cout << (std::rand() % 100) << std::endl;
		// root->InsertPoint(Point(std::rand() % 100, std::rand() % 100));
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
		root->InsertPoint(*i);
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	auto loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Loading time: "
	<< loadTime
	<< " milliseconds" << std::endl;

	t1 = std::chrono::high_resolution_clock::now();

	std::cout << "Running queries..." << std::endl;

	for (auto i = queries.begin(); i != queries.end(); ++i)
	{
		root->Query(*i);

		// auto res = root->Query(*i);

		// std::cout << "Query: " << (*i) << ", Results: " << res.size() << std::endl;
		/* for (auto j = res.begin(); j != res.end(); ++j)
		{
			std::cout << "\t" << (*j) << std::endl;
		} */
	}

	std::cout << "...done" << std::endl;

	t2 = std::chrono::high_resolution_clock::now();
	auto queryTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cout << "Total query time: "
		<< queryTime
		<< " milliseconds"
		<< ", avg per query: " << ((float)queryTime / (float)num_queries) << std::endl;

	std::cin >> a;

    return 0;
}

