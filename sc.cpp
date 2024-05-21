#include "sc.hpp"

namespace sc
{
	void simple_calculator::show_help(char* name)
	{
		std::cerr << name << ": Arbitrary length calculator" << std::endl
				  << "\t-h, --help: Show this" << std::endl
				  << "\t-e, --expr=[EXPRESSION]: Calculates EXPRESSION" << std::endl
				  << "\t-r, --repl: Start the REPL" << std::endl
				  << "\t-f, --file=[FILE]: Read expressions from FILE" << std::endl
				  << "\t-s, --stdin: Read expression from standard input until EOF" << std::endl
				  << "\t-v, --verbose: Be verbose"
				  << std::endl;
	}

	void simple_calculator::parse_arguments(int argc, char** argv)
	{
		std::list<std::string> list_expr, list_files;
		bool is_repl = argc == 1, is_stdin = false;

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
					list_expr.push_back(std::move(match[1].str()));
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

			else if (strncmp(argv[i], "--file", 2+4) == 0 || strncmp(argv[i], "-f", 1+1) == 0)
			{
				std::regex reg(R"(--file=(.+))"), reg2(R"(-f=(.+))");
				std::cmatch match;
				if (std::regex_match(argv[i], match, reg) || std::regex_match(argv[i], match, reg2))
				{
					list_files.push_back(std::move(match[1].str()));
				}
				else
				{
					throw sc::exception("Please supply a file to read", sc::error_type::init);
				}
			}

			else if (strcmp(argv[i], "--stdin") == 0 || strcmp(argv[i], "-s") == 0)
			{
				is_stdin = true;
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

		for (const auto& what : list_files)
			file(what);

		if (is_stdin)
			file(std::cin);

		if (is_repl)
			repl();
	}

	void simple_calculator::evaluate()
	{
		while (stack.size() > 0 && stack.back().type() == typeid(const operation_t*))
		{
			auto op = std::any_cast<const operation_t*>(stack.back());
			stack.pop_back();

			if (stack.size() < std::get<1>(*op).size())
			{
				std::ostringstream oss;
				oss << "Operation '" << std::get<0>(*op) << "' requires "
					<< std::get<1>(*op).size() << " elements but only "
					<< stack.size() << " are left";
				throw sc::exception(oss.str(), sc::error_type::eval);
			}
			else
			{
				for (unsigned i = 0; i < std::get<1>(*op).size(); i++)
				{
					const auto& operand = stack[stack.size() - i - 1];
					const auto operand_index = std::get<1>(*op).size() - i - 1;
					const auto need_operand_type = std::get<1>(*op)[operand_index];

					bool matched = false;
					switch (need_operand_type)
					{
					case operand_type::string:
						if (operand.type() != typeid(std::string))
							matched = true;
						break;

					case operand_type::number:
						if (operand.type() != typeid(number_t))
							matched = true;
						break;
					}
					if (matched)
					{
						std::ostringstream oss;
						oss << "Expected operand of type ";
						if (need_operand_type == operand_type::string) oss << "string";
						else if (need_operand_type == operand_type::number) oss << "number";
						else oss << "unknown";
						oss << " at index " << operand_index << " for operation '" << std::get<0>(*op) << "'";
						throw sc::exception(oss.str(), sc::error_type::eval);
					}
				}

				std::get<2>(*op)(this);
			}
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

		for (const auto& sub : subs)
		{
			element_t elem;

			for (size_t i=0; i < operations.size(); i++)
			{
				if (sub == std::get<0>(operations[i]))
				{
					elem = &operations[i];
					break;
				}
			}

			if (!elem.has_value())
			{
				if (sub[0] == ':')
				{
					if (sub.size() <= 1)
					{
						throw sc::exception("Empty string argument provided", sc::error_type::expr);
					}
					else
					{
						elem = std::move(sub.substr(1));
					}
				}

				if (!elem.has_value())
				{
					try
					{
						elem = std::stold(sub);
					}
					catch (const std::out_of_range&) {}
					catch (const std::invalid_argument&) {}
				}
			}

			if (elem.has_value())
			{
				bool is_op = elem.type() == typeid(const operation_t*);
				stack.push_back(std::move(elem));
				if (is_op)
				{
					evaluate();
				}
			}
			else
			{
				std::ostringstream oss;
				oss << "Garbage sub-expression: '" << sub << "'";
				throw sc::exception(oss.str(), sc::error_type::expr);
			}
		}
	}

	void simple_calculator::file(std::string_view what)
	{
		std::ifstream ifs(what.data());
		if (ifs.is_open())
		{
			file(ifs);
		}
		else
		{
			std::ostringstream oss;
			oss << "Cannot open file '" << what << "'";
			throw sc::exception(oss.str(), sc::error_type::file);
		}
	}

	void simple_calculator::file(std::istream& is)
	{
		std::string line;
		while (std::getline(is, line))
		{
			expr(line);
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
					std::cout << std::any_cast<number_t>(stack.back()) << std::endl;
				}
			}
		}
		catch (const sc::exception& e)
		{
			auto cleanup_global = [&]() {
				rl_clear_history();
			};

			switch (e.type)
			{
			case sc::error_type::expr:
			case sc::error_type::eval:
			case sc::error_type::file:
				break;
			case sc::error_type::repl_quit:
				cleanup_global();
				return;
			default:
				cleanup_global();
				throw;
			}

			std::ostringstream oss;
			oss << "Error: ";
			oss << sc::error_type_str[static_cast<int>(e.type)] << ": ";
			oss << e.what();
			std::cerr << oss.str() << std::endl;

			goto begin;
		}
	}

	void simple_calculator::op_add(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();
		auto b = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = b + a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " + " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_subtract(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();
		auto b = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = b - a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " - " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_multiply(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();
		auto b = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = b * a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " * " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_divide(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		if (a == 0)
			throw sc::exception("Cannot divide by 0", sc::error_type::eval);
		ins->stack.pop_back();
		auto b = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = b / a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " / " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_power(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();
		auto b = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::pow(b, a);

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " ^ " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_stack(simple_calculator* ins)
	{
		for (unsigned i = 0; i < ins->stack.size(); i++)
		{
			const auto& e = ins->stack[i];
			if (e.type() == typeid(number_t))
				std::cout << i << ": " << std::any_cast<number_t>(e);
			else
				throw std::logic_error("There shouldn't be operator or string on the stack");
			std::cout << std::endl;
		}
	}

	void simple_calculator::op_quit(simple_calculator* ins)
	{
		throw sc::exception("", sc::error_type::repl_quit);
	}

	void simple_calculator::op_replace(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();
		auto b = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << "replace " << b << " > " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(a));
	}

	void simple_calculator::op_swap(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();
		auto b = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose)
			std::cerr << ins->stack.size()+2 << "> " << "swap " << b << " <> " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(a));
		ins->stack.push_back(std::make_any<number_t>(b));
	}

	void simple_calculator::op_pop(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose)
			std::cerr << ins->stack.size() << "> " << "pop " << a << std::endl;
	}

	void simple_calculator::op_clear(simple_calculator* ins)
	{
		ins->stack.clear();
	}

	void simple_calculator::op_file(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();
		ins->file(a);
	}

	void simple_calculator::op_top(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t&>(ins->stack.back());
		std::cout << a << std::endl;
	}

	void simple_calculator::op_neg(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = -a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = -(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_help(simple_calculator* ins)
	{
		std::cerr << R"(operation: operand size: description:
-------------------------------------
+: 2: addition
-: 2: subtraction
*: 2: multiplication
/: 2: division
^: 2: power
---
neg: 1: negate the top
sin: 1: sine
cos: 1: cosine
floor: 1: floor
ceil: 1: ceiling
---
file: -1: read commands from file
stack: 0: show the stack
clear: 0: empty the stack
pop: 1: pop the stack
replace: 2: replaces the top of the stack
swap: 2: swap the last two elements
quit: 0: quit the REPL
---
help: 0: show this screen)" << std::endl;
	}

	void simple_calculator::op_sin(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::sin(a);

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = sin(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_cos(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::cos(a);

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = cos(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_floor(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::floor(a);

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = floor(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_ceil(simple_calculator* ins)
	{
		auto a = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::ceil(a);

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = ceil(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_var(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		auto b = std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		ins->variables[a] = b;

		if (ins->verbose)
			std::cerr << ins->stack.size() << "> $" << a << " = " << b << std::endl;
	}

	void simple_calculator::op_vars(simple_calculator* ins)
	{
		for (const auto& v : ins->variables)
		{
			std::cout << "$" << v.first << " = " << v.second << std::endl;
		}
	}
}; // namespace sc
