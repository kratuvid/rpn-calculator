#include "wc.hpp"

int main(int argc, char** argv)
{
	wc::wtf_calculator app;
	try
	{
		app.start(argc, argv);
	}
	catch (const wc::exception& e)
	{
		if (e.type == wc::error_type::init_help)
			return 0;

		std::cerr << "Fatal exception: "
				  << wc::error_type_str[static_cast<int>(e.type)] << ": "
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
