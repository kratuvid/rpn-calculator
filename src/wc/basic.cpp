module wc;

why_calculator::why_calculator()
{
	tp_begin = std::chrono::high_resolution_clock::now();
}

void why_calculator::start(int argc, char** argv)
{
	parse_arguments(argc, argv);
}

why_calculator::~why_calculator()
{
#ifndef WC_USE_TRADITIONAL_GETLINE
	rl_clear_history();
#endif

	if (is_time)
	{
		auto tp_end = std::chrono::high_resolution_clock::now();
		auto tp_diff = tp_end - tp_begin;
		auto diff_nsecs = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_diff);
		auto diff_usecs = std::chrono::duration_cast<std::chrono::microseconds>(tp_diff);
		auto diff_msecs = std::chrono::duration_cast<std::chrono::milliseconds>(tp_diff);
		auto diff_secs = std::chrono::duration_cast<std::chrono::seconds>(tp_diff);
		auto diff_mins = std::chrono::duration_cast<std::chrono::minutes>(tp_diff);
		std::println(stderr, "Runtime (truncated): {}, {}, {}, {}, {}",
					 diff_nsecs, diff_usecs, diff_msecs, diff_secs, diff_mins);
	}
}

void why_calculator::show_help(char* name)
{
	std::println(stderr, "{}: Why Calculator: Yet another RPN calculator\n"
				 "\t-h, --help: Show this\n"
				 "\t-e, --expr [EXPRESSION]: Calculates EXPRESSION\n"
				 "\t-f, --file [FILE]: Read expressions from FILE\n"
				 "\t-s, --stdin: Read expression from standard input until EOF\n"
				 "\t-r, --repl: Start the REPL\n"
				 "\t-p, --prefix: Use prefix notation\n"
				 "\t-t, --time: Show runtime\n"
				 "\t-v, --verbose: Be verbose", name);
}

void why_calculator::parse_arguments(int argc, char** argv)
{
	enum class work_type { expression, file, stdin };
	struct _parsed_t {
		std::list<std::pair<work_type, std::string_view>> work;
		bool is_repl;

		bool *const is_time_ptr, *const is_prefix_ptr, *const is_verbose_ptr;
		char **argv;

		_parsed_t(why_calculator* ins, int argc, char** argv)
			:is_repl(argc == 1),
			 is_time_ptr(&ins->is_time), is_prefix_ptr(&ins->is_prefix),
			 is_verbose_ptr(&ins->verbose), argv(argv)
		{}
	} parsed(this, argc, argv);

	const std::array<std::tuple<std::string_view, int, void(*)(_parsed_t&, int)>, 8> arguments {{
			{"help", 0, [](_parsed_t& p, int i) {
				why_calculator::show_help(p.argv[0]);
				WC_EXCEPTION(init_help, "");
			}},
			{"expr", 1, [](_parsed_t& p, int i) {
				p.work.push_back({work_type::expression, std::string_view(p.argv[i+1])});
			}},
			{"file", 1, [](_parsed_t& p, int i) {
				p.work.push_back({work_type::file, std::string_view(p.argv[i+1])});
			}},
			{"stdin", 0, [](_parsed_t& p, int i) {
				p.work.push_back({work_type::stdin, ""});
			}},
			{"repl", 0, [](_parsed_t& p, int i) {
				p.is_repl = true;
			}},
			{"prefix", 0, [](_parsed_t& p, int i) {
				WC_STD_EXCEPTION("--prefix is currently broken");
				*p.is_prefix_ptr = true;
			}},
			{"time", 0, [](_parsed_t& p, int i) {
				*p.is_time_ptr = true;
			}},
			{"verbose", 0, [](_parsed_t& p, int i) {
				*p.is_verbose_ptr = true;
			}}
		}
	};

	std::list<std::pair<int, int>> todo;
	for (int i=1; i < argc; i++)
	{
		const auto length = (int)std::strlen(argv[i]);

		if (length > 2 && argv[i][0] == '-' && argv[i][1] == '-')
		{
			bool none = true;
			const char* arg = &argv[i][2];

			for (int k=0; k < (int)arguments.size(); k++)
			{
				const auto& [name, ops, _func] = arguments[k];
				if (name == arg)
				{
					if (i+ops >= argc)
						WC_EXCEPTION(init, "Argument '{}' requires {} operands but only {} "
									 "are left", name, ops, argc-i-1);
					todo.push_back({k, i});
					i += ops;
					none = false;
					break;
				}
			}

			if (none)
				WC_EXCEPTION(init, "Unknown argument: '{}'", argv[i]);
		}
		else if (length > 1 && argv[i][0] == '-')
		{
			for (int j=1; j < length; j++)
			{
				bool none = true;
				const char arg = argv[i][j];

				for (int k=0; k < (int)arguments.size(); k++)
				{
					const auto& [name, ops, _func] = arguments[k];
					if (name[0] == arg)
					{
						if (ops > 0 && j != length-1)
							WC_EXCEPTION(init, "Argument '{}' requiring non-zero operands should "
										 "be at the end", name[0]);
						if (i+ops >= argc)
							WC_EXCEPTION(init, "Argument '{}' requires {} operands but only {} "
										 "are left", name[0], ops, argc-i-1);
						todo.push_back({k, i});
						i += ops;
						none = false;
					}
				}

				if (none)
					WC_EXCEPTION(init, "Unknown argument: '{}'", argv[i][j]);
			}
		}
		else
		{
			WC_EXCEPTION(init, "Unknown argument: '{}'", argv[i]);
		}
	}
	for (auto [k, i] : todo)
	{
		std::get<2>(arguments[k])(parsed, i);
	}

	for (const auto& [type, what] : parsed.work)
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
	if (parsed.is_repl || parsed.work.empty())
		repl();
}

void why_calculator::execute()
{
	while (stack.size() > 0 && std::holds_alternative<operations_iter_t>(stack.back()))
	{
		auto op = std::move(std::get<operations_iter_t>(stack.back()));
		stack.pop_back();

		const auto& [opr_list, op_func] = op->second;

		if (stack.size() < opr_list.size())
		{
			WC_EXCEPTION(exec, "Operation '{}' requires {} elements but only {} are left",
						 op->first, opr_list.size(), stack.size());
		}
		else
		{
			for (unsigned i=0; i < opr_list.size(); i++)
			{
				const auto& opr = stack[stack.size() - i - 1];
				const auto opr_index = opr_list.size() - i - 1;
				const auto need_opr_type = opr_list[opr_index];

				operand_type opr_type;
				if (std::holds_alternative<number_t>(opr))
					opr_type = operand_type::number;
				else if (std::holds_alternative<std::string>(opr))
					opr_type = operand_type::string;
				else
				{
					WC_STD_EXCEPTION("Unknown operand type '{}' encountered while"
									 "executing operation '{}'. This is a program error",
									 opr.index(), op->first);
				}

				if (need_opr_type != opr_type)
				{
					WC_EXCEPTION(exec, "Expected an operand of type {} at index {} for operation '{}'",
								 need_opr_type == operand_type::string ? "string" :
								 (need_opr_type == operand_type::number ? "number" : "unknown"),
								 opr_index, op->first);
				}
			}

			op_func(this);
		}
	}
}

void why_calculator::ensure_clean_stack()
{
	const auto it_pop_locals = operations.find("_pop_locals"), it_push_locals = operations.find("_push_locals");
	std::list<std::string> names;

	if (variables_local.size() > 0)
	{
		std::list<stack_t::iterator> it_names;
		for (auto it = secondary_stack.begin(); it != secondary_stack.end(); it++)
		{
			if (std::holds_alternative<operations_iter_t>(*it))
			{
				const auto& it_op = std::get<operations_iter_t>(*it);
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
			names.push_back(std::move(std::get<std::string>(*it_name)));
	}

	secondary_stack.clear();
	for (auto& name : names)
	{
		secondary_stack.push_back(std::move(name));
		secondary_stack.push_back(it_pop_locals);
		evaluate();
	}
}

void why_calculator::evaluate()
{
	try
	{
		while (secondary_stack.size() > 0)
		{
			auto elem = std::move(secondary_stack.front());
			secondary_stack.pop_front();

			if (current_eval_function.empty() && current_eval_times.empty())
			{
				if (std::holds_alternative<variable_ref_t>(elem))
				{
					const auto& var = std::get<variable_ref_t>(elem);

					number_t out;
					if (!dereference_variable(var, out))
						WC_EXCEPTION(eval, "No such variable '{}' exists in relevant scopes", var.name);

					elem = out;
				}
				else if (std::holds_alternative<function_ref_t>(elem))
				{
					const auto& func = std::get<function_ref_t>(elem);

					const auto it_func = functions.find(func.name);
					if (it_func == functions.end())
					{
						WC_EXCEPTION(eval, "No such function '{}' exists", func.name);
					}
					else
					{
						const auto& [opr_count, func_stack] = it_func->second;

						if (stack.size() < opr_count)
						{
							WC_EXCEPTION(eval, "Function '{}' requires {} elements but only {} are left",
										 func.name, opr_count, stack.size());
						}
						else
						{
							for (size_t i = 0; i < opr_count; i++)
							{
								const auto& opr = stack[stack.size() - i - 1];
								const auto opr_index = opr_count - i - 1;

								if (!std::holds_alternative<number_t>(opr) && !std::holds_alternative<variable_ref_t>(opr))
								{
									WC_EXCEPTION(eval, "Expected operand of type number or"
												 "variable at index {} for function '{}'",
												 opr_index, func.name);
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

			const bool is_op = std::holds_alternative<operations_iter_t>(elem);
			bool is_only_stack = false;

			if (is_op)
			{
				const auto& name = std::get<operations_iter_t>(elem)->first;
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
		ensure_clean_stack();
		throw;
	}
}

bool why_calculator::dereference_variable(const why_calculator::variable_ref_t& what, number_t& out)
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

why_calculator::number_t why_calculator::resolve_variable_if(const element_t& e)
{
	if (std::holds_alternative<variable_ref_t>(e))
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
		auto num = std::move(std::get<number_t>(e));
		return num;
	}
}

void why_calculator::parse(std::string_view what)
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
		bool assigned = false;

		const auto it_op = operations.find(sub);
		if (it_op != operations.end())
		{
			elem.emplace<operations_iter_t>(it_op);
			assigned = true;
		}

		if (!assigned)
		{
			if (sub[0] == ':')
			{
				if (sub.size() <= 1)
				{
					WC_EXCEPTION(parse, "Empty string provided");
				}
				else
				{
					elem.emplace<std::string>(std::move(sub.substr(1)));
					assigned = true;
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
					elem.emplace<variable_ref_t>(std::move(sub.substr(1)));
					assigned = true;
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
					elem.emplace<function_ref_t>(std::move(sub.substr(1)));
					assigned = true;
				}
			}

			if (!assigned)
			{
				try
				{
					elem.emplace<number_t>(std::stold(sub));
					assigned = true;
				}
				catch (const std::out_of_range&) {}
				catch (const std::invalid_argument&) {}
			}
		}

		if (assigned)
		{
			secondary_stack.push_back(std::move(elem));
			assigned = true;
		}
		else
		{
			WC_EXCEPTION(parse, "Garbage sub-expression: '{}'", sub);
		}
	}

	evaluate();
}

void why_calculator::file(std::string_view what)
{
	std::ifstream ifs(what.data());
	if (ifs.is_open())
	{
		file(ifs);
	}
	else
	{
		WC_EXCEPTION(file, "Cannot open file '{}'", what);
	}
}

void why_calculator::file(std::istream& is)
{
	std::string line;
	while (std::getline(is, line))
	{
		parse(line);
	}
}

void why_calculator::repl()
{
	auto cleanup_local = [](char*& what) {
		if (what)
		{
			free(what);
			what = nullptr;
		}
	};

	using_history();

	bool quit = false;
	while (!quit)
	{
		try
		{
#ifdef WC_USE_TRADITIONAL_GETLINE
			std::string what_alt;

			std::print("{}>> ", stack.size());
			if (!std::getline(std::cin, what_alt))
				WC_EXCEPTION(repl_quit, "");

			if (what_alt.size() > 0)
			{
				parse(what_alt);
			}
#else
			char* what = nullptr;
			try
			{
				auto prompt = std::format("{}>> ", stack.size());

				what = readline(prompt.c_str());
				if (!what)
					WC_EXCEPTION(repl_quit, "");

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
#endif

			if (stack.size() > 0)
			{
				std::print("^");
				op_top(this);
			}
		}
		catch (const why_calculator::exception& e)
		{
			switch (e.type)
			{
			case why_calculator::error_type::parse:
			case why_calculator::error_type::eval:
			case why_calculator::error_type::exec:
			case why_calculator::error_type::file:
				break;
			case why_calculator::error_type::repl_quit:
				quit = true;
				continue;
			default:
				throw;
			}
			std::println(stderr, "Error: {}: {}", why_calculator::error_type_str[static_cast<int>(e.type)], e.what());
		}
	}
}

void why_calculator::display_stack(const stack_t& what_stack)
{
	for (const auto& elem : what_stack)
	{
		if (std::holds_alternative<variable_ref_t>(elem))
		{
			const auto& var = std::get<variable_ref_t>(elem);
			std::print("${}", var.name);
		}
		else if (std::holds_alternative<function_ref_t>(elem))
		{
			const auto& func = std::get<function_ref_t>(elem);
			std::print("@{}", func.name);
		}
		else if (std::holds_alternative<std::string>(elem))
		{
			const auto& str = std::get<std::string>(elem);
			std::print(":{}", str);
		}
		else if (std::holds_alternative<operations_iter_t>(elem))
		{
			const auto& op_it = std::get<operations_iter_t>(elem);
			std::print("{}", op_it->first);
		}
		else
		{
			const auto& num = std::get<number_t>(elem);
			std::print("{}", num);
		}
		std::print(" ");
	}
	if (!what_stack.empty())
		std::println("");
}
