#include "wc.hpp"
#include "arbit.hpp"

#include <random>

int main(int argc, char** argv)
{
	try
	{
		const auto max_i = std::numeric_limits<int>::max();
		const auto max_ll = std::numeric_limits<long long>::max();
		const auto max_u = std::numeric_limits<unsigned>::max();

		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<int> dist(-1e4, 1e4);

		// wc::arbit n100("109.8442");
		const auto begin = 500;
		wc::arbit n0({begin, begin}, {});
		for (int i=0; i < 10; i++)
		{
			auto by = dist(engine), by2 = dist(engine);
			wc::arbit n1({by, by2}, {});

			std::print("["); n0.raw_print(true); std::print("]");
			std::print(" + ");
			std::print("["); n1.raw_print(true); std::print("]");
			n0 += n1;
			std::println("");
			// std::print(" = "); n0.print(); std::print(" <> "); n0.raw_print(true);
		}
		std::print("["); n0.raw_print(true); std::println("]");
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
