import <print>;

import wc;

int main(int argc, char** argv)
{
	why_calculator app;
	try
	{
		app.start(argc, argv);
	}
	catch (const why_calculator::exception& e)
	{
		if (e.type == why_calculator::error_type::init_help)
			return 0;

		std::println(stderr, "Fatal exception: {}: {}",
					 why_calculator::error_type_str[static_cast<int>(e.type)], e.what());
		return 2;
	}
	catch (const std::exception& e)
	{
		std::println(stderr, "Fatal standard exception: {}", e.what());
		return 1;
	}
}
