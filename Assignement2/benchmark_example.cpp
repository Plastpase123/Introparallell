#include <cstdlib>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "benchmark.hpp"

// HAS DEADLOCK
// #include "sorted_list.hpp"

#include "sorted_list_CG.hpp"
#include "sorted_list_CG_TATAS.hpp"
#include "sorted_list_FG.hpp"
#include "sorted_list_FG_TATAS.hpp"

// DOES NOT WORK
//  #include "sorted_list_FG_MCS.hpp"
//  #include "sorted_list_CG_MCS.hpp"

static const int DATA_VALUE_RANGE_MIN = 0;
static const int DATA_VALUE_RANGE_MAX = 256;
static const int DATA_PREFILL = 512;

template <typename List>
void read(List &l, int random)
{
	/* read operations: 100% count */
	l.count(random % DATA_VALUE_RANGE_MAX);
}

template <typename List>
void update(List &l, int random)
{
	/* update operations: 50% insert, 50% remove */
	auto choice = (random % (2 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
	if (choice == 0)
	{
		l.insert(random % DATA_VALUE_RANGE_MAX);
	}
	else
	{
		l.remove(random % DATA_VALUE_RANGE_MAX);
	}
}

template <typename List>
void mixed(List &l, int random)
{
	/* mixed operations: 6.25% update, 93.75% count */
	auto choice = (random % (32 * DATA_VALUE_RANGE_MAX)) / DATA_VALUE_RANGE_MAX;
	if (choice == 0)
	{
		l.insert(random % DATA_VALUE_RANGE_MAX);
	}
	else if (choice == 1)
	{
		l.remove(random % DATA_VALUE_RANGE_MAX);
	}
	else
	{
		l.count(random % DATA_VALUE_RANGE_MAX);
	}
}

int main(int argc, char *argv[])
{
	/* get number of threads from command line */
	if (argc < 2)
	{
		std::cerr << u8"Please specify number of worker threads: " << argv[0] << u8" <number>\n";
		std::exit(EXIT_FAILURE);
	}
	std::istringstream ss(argv[1]);
	int threadcnt;
	if (!(ss >> threadcnt))
	{
		std::cerr << u8"Invalid number of threads '" << argv[1] << u8"'\n";
		std::exit(EXIT_FAILURE);
	}
	/* set up random number generator */
	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_int_distribution<int> uniform_dist(DATA_VALUE_RANGE_MIN, DATA_VALUE_RANGE_MAX);

	/* {
		sorted_list<int> l1;
		//prefill list with 1024 elements
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "First benchmark\n";
		benchmark(threadcnt, u8"non-thread-safe read", [&l1](int random)
				  { read(l1, random); });
		std::cout << "Second benchmark \n";
		benchmark(threadcnt, u8"non-thread-safe update", [&l1](int random)
				  { update(l1, random); });
	}
	{
		//start with fresh list: update test left list in random size
		sorted_list<int> l1;
		//prefill list with 1024 elements
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "Third benchmark \n";
		benchmark(threadcnt, u8"non-thread-safe mixed", [&l1](int random)
				  { mixed(l1, random); });
	} */

	{
		std::cout << "Course grained\n"
				  << std::endl;
	}
	{
		sorted_list_cg<int> l1;
		/* prefill list with 1024 elements */
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "First benchmark\n";
		benchmark(threadcnt, u8"Course grained read", [&l1](int random)
				  { read(l1, random); });
		std::cout << "Second benchmark \n";
		benchmark(threadcnt, u8"Course grained update", [&l1](int random)
				  { update(l1, random); });
	}
	{
		/* start with fresh list: update test left list in random size */
		sorted_list_cg<int> l1;
		/* prefill list with 1024 elements */
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "Third benchmark \n";
		benchmark(threadcnt, u8"Course grained mixed", [&l1](int random)
				  { mixed(l1, random); });
	}
	{
		std::cout << "\nCourse grained TATAS\n"
				  << std::endl;
	}
	{
		sorted_list_cg_tatas<int> l1;
		/* prefill list with 1024 elements */
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "First benchmark\n";
		benchmark(threadcnt, u8"Course grained TATAS read", [&l1](int random)
				  { read(l1, random); });
		std::cout << "Second benchmark \n";
		benchmark(threadcnt, u8"Course grained TATAS update", [&l1](int random)
				  { update(l1, random); });
	}
	{
		/* start with fresh list: update test left list in random size */
		sorted_list_cg_tatas<int> l1;
		/* prefill list with 1024 elements */
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "Third benchmark \n";
		benchmark(threadcnt, u8"Course grained TATAS mixed", [&l1](int random)
				  { mixed(l1, random); });
	}
	{
		std::cout << "\nFine grained\n"
				  << std::endl;
	}
	{
		sorted_list_fg<int> l1;
		/* prefill list with 1024 elements */
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "First benchmark\n";
		benchmark(threadcnt, u8"Fine grained read", [&l1](int random)
				  { read(l1, random); });
		std::cout << "Second benchmark \n";
		benchmark(threadcnt, u8"Fine grained update", [&l1](int random)
				  { update(l1, random); });
	}
	{
		/* start with fresh list: update test left list in random size */
		sorted_list_fg<int> l1;
		/* prefill list with 1024 elements */
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "Third benchmark \n";
		benchmark(threadcnt, u8"Fine grained mixed", [&l1](int random)
				  { mixed(l1, random); });
	}
	{
		std::cout << "\nFine grained TATAS\n"
				  << std::endl;
	}
	{
		sorted_list_fg_tatas<int> l1;
		/* prefill list with 1024 elements */
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "First benchmark\n";
		benchmark(threadcnt, u8"Fine grained TATAS read", [&l1](int random)
				  { read(l1, random); });
		std::cout << "Second benchmark \n";
		benchmark(threadcnt, u8"Fine grained TATAS update", [&l1](int random)
				  { update(l1, random); });
	}
	{
		/* start with fresh list: update test left list in random size */
		sorted_list_fg_tatas<int> l1;
		/* prefill list with 1024 elements */
		for (int i = 0; i < DATA_PREFILL; i++)
		{
			l1.insert(uniform_dist(engine));
		}
		std::cout << "Third benchmark \n";
		benchmark(threadcnt, u8"Fine grained TATAS mixed", [&l1](int random)
				  { mixed(l1, random); });
	}

	return EXIT_SUCCESS;
}
