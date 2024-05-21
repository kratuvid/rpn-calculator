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

					operand_type op_type;
					if (operand.type() == typeid(variable_t))
					{
						auto var_ptr = std::any_cast<variable_t>(&operand);
						if (!variables.contains(var_ptr->name))
						{
							std::ostringstream oss;
							oss << "Use of undefined variable '" << var_ptr->name
								<<"' at index " << operand_index
								<< " for operation '" << std::get<0>(*op) << "'";
							throw sc::exception(oss.str(), sc::error_type::eval);
						}
						op_type = operand_type::number;
					}
					else if (operand.type() == typeid(number_t))
						op_type = operand_type::number;
					else if (operand.type() == typeid(std::string))
						op_type = operand_type::string;
					else
					{
						std::ostringstream oss;
						oss << "Unknown operand type '" << operand.type().name()
							<< "'. This is a program error";
						throw std::runtime_error(oss.str());
					}

					if (need_operand_type != op_type)
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

	simple_calculator::number_t simple_calculator::resolve_variable_if(const element_t& e)
	{
		if (e.type() == typeid(variable_t))
		{
			auto var = std::any_cast<variable_t const&>(e);
			return variables[var.name];
		}
		else
		{
			auto num = std::any_cast<number_t>(e);
			return num;
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
				else if (sub[0] == '$')
				{
					if (sub.size() <= 1)
					{
						throw sc::exception("Empty variable provided", sc::error_type::expr);
					}
					else
					{
						auto var_name = sub.substr(1);
						if (!variables.contains(var_name))
						{
							std::ostringstream oss;
							oss << "No such variable '" << var_name << "' exists";
							throw sc::exception(oss.str(), sc::error_type::expr);
						}
						elem = variable_t(var_name);
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
				for (int i = (int)stack.size()-1; i >= 0; i--)
				{
					if (stack[i].type() != typeid(std::string))
						break;
					else
						stack.pop_back();
				}

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
						prompt += ">> ";

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
					std::cout << "~ ";
					op_top(this);
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
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = b + a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " + " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_subtract(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = b - a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " - " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_multiply(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = b * a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " * " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_divide(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		if (a == 0)
			throw sc::exception("Cannot divide by 0", sc::error_type::eval);
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = b / a;

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " / " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_power(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
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
			{
				std::cout << i << ": " << std::any_cast<number_t>(e);
			}
			else if (e.type() == typeid(variable_t))
			{
				auto var = std::any_cast<variable_t const&>(e);
				std::cout << i << ": $" << var.name << ": " << ins->variables[var.name];
			}
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
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << "replace " << b << " > " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(a));
	}

	void simple_calculator::op_swap(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose)
			std::cerr << ins->stack.size()+2 << "> " << "swap " << b << " <> " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(a));
		ins->stack.push_back(std::make_any<number_t>(b));
	}

	void simple_calculator::op_pop(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
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
		const auto back = ins->stack.back();

		if (back.type() == typeid(variable_t))
		{
			auto var = std::any_cast<variable_t const&>(back);
			std::cout << "$" << var.name << " = " << ins->variables[var.name];
		}
		else
		{
			auto num = std::any_cast<number_t>(back);
			std::cout << num;
		}

		std::cout << std::endl;
	}

	void simple_calculator::op_neg(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
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
+: n, n: addition
-: n, n: subtraction
*: n, n: multiplication
/: n, n: division
^: n, n: power
---
neg: n: negate the top
sin: n: sine
cos: n: cosine
floor: n: floor
ceil: n: ceiling
---
stack: show the stack
clear: empty the stack
pop: n: pop the stack
replace: n, n: replaces the top of the stack
swap: n, n: swap the last two elements
var: n, s: set a variable s with n
vars: list all variables
del: s: delete variable s
delall: delete all variables
---
file: s: read commands from file
quit: quit the REPL
---
help: show this screen)" << std::endl;
	}

	void simple_calculator::op_sin(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::sin(a);

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = sin(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_cos(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::cos(a);

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = cos(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_floor(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::floor(a);

		if (ins->verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = floor(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_ceil(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
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

		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		ins->variables[a] = b;

		if (ins->verbose)
			std::cerr << ins->stack.size() << "> $" << a << " = " << b << std::endl;
	}

	void simple_calculator::op_vars(simple_calculator* ins)
	{
		for (const auto& v : ins->variables)
		{
			std::cout << "$" << v.first << ": " << v.second << std::endl;
		}
	}

	void simple_calculator::op_del(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		const auto it = ins->variables.find(a);
		if (it == ins->variables.end())
		{
			std::ostringstream oss;
			oss << "No such variable '" << a << "' exists";
			throw sc::exception(oss.str(), sc::error_type::eval);
		}
		else
		{
			ins->variables.erase(it);
			for (size_t i=0; i < ins->stack.size(); i++)
			{
				if (ins->stack[i].type() == typeid(variable_t))
				{
					auto var = std::any_cast<variable_t const&>(ins->stack[i]);
					if (var.name == a)
					{
						ins->stack.erase(ins->stack.begin() + i);
						if (i != 0) i--;
					}
				}
			}
		}
	}

	void simple_calculator::op_delall(simple_calculator* ins)
	{
		ins->variables.clear();
		for (size_t i=0; i < ins->stack.size(); i++)
		{
			if (ins->stack[i].type() == typeid(variable_t))
			{
				ins->stack.erase(ins->stack.begin() + i);
				if (i != 0) i--;
			}
		}
	}
}; // namespace sc
