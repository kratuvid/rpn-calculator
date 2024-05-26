#include "sc.hpp"

int main(int argc, char** argv)
{
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
}
