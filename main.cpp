#include "wc.hpp"
#include "arbit.hpp"

#include <random>

int main(int argc, char** argv)
{
	try
	{
		[[maybe_unused]] const auto max_i = std::numeric_limits<int>::max(), min_i = std::numeric_limits<int>::min();
		[[maybe_unused]] const auto max_ll = std::numeric_limits<long long>::max();
		[[maybe_unused]] const auto max_u = std::numeric_limits<unsigned>::max();

		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<int> dist0(min_i, max_i), dist_loop(1, 1), dist1(max_i/4, max_i/2);

		const char print_way = 'x';

		for (int i=0; i < 1000; i++)
		{
			std::list<int> s0, s1;
			s0.push_back(dist0(engine));
			for (int j=0; j < dist_loop(engine); j++)
			{
				s1.push_back(dist1(engine));
			}
			wc::arbit n0(s0, {});
			wc::arbit n1(s1, {});

			n0.raw_print(print_way);
			std::print(" * ");
			n1.raw_print(print_way);

			const auto nr = n0 * n1;
			std::print(" = ");
			nr.raw_print(print_way, 1);
		}
	}
	catch (wc::arbit::exception& e)
	{
		std::println("Fatal arbit exception: {}: {}",
					 wc::arbit::error_type_str[static_cast<int>(e.type)],
					 e.what());
	}
	catch (std::exception& e)
	{
		std::println("Fatal standard exception: {}", e.what());
	}

	std::println("Arbit heap stats: Max: {}B on {} entries, current: {}B. "
				 "Mallocs: {}, reallocs: {}, frees: {}",
				 wc::arbit::max_heap(), wc::arbit::max_entries_heap(), wc::arbit::net_heap(),
				 wc::arbit::mallocs_heap(), wc::arbit::reallocs_heap(), wc::arbit::frees_heap());

	return 10;

	wc::wtf_calculator app;
	try
	{
		app.start(argc, argv);
	}
	catch (const wc::exception& e)
	{
		if (e.type == wc::error_type::init_help)
			return 0;

		std::println(stderr, "Fatal exception: {}: {}",
					 wc::error_type_str[static_cast<int>(e.type)], e.what());
		return 2;
	}
	catch (const std::exception& e)
	{
		std::println(stderr, "Fatal standard exception: {}", e.what());
		return 1;
	}
}
