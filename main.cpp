#include <chrono>
#include <iomanip>

#include "sc.hpp"

int main(int argc, char** argv)
{
	auto tp_begin = std::chrono::high_resolution_clock::now();
	bool want_runtime = false;
	for (int i=1; i < argc; i++)
		if (strcmp("-t", argv[i])==0 || strcmp("--time", argv[i])==0)
			want_runtime=true;

	try
	{
		sc::simple_calculator sc(argc, argv);
	}
	catch (const sc::exception& e)
	{
		if (e.type == sc::error_type::init_help)
			return 0;

		std::cerr << "Fatal exception: "
				  << sc::error_type_str[static_cast<int>(e.type)] << ": "
				  << e.what() << std::endl;
		return 2;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Fatal standard exception";
		if (e.what()[0] != '\0')
			std::cerr << ": " << e.what();
		std::cerr << std::endl;
		return 1;
	}

	if (want_runtime)
	{
		auto tp_end = std::chrono::high_resolution_clock::now();
		auto tp_diff = tp_end - tp_begin;
		auto diff_nsecs = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_diff).count();
		auto diff_usecs = std::chrono::duration_cast<std::chrono::microseconds>(tp_diff).count();
		auto diff_msecs = std::chrono::duration_cast<std::chrono::milliseconds>(tp_diff).count();
		auto diff_secs = std::chrono::duration_cast<std::chrono::seconds>(tp_diff).count();
		auto diff_mins = diff_secs / 60;
		std::cerr << std::setprecision(5) << "Runtime (truncated): "
				  << (long double)diff_nsecs << "ns, "
				  << (long double)diff_usecs << "us, "
				  << (long double)diff_msecs << "ms, "
				  << (long double)diff_secs << "s, "
				  << (long double)diff_mins << "m"
				  << std::endl;
	}
}
