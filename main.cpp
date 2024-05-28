#include "wc.hpp"
#include "arbit.hpp"

int main(int argc, char** argv)
{
	try
	{
		wc::arbit n100("109.8442");
		wc::arbit n0(~wc::arbit::base_t(0) - 450, 10), n1;
		wc::arbit n2;
		n0 += 500u;
		n1 += n2;
		n0.raw_print();
		n1.raw_print();
		n2.raw_print();
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
