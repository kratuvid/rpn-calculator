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
		std::list<std::pair<std::string, bool>> list_work; // true if an expression
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
					list_work.push_back(std::make_pair(std::move(match[1].str()), true));
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
					list_work.push_back(std::make_pair(std::move(match[1].str()), false));
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

		for (const auto& what : list_work)
		{
			if (what.second)
				parse(what.first);
			else
				file(what.first);
		}

		if (is_stdin)
			file(std::cin);

		if (is_repl)
			repl();
	}

	void simple_calculator::execute()
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
				throw sc::exception(oss.str(), sc::error_type::exec);
			}
			else
			{
				for (unsigned i = 0; i < std::get<1>(*op).size(); i++)
				{
					const auto& operand = stack[stack.size() - i - 1];
					const auto operand_index = std::get<1>(*op).size() - i - 1;
					const auto need_operand_type = std::get<1>(*op)[operand_index];

					operand_type op_type;
					if (operand.type() == typeid(variable_ref_t) || operand.type() == typeid(number_t))
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
						throw sc::exception(oss.str(), sc::error_type::exec);
					}
				}

				std::get<2>(*op)(this);
			}
		}
	}

	void simple_calculator::evaluate()
	{
		while (secondary_stack.size() > 0)
		{
			auto elem = std::move(secondary_stack.front());
			secondary_stack.pop_front();

			if (elem.type() == typeid(variable_ref_t))
			{
				auto var = std::any_cast<variable_ref_t const&>(elem);
				auto it = variables.find(var.name);
				if (it == variables.end())
				{
					std::ostringstream oss;
					oss << "No such variable '" << var.name << "' exists";
					throw sc::exception(oss.str(), sc::error_type::eval);
				}

				elem = it->second;
			}
			else if (elem.type() == typeid(function_ref_t))
			{
				auto func = std::any_cast<function_ref_t const&>(elem);
				auto it = functions.find(func.name);
				if (it == functions.end())
				{
					std::ostringstream oss;
					oss << "No such function '" << func.name << "' exists";
					throw sc::exception(oss.str(), sc::error_type::eval);
				}
				else
				{
					if (stack.size() < std::get<0>(it->second))
					{
						std::ostringstream oss;
						oss << "Function '" << func.name << "' requires "
							<< std::get<0>(it->second) << " elements but only "
							<< stack.size() << " are left";
						throw sc::exception(oss.str(), sc::error_type::eval);
					}
					else
					{
						for (size_t i = 0; i < std::get<0>(it->second); i++)
						{
							const auto& operand = stack[stack.size() - i - 1];
							const auto operand_index = std::get<0>(it->second) - i - 1;

							if (operand.type() != typeid(number_t) &&
								operand.type() != typeid(variable_ref_t))
							{
								std::ostringstream oss;
								oss << "Expected operand of type number or variable"
									<< " at index " << operand_index << " for function '"
									<< func.name << "'";
								throw sc::exception(oss.str(), sc::error_type::eval);
							}
						}
					}

					const auto& func_stack = std::get<1>(it->second);
					for (auto it2 = func_stack.rbegin(); it2 != func_stack.rend(); it2++)
					{
						secondary_stack.push_front(*it2);
					}
				}
				continue;
			}

			const bool is_op = elem.type() == typeid(const operation_t*);
			bool is_op_end = false;
			if (is_op)
			{
				const auto& op_name = std::get<0>(*std::any_cast<const operation_t*>(elem));
				is_op_end = op_name == "end";
			}

			if (current_function && !is_op_end)
			{
				auto& func_stack = std::get<1>(*current_function);
				func_stack.push_back(std::move(elem));
			}
			else
			{
				stack.push_back(std::move(elem));
				if (is_op)
				{
					execute();
				}
			}
		}
	}

	simple_calculator::number_t simple_calculator::resolve_variable_if(const element_t& e)
	{
		if (e.type() == typeid(variable_ref_t))
		{
			throw std::runtime_error("Variables shouldn't be on the stack. This is a removed feature");
			// auto var = std::any_cast<variable_ref_t const&>(e);
			// return variables[var.name];
		}
		else
		{
			auto num = std::any_cast<number_t>(e);
			return num;
		}
	}

	void simple_calculator::parse(std::string_view what)
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
						throw sc::exception("Empty string provided", sc::error_type::parse);
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
						throw sc::exception("Empty variable provided", sc::error_type::parse);
					}
					else
					{
						elem = variable_ref_t(std::move(sub.substr(1)));
					}
				}
				else if (sub[0] == '@')
				{
					if (sub.size() <= 1)
					{
						throw sc::exception("Empty function provided", sc::error_type::parse);
					}
					else
					{
						elem = function_ref_t(std::move(sub.substr(1)));
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
				secondary_stack.push_back(std::move(elem));
			}
			else
			{
				std::ostringstream oss;
				oss << "Garbage sub-parseession: '" << sub << "'";
				throw sc::exception(oss.str(), sc::error_type::parse);
			}
		}

		evaluate();
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
			parse(line);
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

					parse(what);
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
							parse(what);
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
					std::cout << "^";
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
			case sc::error_type::parse:
			case sc::error_type::eval:
			case sc::error_type::exec:
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
}; // namespace sc
