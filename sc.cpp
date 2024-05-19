#include "sc.hpp"

namespace sc
{
	void simple_calculator::show_help(char* name)
	{
		std::cerr << name << ": Arbitrary length calculator" << std::endl
				  << "\t-h, --help: Show this" << std::endl
				  << "\t-e, --expr=[EXPRESSION]: Calculates EXPRESSION and quits" << std::endl
				  << "\t-r, --repl: Start the REPL" << std::endl
				  << "\t-v, --verbose: Be verbose"
				  << std::endl;
	}

	void simple_calculator::parse_arguments(int argc, char** argv)
	{
		std::list<std::string> list_expr;
		bool is_repl = false;
			
		for (int i=1; i < argc; i++)
		{
			if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				show_help(argv[0]);
				throw sc::exception("", sc::error_type::init_help);
			}
			
			else if (strncmp(argv[i], "--expr", 2+4) == 0 || strncmp(argv[i], "-e", 1+1) == 0)
			{
				std::regex reg(R"(--expr=(.+))"), reg2(R"(-e=(.+))");
				std::cmatch match;
				if (std::regex_match(argv[i], match, reg) || std::regex_match(argv[i], match, reg2))
				{
					auto e = match[1].str();
					list_expr.push_back(std::move(e));
				}
				else
				{
					throw sc::exception("Please supply an expression to calculate", sc::error_type::init);
				}
			}

			else if (strcmp(argv[i], "--repl") == 0 || strcmp(argv[i], "-r") == 0)
			{
				is_repl = true;
			}
			
			else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0)
			{
				verbose = true;
			}
			
			else
			{
				std::ostringstream oss;
				oss << "Unknown argument: '" << argv[i] << "'";
				show_help(argv[0]);
				throw sc::exception(oss.str(), sc::error_type::init);
			}
		}

		for (const auto& what : list_expr)
			expr(what);
		
		if (is_repl || argc == 1)
			repl();
	}

	void simple_calculator::perform_operation(operator_t operation)
	{
		switch (operation)
		{
		case operator_t::add: {			
			auto a = stack.back().first.n;
			stack.pop_back();
			auto b = stack.back().first.n;
			stack.pop_back();
			auto r = b + a;
			if (verbose)
				std::cerr << r << " = " << b << " + " << a << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;
			
		case operator_t::subtract: {
			auto a = stack.back().first.n;
			stack.pop_back();
			auto b = stack.back().first.n;
			stack.pop_back();
			auto r = b - a;
			if (verbose)
				std::cerr << r << " = " << b << " - " << a << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;
			
		case operator_t::multiply: {
			auto a = stack.back().first.n;
			stack.pop_back();
			auto b = stack.back().first.n;
			stack.pop_back();
			auto r = b * a;
			if (verbose)
				std::cerr << r << " = " << b << " * " << a << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;
			
		case operator_t::divide: {
			auto a = stack.back().first.n;
			if (a == 0)
				throw sc::exception("Cannot divide by 0", sc::error_type::expr_divide_by_zero);
			stack.pop_back();
			auto b = stack.back().first.n;
			stack.pop_back();
			auto r = b / a;
			if (verbose)
				std::cerr << r << " = " << b << " / " << a << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;

		case operator_t::power: {
			auto a = stack.back().first.n;
			stack.pop_back();
			auto b = stack.back().first.n;
			stack.pop_back();
			auto r = std::pow(b, a);
			if (verbose)
				std::cerr << r << " = " << b << " ^ " << a << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;

		case operator_t::stack: {
			std::cout << stack.size()  << " elements in stack" << std::endl;
			for (unsigned i = 0; i < stack.size(); i++)
			{
				const auto& e = stack[i];
				if (e.second)
					std::cout << i << ": " << e.first.n;
				else
					throw std::logic_error("There shouldn't be operator on the stack");
				std::cout << std::endl;
			}
		}
			break;
			
		case operator_t::quit: {
			throw sc::exception("", sc::error_type::repl_quit);
		}
			break;
			
		case operator_t::replace: {
			auto a = stack.back().first.n;
			stack.pop_back();
			auto b = stack.back().first.n;
			stack.pop_back();
			if (verbose)
				std::cerr << "replace " << b << " -> " << a << std::endl;			
			stack.push_back(std::make_pair(static_cast<element_t>(a), true));
		}
			break;
			
		case operator_t::pop: {
			auto a = stack.back();
			if (verbose)
				std::cerr << "pop " << a.first.n << std::endl;			
			stack.pop_back();
		}
			break;
			
		case operator_t::clear: {
			stack.clear();
		}
			break;
			
		case operator_t::neg: {
			auto a = stack.back();
			stack.pop_back();
			a.first.n = -a.first.n;
			if (verbose)
				std::cerr << a.first.n << " = -(" << -a.first.n << ")" << std::endl;			
			stack.push_back(a);
		}
			break;
			
		case operator_t::help: {
			std::cerr << R"(operation: operand size: description:
-------------------------------------
+: 2: addition
-: 2: subtraction
*: 2: multiplication
/: 2: division
^: 2: power
---
sin: 1: sine
cos: 1: cosine
floor: 1: floor
ceil: 1: ceiling
---
stack: 0: show the stack
clear: 0: empty the stack
pop: 1: pop the stack
replace: 2: replaces the top of the stack
quit: 0: quit the REPL
---
help: 0: show this screen)" << std::endl;
		}
			break;
			
		case operator_t::sin: {
			auto a = stack.back().first.n;
			stack.pop_back();
			auto r = std::sin(a);
			if (verbose)
				std::cerr << r << " = sin(" << a << ")" << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;
			
		case operator_t::cos: {
			auto a = stack.back().first.n;
			stack.pop_back();
			auto r = std::cos(a);
			if (verbose)
				std::cerr << r << " = cos(" << a << ")" << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;
			
		case operator_t::floor: {
			auto a = stack.back().first.n;
			stack.pop_back();
			auto r = std::floor(a);
			if (verbose)
				std::cerr << r << " = floor(" << a << ")" << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;
			
		case operator_t::ceil: {
			auto a = stack.back().first.n;
			stack.pop_back();
			auto r = std::ceil(a);
			if (verbose)
				std::cerr << r << " = ceil(" << a << ")" << std::endl;
			stack.push_back(std::make_pair(static_cast<element_t>(r), true));
		}
			break;

		case operator_t::_length: // unreachable code
			break;
		}
	}

	void simple_calculator::evaluate()
	{
		size_t i = 0;
		auto og_size = stack.size();
		while (i < og_size && stack.size() > 0)
		{
			if (!stack.back().second)
			{
				auto e = stack.back();
				stack.pop_back();
			
				int op_i = static_cast<int>(e.first.o);
				if (stack.size() < operand_size[op_i])
				{
					std::ostringstream oss;
					oss << "Operation '" << operations[op_i] << "' requires "
						<< operand_size[op_i] << " elements but only "
						<< stack.size() << " are left";
					throw sc::exception(oss.str(), sc::error_type::expr);
				}
				else
				{
					for (unsigned i = 0; i < operand_size[op_i]; i++)
					{
						if (!stack[stack.size()-i-1].second)
						{
							std::ostringstream oss;
							oss << "Expected " << operand_size[op_i] << " numerical operands for"
								<< " operation '" << operations[op_i] << "' in the stack";
							throw sc::exception(oss.str(), sc::error_type::expr);
						}
					}
					
					perform_operation(e.first.o);
				}
			}

			else
			{
			}

			i++;
		}
	}

	void simple_calculator::expr(std::string_view what)
	{
		std::vector<std::string> subs;
		{
			std::string tmp;
			for (char c : what)
			{
				if (isspace(c))
				{
					if (!tmp.empty())
						subs.push_back(std::move(tmp));
				}
				else
				{
					tmp += c;
				}
			}
			if (!tmp.empty())
				subs.push_back(std::move(tmp));
		}

		bool okay = true;
		for (const auto& sub : subs)
		{
			okay = true;
			auto elem {std::make_pair(static_cast<element_t>(0), false)};
			
			elem.second = false;
			bool is_op = false;
			for (unsigned i=0; i < operations.size(); i++)
			{
				if (sub == operations[i])
				{
					elem.first.o = static_cast<operator_t>(i);
					is_op = true;
					break;
				}
			}

			if (!is_op)
			{
				try
				{
					elem.second = true;
					elem.first.n = std::stold(sub);
				}
				catch (const std::out_of_range& e) {
					okay = false;
				}
				catch (const std::invalid_argument& e) {
					okay = false;
				}
			}

			if (okay)
			{
				stack.push_back(std::move(elem));
				if (is_op) evaluate();
			}
			else
			{
				std::ostringstream oss; 
				oss << "Garbage sub-expression: '" << sub << "'";
				throw sc::exception(oss.str(), sc::error_type::expr);
			}
		}
	}
	
	void simple_calculator::repl()
	{
		using_history();
		
	  begin:
		try
		{
			while (true)
			{
				if (false)
				{					
					std::string what;

					std::cerr << "> ";
					std::getline(std::cin, what);
					
					expr(what);
				}
				else
				{
					char* what = nullptr;

					auto cleanup_local = [&]() {
						if (what)
						{
							free(what);
							what = nullptr;
						}
					};
					
					try
					{
						std::string prompt {std::to_string(stack.size())};
						prompt += "> ";
						
						what = readline(prompt.c_str());
						if (!what) throw sc::exception("", sc::error_type::repl_quit);

						if (*what)
						{
							add_history(what);
							expr(what);
						}
					}
					catch (...)
					{
						cleanup_local();
						throw;
					}
					
					cleanup_local();
				}

				if (stack.size() > 0)
				{
					std::cout << stack.back().first.n << std::endl;
				}
			}
		}
		catch (const sc::exception& e)
		{
			std::ostringstream oss;
			oss << "Error: ";
			oss << sc::error_type_str[static_cast<int>(e.type)] << ": ";
			oss << e.what();
			
			switch (e.type)
			{
			case sc::error_type::expr_divide_by_zero:
			case sc::error_type::expr:
				break;
			case sc::error_type::repl_quit:
				return;
			default:
				throw;
			}
			
			std::cerr << oss.str() << std::endl;
			
			goto begin;
		}
	}	
}; // namespace sc
