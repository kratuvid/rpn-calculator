#include <chrono>
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
		long double diff_nsecs = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_diff).count();
		long double diff_usecs = diff_nsecs / 1.0e3l;
		long double diff_msecs = diff_nsecs / 1.0e6l;
		long double diff_secs = diff_nsecs / 1.0e9l;
		std::cerr << std::scientific << "Runtime: " << diff_nsecs << "ns, "
				  << diff_usecs << "us, "
				  << diff_msecs << "ms, "
				  << diff_secs << "s"
				  << std::endl;
	}
}
