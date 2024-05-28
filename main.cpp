#include "wc.hpp"
#include "arbit.hpp"

int main(int argc, char** argv)
{
	wc::arbit n0(13, 10), n1;
	wc::arbit n2;
	n1 += n2;
	n0.raw_print();
	n1.raw_print();
	n2.raw_print();
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
