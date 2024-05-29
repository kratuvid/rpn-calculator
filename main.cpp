#include "wc.hpp"
#include "arbit.hpp"

#include <random>

int main(int argc, char** argv)
{
	try
	{
		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution dist(-100, 100);

		// wc::arbit n100("109.8442");
		wc::arbit n0(0, 10);
		for (int i=0; i < 10; i++)
		{
			auto by = dist(engine);
			n0 += by;
			std::print("\n+= {}:\n", by);
			n0.raw_print();
			n0.negate(); n0.raw_print();
			n0.negate(); n0.raw_print();
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
