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
				  << "\t-t, --time: Profile runtime" << std::endl
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

			else if (strcmp(argv[i], "--time") == 0 || strcmp(argv[i], "-t") == 0)
			{
				// handled by main()
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
		while (stack.size() > 0)
		{
			if (stack.back().type() == typeid(operations_iter_t))
			{
				auto op = std::any_cast<operations_iter_t&&>(std::move(stack.back()));
				stack.pop_back();

				if (stack.size() < std::get<0>(op->second).size())
				{
					std::ostringstream oss;
					oss << "Operation '" << op->first << "' requires "
						<< std::get<0>(op->second).size() << " elements but only "
						<< stack.size() << " are left";
					throw sc::exception(oss.str(), sc::error_type::exec);
				}
				else
				{
					const auto& opr_list = std::get<0>(op->second);

					for (unsigned i=0; i < opr_list.size(); i++)
					{
						const auto& operand = stack[stack.size() - i - 1];
						const auto operand_index = opr_list.size() - i - 1;
						const auto need_operand_type = opr_list[operand_index];

						operand_type opr_type;
						if (operand.type() == typeid(number_t))
							opr_type = operand_type::number;
						else if (operand.type() == typeid(std::string))
							opr_type = operand_type::string;
						else
						{
							std::ostringstream oss;
							oss << "Unknown operand type '" << operand.type().name() << "' "
								<< "encountered while executing operation '" << op->first
								<< "'. This is a program error";
							throw std::runtime_error(oss.str());
						}

						if (need_operand_type != opr_type)
						{
							std::ostringstream oss;
							oss << "Expected operand of type ";
							if (need_operand_type == operand_type::string) oss << "string";
							else if (need_operand_type == operand_type::number) oss << "number";
							else oss << "unknown";
							oss << " at index " << operand_index << " for operation '" << op->first << "'";
							throw sc::exception(oss.str(), sc::error_type::exec);
						}
					}

					std::get<1>(op->second)(this);
				}
			}
			else break;
		}
	}

	void simple_calculator::ensure_pop_locals()
	{
		const auto it_pop_locals = operations.find("_pop_locals"),
			it_push_locals = operations.find("_push_locals");
		std::list<std::string> names;

		if (variables_local.size() > 0)
		{
			std::list<std::deque<element_t>::iterator> it_names;
			for (auto it = secondary_stack.begin(); it != secondary_stack.end(); it++)
			{
				if ((*it).type() == typeid(operations_iter_t))
				{
					auto it_op = std::any_cast<operations_iter_t>(*it);
					if (it_op == it_pop_locals)
					{
						it_names.push_back(it-1);
					}
					else if (it_op == it_push_locals)
					{
						break;
					}
				}
			}
			for (auto& it_name : it_names)
				names.push_back(std::any_cast<std::string&&>(std::move(*it_name)));
		}

		secondary_stack.clear();
		for (auto& name : names)
		{
			secondary_stack.push_back(std::move(name));
			secondary_stack.push_back(it_pop_locals);
			evaluate();
		}
	}

	void simple_calculator::evaluate()
	{
		try
		{
			while (secondary_stack.size() > 0)
			{
				static std::deque<element_t>* last_stack = nullptr;

				auto elem = std::move(secondary_stack.front());
				secondary_stack.pop_front();

				if (current_eval_function.empty() && current_eval_times == -1)
				{
					if (elem.type() == typeid(variable_ref_t))
					{
						auto var = std::any_cast<variable_ref_t const&>(elem);

						number_t out;
						if (!dereference_variable(var, out))
						{
							std::ostringstream oss;
							oss << "No such variable '" << var.name << "' exists in relevant scopes";
							throw sc::exception(oss.str(), sc::error_type::eval);
						}

						elem = out;
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
							std::deque<element_t>* cs_raw = &stack;
							if (current_eval_times != -1)
								cs_raw = &std::get<1>(times[current_eval_times]);
							else if (!current_eval_function.empty())
								cs_raw = &std::get<1>(functions[current_eval_function]);

							const auto& current_stack = *cs_raw;
							const auto op_count = std::get<0>(it->second);

							if (current_stack.size() < op_count)
							{
								std::ostringstream oss;
								oss << "Function '" << func.name << "' requires "
									<< op_count << " elements but only "
									<< current_stack.size() << " are left";
								throw sc::exception(oss.str(), sc::error_type::eval);
							}
							else
							{
								for (size_t i = 0; i < op_count; i++)
								{
									const auto& operand = current_stack[current_stack.size() - i - 1];
									const auto operand_index = op_count - i - 1;

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
							secondary_stack.push_front(operations.find("_pop_locals"));
							secondary_stack.push_front(func.name);
							for (auto it2 = func_stack.rbegin(); it2 != func_stack.rend(); it2++)
							{
								secondary_stack.push_front(*it2);
							}
							secondary_stack.push_front(operations.find("_push_locals"));
							secondary_stack.push_front(func.name);
							secondary_stack.push_front(static_cast<number_t>(scope_type::function));
						}
						continue;
					}
					else if (elem.type() == typeid(times_ref_t))
					{
						auto t = std::any_cast<times_ref_t const&>(elem);
						const auto& times_stack = std::get<1>(times[t.index]);

						std::string name = "times";
						for (unsigned i=0; i < std::get<0>(times[t.index]); i++)
						{
							secondary_stack.push_front(operations.find("_pop_locals"));
							secondary_stack.push_front(name);
							for (auto it = times_stack.rbegin(); it != times_stack.rend(); it++)
							{
								secondary_stack.push_front(*it);
							}
							secondary_stack.push_front(operations.find("_push_locals"));
							secondary_stack.push_front(name);
							secondary_stack.push_front(static_cast<number_t>(scope_type::loop));
						}
						continue;
					}
				}

				const bool is_op = elem.type() == typeid(operations_iter_t);
				bool is_op_end = false, is_op_end_times = false,
					is_op_defun = false, is_op_times = false;
				if (is_op)
				{
					const auto& op_name = std::any_cast<operations_iter_t>(elem)->first;
					is_op_end = op_name == "end";
					is_op_end_times = op_name == "end-times";
					is_op_defun = op_name == "defun";
					is_op_times = op_name == "times";
				}

				const bool is_only_stack = is_op_end || is_op_end_times || is_op_defun || is_op_times;

				if (current_eval_times != -1 && !is_only_stack)
				{
					auto& times_stack = std::get<1>(times.back());
					times_stack.push_back(std::move(elem));
					last_stack = &times_stack;
				}
				else if (!current_eval_function.empty() && !is_only_stack)
				{
					auto& func_stack = std::get<1>(functions[current_eval_function]);
					func_stack.push_back(std::move(elem));
					last_stack = &func_stack;
				}
				else
				{
					if (is_op_times)
					{
						if (!last_stack)
							throw sc::exception("Pointer to the last stack is null. Probably a malformed expression", sc::error_type::eval);
						if ((*last_stack).size() == 0)
							throw sc::exception("Last stack is empty", sc::error_type::eval);

						auto last_elem = std::move((*last_stack).back());
						(*last_stack).pop_back();

						if (last_elem.type() != typeid(number_t))
						{
							std::ostringstream oss;
							oss << "Can't create new times:" << times.size() << " as the last element in the last stack isn't a number_t";
							throw sc::exception(oss.str(), sc::error_type::eval);
						}

						stack.push_back(std::move(last_elem));
					}

					stack.push_back(std::move(elem));
					last_stack = &stack;

					if (is_op) execute();
				}
			}
		}
		catch(...)
		{
			ensure_pop_locals();
			throw;
		}
	}

	bool simple_calculator::dereference_variable(const simple_calculator::variable_ref_t& what, number_t& out)
	{
		bool found = false;

		for (auto it = variables_local.crbegin(); it != variables_local.crend(); it++)
		{
			auto scope = std::get<0>(*it);
			const auto& local = std::get<1>(*it);

			auto it_local = local.find(what.name);
			if (it_local != local.end())
			{
				found = true;
				out = it_local->second;
				break;
			}

			if (scope != scope_type::loop)
				break;
		}

		if (!found)
		{
			auto it_global = variables.find(what.name);
			if (it_global != variables.end())
			{
				found = true;
				out = it_global->second;
			}
		}

		return found;
	}

	simple_calculator::number_t simple_calculator::resolve_variable_if(const element_t& e)
	{
		if (e.type() == typeid(variable_ref_t))
		{
			throw std::runtime_error("Variables shouldn't be on the stack. This is a removed feature");
			// auto var = std::any_cast<variable_ref_t const&>(e);
			// number_t out;
			// if (!dereference_variable(var, out))
			// {
			// 	std::ostringstream oss;
			// 	oss << "No such variable '" << var.name << "' exists in relevant scopes";
			// 	throw sc::exception(oss.str(), sc::error_type::exec);
			// }
			// return out;
		}
		else
		{
			auto num = std::any_cast<number_t>(e);
			return num;
		}
	}

	void simple_calculator::parse(std::string_view what)
	{
		secondary_stack.clear();

		std::list<std::string> subs;
		{
			std::string tmp;
			bool is_comment = false;
			for (char c : what)
			{
				if (c == ';')
				{
					is_comment = true;
				}
				if (is_comment)
				{
					if (c == '\n')
						is_comment = false;
					else
						continue;
				}

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

		for (auto it = subs.begin(); it != subs.end(); it++)
		{
			if (*it == "end-times")
			{
				subs.insert(std::next(it), "*_use_times");
			}
		}

		for (const auto& sub : subs)
		{
			element_t elem;

			const auto it_op = operations.find(sub);
			if (it_op != operations.end())
			{
				elem = it_op;
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
				else if (sub[0] == '*')
				{
					if (sub != "*_use_times")
					{
						throw sc::exception("* is internally reserved for *_use_times and shouldn't be used on will", sc::error_type::parse);
					}
					else
					{
						elem = times_ref_t(current_times_ref_index++);
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
		auto cleanup_local = [](char*& what) {
			if (what)
			{
				free(what);
				what = nullptr;
			}
		};
		auto cleanup_global = []() {
			rl_clear_history();
		};

		using_history();

	  begin:
		try
		{
			while (true)
			{
				#ifdef SC_USE_TRADITIONAL_GETLINE
				{
					std::string what;

					std::cerr << stack.size() << ">> ";
					if (!std::getline(std::cin, what))
						throw sc::exception("", sc::error_type::repl_quit);

					if (what.size() > 0)
					{
						parse(what);
					}
				}
				#else
				{
					char* what = nullptr;

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
						cleanup_local(what);
						throw;
					}

					cleanup_local(what);
				}
				#endif

				if (stack.size() > 0)
				{
					std::cout << "^";
					op_top(this);
				}
			}
		}
		catch (const sc::exception& e)
		{
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

	void simple_calculator::display_stack(const std::deque<element_t>& that_stack)
	{
		for (const auto& elem : that_stack)
		{
			if (elem.type() == typeid(variable_ref_t))
			{
				auto var = std::any_cast<variable_ref_t const&>(elem);
				std::cout << '$' << var.name;
			}
			else if (elem.type() == typeid(function_ref_t))
			{
				auto func = std::any_cast<function_ref_t const&>(elem);
				std::cout << '@' << func.name;
			}
			else if (elem.type() == typeid(times_ref_t))
			{
				auto time = std::any_cast<times_ref_t const&>(elem);
				std::cout << "*times:" << time.index;
			}
			else if (elem.type() == typeid(std::string))
			{
				auto str = std::any_cast<std::string const&>(elem);
				std::cout << ':' << str;
			}
			else if (elem.type() == typeid(operations_iter_t))
			{
				auto op_it = std::any_cast<operations_iter_t>(elem);
				std::cout << op_it->first;
			}
			else
			{
				auto num = std::any_cast<number_t const&>(elem);
				std::cout << num;
			}
			std::cout << ' ';
		}
		if (!that_stack.empty())
			std::cout << std::endl;
	}
}; // namespace sc
