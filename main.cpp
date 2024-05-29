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
		std::uniform_int_distribution<int> dist(0, 1000);

		// wc::arbit n100("109.8442");
		const auto begin = max_i - 2000;
		std::println("Begin: {}", begin);

		wc::arbit n0({begin, begin - 10000}, {});
		for (int i=0; i < 10; i++)
		{
			auto by = dist(engine);
			wc::arbit n1({by, begin / 4}, {});
			n0 += n1;
			std::print("+= {}: ", by); n0.print();
			std::print(" <> ");
			n0.raw_print(true);
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
