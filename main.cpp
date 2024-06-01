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
		std::uniform_int_distribution<int> dist(min_i, max_i), dist2(3, 7);

		const char print_way = 'x';

		for (int i=0; i < 5; i++)
		{
			std::list<int> s0, s1;
			for (int j=0; j < dist2(engine); j++)
			{
				s0.push_back(dist(engine));
				s1.push_back(dist(engine));
			}
			wc::arbit n0(s0, {});
			wc::arbit n1(s1, {});

			n0.raw_print(print_way);
			std::print(" + ");
			n1.raw_print(print_way);

			const auto nr = n0 + n1;
			std::print(" = ");
			nr.raw_print(print_way, 1);
		}
	}
	catch (wc::arbit::exception& e)
	{
		std::println("Fatal arbit exception: {}: {}",
					 wc::arbit::error_type_str[static_cast<int>(e.type)],
					 e.what());
		return 12;
	}
	catch (std::exception& e)
	{
		std::println("Fatal standard exception: {}", e.what());
		return 11;
	}

	const auto m = wc::arbit::max_heap_used(), n = wc::arbit::net_heap_used();
	std::println("Arbit heap used net {}, and max {} bytes", n, m);

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
