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
		
		std::ostringstream oss;
		oss << "Fatal exception: ";
		oss << sc::error_type_str[static_cast<int>(e.type)] << ": ";
		std::cerr << oss.str() << e.what() << std::endl;
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
