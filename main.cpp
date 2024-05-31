#include "wc.hpp"
#include "arbit.hpp"

#include <random>

int main(int argc, char** argv)
{
	try
	{
		const auto max_i = std::numeric_limits<int>::max(),
			min_i = std::numeric_limits<int>::min();
		const auto max_ll = std::numeric_limits<long long>::max();
		const auto max_u = std::numeric_limits<unsigned>::max();

		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<int> dist(min_i, 0), dist2(0, 1000);

		const char print_way = 'e';

		for (int i=0; i < 10; i++)
		{
			const auto s0 = {100}, s1 = {dist2(engine)};
			wc::arbit n0(std::string_view("-100"));
			wc::arbit n1(s1);

			n0.raw_print(print_way);
			std::print(" * ");
			n1.raw_print(print_way);

			auto ns = n0 * n1;
			std::print(" = ");
			ns.raw_print(print_way, 1);
		}

		/*
		for (int i=0; i < 0; i++)
		{
			const auto by = dist2(engine), by2 = dist(engine);

			wc::arbit n0({1073741824}, {});
			wc::arbit n1({unsigned(3)}, {});

			n0.raw_print(print_way);
			std::print(" * ");
			n1.raw_print(print_way);

			std::print(" = ");
			auto np = n0 * n1;
			np.raw_print(print_way);
			std::println("");
		}
		*/
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
