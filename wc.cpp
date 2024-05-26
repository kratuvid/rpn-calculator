#include "wc.hpp"

namespace wc
{
	wtf_calculator::wtf_calculator()
	{
		tp_begin = std::chrono::high_resolution_clock::now();
	}

	void wtf_calculator::start(int argc, char** argv)
	{
		parse_arguments(argc, argv);
	}

	wtf_calculator::~wtf_calculator()
	{
		if (is_time)
		{
			auto tp_end = std::chrono::high_resolution_clock::now();
			auto tp_diff = tp_end - tp_begin;
			auto diff_nsecs = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_diff).count();
			auto diff_usecs = std::chrono::duration_cast<std::chrono::microseconds>(tp_diff).count();
			auto diff_msecs = std::chrono::duration_cast<std::chrono::milliseconds>(tp_diff).count();
			auto diff_secs = std::chrono::duration_cast<std::chrono::seconds>(tp_diff).count();
			auto diff_mins = diff_secs / 60;
			std::cerr << std::setprecision(5) << "Runtime (truncated): "
					  << (long double)diff_nsecs << "ns, "
					  << (long double)diff_usecs << "us, "
					  << (long double)diff_msecs << "ms, "
					  << (long double)diff_secs << "s, "
					  << (long double)diff_mins << "m"
					  << std::endl;
		}
	}

	void wtf_calculator::show_help(char* name)
	{
		std::cerr << name << ": Wtf Calculator: Another RPN calculator" << std::endl
				  << "\t-h, --help: Show this" << std::endl
				  << "\t-e, --expr [EXPRESSION]: Calculates EXPRESSION" << std::endl
				  << "\t-r, --repl: Start the REPL" << std::endl
				  << "\t-f, --file [FILE]: Read expressions from FILE" << std::endl
				  << "\t-s, --stdin: Read expression from standard input until EOF" << std::endl
				  << "\t-p, --prefix: Use prefix notation" << std::endl
				  << "\t-t, --time: Show runtime" << std::endl
				  << "\t-v, --verbose: Be verbose"
				  << std::endl;
	}

	void wtf_calculator::parse_arguments(int argc, char** argv)
	{
		enum class work_type { expression, file, stdin };
		std::list<std::pair<work_type, std::string_view>> list_work;
		bool is_repl = argc == 1;

		for (int i=1; i < argc; i++)
		{
			if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				show_help(argv[0]);
				WC_EXCEPTION(init_help, "");
			}
			else if (strcmp(argv[i], "--expr") == 0 || strcmp(argv[i], "-e") == 0)
			{
				if (i+1 >= argc)
					WC_EXCEPTION(init, "Please supply an expression to calculate");

				list_work.push_back({work_type::expression, std::string_view(argv[i+1])});
				i++;
			}
			else if (strcmp(argv[i], "--repl") == 0 || strcmp(argv[i], "-r") == 0)
			{
				is_repl = true;
			}
			else if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
			{
				if (i+1 >= argc)
					WC_EXCEPTION(init, "Please supply a file to read");

				list_work.push_back({work_type::file, std::string_view(argv[i+1])});
				i++;
			}
			else if (strcmp(argv[i], "--stdin") == 0 || strcmp(argv[i], "-s") == 0)
			{
				list_work.push_back({work_type::stdin, std::string_view("")});
			}
			else if (strcmp(argv[i], "--prefix") == 0 || strcmp(argv[i], "-p") == 0)
			{
				is_prefix = true;
			}
			else if (strcmp(argv[i], "--time") == 0 || strcmp(argv[i], "-t") == 0)
			{
				is_time = true;
			}
			else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0)
			{
				verbose = true;
			}
			else
			{
				show_help(argv[0]);
				WC_EXCEPTION(init, "Unknown argument: '" << argv[i] << "'");
			}
		}

		for (const auto& [type, what] : list_work)
		{
			switch(type)
			{
			case work_type::expression:
				parse(what);
				break;
			case work_type::file:
				file(what);
				break;
			case work_type::stdin:
				file(std::cin);
				break;
			}
		}

		if (is_repl)
			repl();
	}

	void wtf_calculator::execute()
	{
		while (stack.size() > 0 && stack.back().type() == typeid(operations_iter_t))
		{
			auto op = std::any_cast<operations_iter_t&&>(std::move(stack.back()));
			stack.pop_back();

			const auto& [opr_list, op_func] = op->second;

			if (stack.size() < opr_list.size())
			{
				WC_EXCEPTION(exec, "Operation '" << op->first << "' requires "
							 << opr_list.size() << " elements but only "
							 << stack.size() << " are left");
			}
			else
			{
				for (unsigned i=0; i < opr_list.size(); i++)
				{
					const auto& opr = stack[stack.size() - i - 1];
					const auto opr_index = opr_list.size() - i - 1;
					const auto need_opr_type = opr_list[opr_index];

					operand_type opr_type;
					if (opr.type() == typeid(number_t))
						opr_type = operand_type::number;
					else if (opr.type() == typeid(std::string))
						opr_type = operand_type::string;
					else
					{
						WC_STD_EXCEPTION("Unknown operand type '" << opr.type().name() << "' "
										 << "encountered while executing operation '" << op->first
										 << "'. This is a program error");
					}

					if (need_opr_type != opr_type)
					{
						WC_EXCEPTION(exec, "Expected an operand of type ";
									 if (need_opr_type == operand_type::string) oss << "string";
									 else if (need_opr_type == operand_type::number) oss << "number";
									 else oss << "unknown";
									 oss << " at index " << opr_index << " for operation '" << op->first << "'");
					}
				}

				op_func(this);
			}
		}
	}

	void wtf_calculator::ensure_pop_locals()
	{
		const auto it_pop_locals = operations.find("_pop_locals"),
			it_push_locals = operations.find("_push_locals");
		std::list<std::string> names;

		if (variables_local.size() > 0)
		{
			std::list<stack_t::iterator> it_names;
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

	void wtf_calculator::evaluate()
	{
		try
		{
			while (secondary_stack.size() > 0)
			{
				auto elem = std::move(secondary_stack.front());
				secondary_stack.pop_front();

				if (current_eval_function.empty() && current_eval_times.empty())
				{
					if (elem.type() == typeid(variable_ref_t))
					{
						auto var = std::any_cast<variable_ref_t const&>(elem);

						number_t out;
						if (!dereference_variable(var, out))
							WC_EXCEPTION(eval, "No such variable '" << var.name << "' exists in relevant scopes");

						elem = out;
					}
					else if (elem.type() == typeid(function_ref_t))
					{
						auto func = std::any_cast<function_ref_t const&>(elem);

						const auto it_func = functions.find(func.name);
						if (it_func == functions.end())
						{
							WC_EXCEPTION(eval, "No such function '" << func.name << "' exists");
						}
						else
						{
							const auto& [opr_count, func_stack] = it_func->second;

							if (stack.size() < opr_count)
							{
								WC_EXCEPTION(eval, "Function '" << func.name << "' requires "
											 << opr_count << " elements but only "
											 << stack.size() << " are left");
							}
							else
							{
								for (size_t i = 0; i < opr_count; i++)
								{
									const auto& opr = stack[stack.size() - i - 1];
									const auto opr_index = opr_count - i - 1;

									if (opr.type() != typeid(number_t) &&
										opr.type() != typeid(variable_ref_t))
									{
										WC_EXCEPTION(eval, "Expected operand of type number or variable"
													 << " at index " << opr_index << " for function '"
													 << func.name << "'");
									}
								}
							}

							secondary_stack.push_front(operations.find("_pop_locals"));
							secondary_stack.push_front(func.name);
							for (auto it = func_stack.rbegin(); it != func_stack.rend(); it++)
							{
								secondary_stack.push_front(*it);
							}
							secondary_stack.push_front(operations.find("_push_locals"));
							secondary_stack.push_front(func.name);
							secondary_stack.push_front(static_cast<number_t>(scope_type::function));
						}
						continue;
					}
				}

				const bool is_op = elem.type() == typeid(operations_iter_t);
				bool is_only_stack = false;

				if (is_op)
				{
					const auto& name = std::any_cast<operations_iter_t const&>(elem)->first;
					for (const auto& what : {"defun", "end", "times", "end-times"})
						if (name == what)
						{
							is_only_stack = true;
							break;
						}
				}

				if (!current_eval_times.empty() && !is_only_stack)
				{
					auto& times_stack = times[current_eval_times.back()];
					times_stack.push_back(std::move(elem));
				}
				else if (!current_eval_function.empty() && !is_only_stack)
				{
					auto& func_stack = std::get<1>(functions[current_eval_function]);
					func_stack.push_back(std::move(elem));
				}
				else
				{
					stack.push_back(std::move(elem));
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

	bool wtf_calculator::dereference_variable(const wtf_calculator::variable_ref_t& what, number_t& out)
	{
		bool found = false;

		for (auto it = variables_local.crbegin(); it != variables_local.crend(); it++)
		{
			const auto& [scope, locals] = *it;

			auto it_local = locals.find(what.name);
			if (it_local != locals.end())
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

	wtf_calculator::number_t wtf_calculator::resolve_variable_if(const element_t& e)
	{
		if (e.type() == typeid(variable_ref_t))
		{
			WC_STD_EXCEPTION("Variables shouldn't be on the stack. This is a removed feature");
			// auto var = std::any_cast<variable_ref_t const&>(e);
			// number_t out;
			// if (!dereference_variable(var, out))
			// {
			// 	std::ostringstream oss;
			// 	oss << "No such variable '" << var.name << "' exists in relevant scopes";
			// 	throw wc::exception(oss.str(), wc::error_type::exec);
			// }
			// return out;
		}
		else
		{
			auto num = std::any_cast<number_t>(e);
			return num;
		}
	}

	void wtf_calculator::parse(std::string_view what)
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

				if (isspace(c) || c == '~')
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

		if (is_prefix)
		{
			std::reverse(subs.begin(), subs.end());
		}

		for (auto it = subs.begin(); it != subs.end(); it++)
		{
			static std::list<unsigned> parse_times;
			static unsigned parse_times_index = 0;
			if (*it == "times")
			{
				parse_times.push_back(parse_times_index++);
			}
			else if (*it == "end-times")
			{
				if (parse_times.empty())
					WC_EXCEPTION(parse, "Unexpected operation 'end-times'");
				subs.insert(std::next(it), "_use_times");
				subs.insert(std::next(it), std::to_string(number_t(parse_times.back())));
				parse_times.pop_back();
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
						WC_EXCEPTION(parse, "Empty string provided");
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
						WC_EXCEPTION(parse, "Empty variable provided");
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
						WC_EXCEPTION(parse, "Empty function provided");
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
				WC_EXCEPTION(parse, "Garbage sub-parseession: '" << sub << "'");
			}
		}

		evaluate();
	}

	void wtf_calculator::file(std::string_view what)
	{
		std::ifstream ifs(what.data());
		if (ifs.is_open())
		{
			file(ifs);
		}
		else
		{
			WC_EXCEPTION(file, "Cannot open file '" << what << "'");
		}
	}

	void wtf_calculator::file(std::istream& is)
	{
		std::string line;
		while (std::getline(is, line))
		{
			parse(line);
		}
	}

	void wtf_calculator::repl()
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
						WC_EXCEPTION(repl_quit, "");

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
						if (!what) WC_EXCEPTION(repl_quit, "");

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
		catch (const wc::exception& e)
		{
			switch (e.type)
			{
			case wc::error_type::parse:
			case wc::error_type::eval:
			case wc::error_type::exec:
			case wc::error_type::file:
				break;
			case wc::error_type::repl_quit:
				cleanup_global();
				return;
			default:
				cleanup_global();
				throw;
			}

			std::cerr << "Error: " << wc::error_type_str[static_cast<int>(e.type)]
					  << ": " << e.what() << std::endl;

			goto begin;
		}
	}

	void wtf_calculator::display_stack(const stack_t& what_stack)
	{
		for (const auto& elem : what_stack)
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
		if (!what_stack.empty())
			std::cout << std::endl;
	}
}; // namespace wc
