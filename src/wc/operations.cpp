#include "wc/wc.hpp"

void wtf_calculator::op_add(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();
	auto b = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = b + a;

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = {} + {}", ins->stack.size()+1, r, b, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_subtract(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();
	auto b = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = b - a;

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = {} - {}", ins->stack.size()+1, r, b, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_multiply(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();
	auto b = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = b * a;

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = {} * {}", ins->stack.size()+1, r, b, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_divide(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();
	if (std::fpclassify(a) == FP_ZERO)
		WC_EXCEPTION(exec, "Cannot divide by 0");
	auto b = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = b / a;

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = {} / {}", ins->stack.size()+1, r, b, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_power(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();
	auto b = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = std::pow(b, a);

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = {} ^ {}", ins->stack.size()+1, r, b, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_stack(wtf_calculator* ins)
{
	for (unsigned i = 0; i < ins->stack.size(); i++)
	{
		const auto& e = ins->stack[i];
		if (e.type() == typeid(number_t))
		{
			std::print("{}: {}", i, std::any_cast<number_t>(e));
		}
		else
			WC_STD_EXCEPTION("There shouldn't be non-number_t '{}' on the stack. "
							 "This is a program error", e.type().name());
		std::println("");
	}
}

void wtf_calculator::op_quit(wtf_calculator* ins)
{
	WC_EXCEPTION(repl_quit, "");
}

void wtf_calculator::op_replace(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();
	auto b = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> replace {} > {}", ins->stack.size()+1, b, a);

	ins->stack.push_back(std::make_any<number_t>(a));
}

void wtf_calculator::op_swap(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();
	auto b = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> swap {} <> {}", ins->stack.size()+2, b, a);

	ins->stack.push_back(std::make_any<number_t>(a));
	ins->stack.push_back(std::make_any<number_t>(b));
}

void wtf_calculator::op_pop(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> pop {}", ins->stack.size(), a);
}

void wtf_calculator::op_clear(wtf_calculator* ins)
{
	ins->stack.clear();
}

void wtf_calculator::op_file(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();
	ins->file(name);
}

void wtf_calculator::op__view(wtf_calculator* ins)
{
	ins->display_stack(ins->stack);
}

void wtf_calculator::op_top(wtf_calculator* ins)
{
	wtf_calculator::op_topb(ins);
	std::println("");
}

void wtf_calculator::op_topb(wtf_calculator* ins)
{
	auto a = std::any_cast<number_t>(ins->stack.back());

	std::print("{}", a);
}

void wtf_calculator::op_neg(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = -a;

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = -({})", ins->stack.size()+1, r, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_help(wtf_calculator* ins)
{
	std::println(stderr, R"(operation: operand size: description:
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
top: n: display the top of the stack
topb: n: display the top of the stack without the newline
---
var: n, s: set a global/local variable s with n depending on context
varg: n, s: set a global variable s with n
vars: list all variables
del: s: delete variable s
delall: delete all variables
---
defun: n, s: begin a function declaration
end: end the function declaration
desc: s: show the elements of the function
funcs: briefly list out all functions
---
times: n: execute the loop code n times
desc-loop: n: show the elements of loop n
loops: briefly list out all loops
end-times: end the last times loop
---
noverbose: suppress verbose even if enabled
verbose: unsuppress verbose
---
print: s: print s to standard output. '`' will be replaced with space
println: s: print s and a newline to the standard output. Same with '`'
---
file: s: read commands from file
quit: quit the REPL
---
help: show this screen)");
}

void wtf_calculator::op_sin(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = std::sin(a);

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = sin({})", ins->stack.size()+1, r, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_cos(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = std::cos(a);

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = cos({})", ins->stack.size()+1, r, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_floor(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = std::floor(a);

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = floor({})", ins->stack.size()+1, r, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_ceil(wtf_calculator* ins)
{
	auto a = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto r = std::ceil(a);

	if (ins->verbose && !ins->suppress_verbose)
		std::println(stderr, "{}> {} = ceil({})", ins->stack.size()+1, r, a);

	ins->stack.push_back(std::make_any<number_t>(r));
}

void wtf_calculator::op_var(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();
	auto value = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	bool is_local = false, exists = true;

	if (ins->variables_local.size() > 0)
	{
		auto& locals = std::get<1>(ins->variables_local.back());

		auto it_local = locals.find(name);
		if (it_local == locals.end())
		{
			is_local = true;
			exists = false;
			locals[name] = value;
		}
	}
	else
	{
		auto it_global = ins->variables.find(name);
		if (it_global == ins->variables.end())
		{
			exists = false;
			ins->variables[name] = value;
		}
	}

	if (exists)
	{
		WC_EXCEPTION(exec, "Variable '{}' already exists at scope {}. You probably meant to use 'set'",
					 name, is_local ? "local" : "global");
	}

	if (ins->verbose && !ins->suppress_verbose)
	{
		std::print(stderr, "{}> new ", ins->stack.size());
		if (is_local)
			std::print(stderr, "local:{} ", ins->variables_local.size()-1);
		std::println(stderr, "${} = {}", name, value);
	}
}

void wtf_calculator::op_set(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();
	auto value = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	bool is_local = false, found = false;
	int local_index = (int)ins->variables_local.size() - 1;

	for (auto it = ins->variables_local.rbegin(); it != ins->variables_local.rend(); it++, local_index--)
	{
		auto& [scope, locals] = *it;

		auto it_local = locals.find(name);
		if (it_local != locals.end())
		{
			found = is_local = true;
			it_local->second = value;
			break;
		}

		if (scope != scope_type::loop)
			break;
	}

	if (!found)
	{
		auto it_global = ins->variables.find(name);
		if (it_global != ins->variables.end())
		{
			found = true;
			it_global->second = value;
		}
	}

	if (!found)
	{
		WC_EXCEPTION(exec, "No such variables '{}' exists in relevant scopes", name);
	}

	if (ins->verbose && !ins->suppress_verbose)
	{
		std::print(stderr, "{}> ", ins->stack.size());
		if (is_local)
			std::print(stderr, "local:{} ", ins->variables_local.size()-1);
		std::println(stderr, "${} = {}", name, value);
	}
}

void wtf_calculator::op_varg(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();
	auto value = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	auto it_global = ins->variables.find(name);
	if (it_global == ins->variables.end())
	{
		ins->variables[name] = value;
	}
	else
	{
		WC_EXCEPTION(exec, "Variable '{}' already exists at scope global."
					 "You probably meant to use operation 'set'", name);
	}

	if (ins->verbose && !ins->suppress_verbose)
	{
		std::println(stderr, "{}> new ${} = {}", ins->stack.size(), name, value);
	}
}

void wtf_calculator::op_vars(wtf_calculator* ins)
{
	for (const auto& [name, value] : ins->variables)
	{
		std::println("${}: {}", name, value);
	}

	unsigned i = 0;
	for (const auto& [scope, locals] : ins->variables_local)
	{
		for (const auto& [name, value] : locals)
		{
			std::println("local:{},{} ${}: {}", static_cast<int>(scope), i, name, value);
		}
		i++;
	}
}

void wtf_calculator::op_del(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();

	const auto it = ins->variables.find(name);
	if (it == ins->variables.end())
	{
		WC_EXCEPTION(exec, "No such variable '{}' exists", name);
	}
	else
	{
		ins->variables.erase(it);

		if (ins->verbose && !ins->suppress_verbose)
		{
			std::println(stderr, "{}> del ${}", ins->stack.size(), name);
		}
	}
}

void wtf_calculator::op_delall(wtf_calculator* ins)
{
	ins->variables.clear();
}

void wtf_calculator::op_defun(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();
	auto num = (unsigned) ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	if (!ins->current_eval_function.empty())
	{
		WC_EXCEPTION(exec, "Cannot begin parsing '{}' as another function is currently being", name);
	}

	ins->functions[name] = function_t(num, {});
	ins->current_eval_function = name;
}

void wtf_calculator::op_end(wtf_calculator* ins)
{
	if (ins->current_eval_function.empty())
	{
		WC_EXCEPTION(exec, "Unexpected call to operation end");
	}

	ins->current_eval_function.clear();
}

void wtf_calculator::op_desc(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();

	const auto it = ins->functions.find(name);
	if (it == ins->functions.end())
	{
		WC_EXCEPTION(exec, "No such function '{}' exists", name);
	}
	else
	{
		const auto& func_stack = std::get<1>(it->second);
		wtf_calculator::display_stack(func_stack);
	}
}

void wtf_calculator::op_funcs(wtf_calculator* ins)
{
	for (const auto& [name, stuff] : ins->functions)
	{
		std::println("@{}: {} arguments, {} elements",
					 name, std::get<0>(stuff), std::get<1>(stuff).size());
	}
}

void wtf_calculator::op__push_locals(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();
	auto scope = ins->resolve_variable_if(ins->stack.back());
	ins->stack.pop_back();

	if (ins->verbose && !ins->suppress_verbose)
	{
		std::println(stderr, "{}> begin {} - {},{}",
					 ins->stack.size(), name, scope, ins->variables_local.size());
	}

	ins->variables_local.push_back({static_cast<scope_type>(scope), {}});
}

void wtf_calculator::op__pop_locals(wtf_calculator* ins)
{
	auto name = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();

	if (ins->variables_local.empty())
	{
		WC_STD_EXCEPTION("Operation '_pop_locals' executed on an empty list. This is a program error");
	}

	if (ins->verbose && !ins->suppress_verbose)
	{
		auto scope = static_cast<int>(std::get<0>(ins->variables_local.back()));
		auto freed = std::get<1>(ins->variables_local.back()).size();
		std::print(stderr, "{}> end {} - {},{}",
				   ins->stack.size(), name, scope, ins->variables_local.size()-1);
		if (freed > 0)
			std::print(stderr, " - freed {} variables", freed);
		std::println(stderr, "");
	}

	ins->variables_local.pop_back();
}

void wtf_calculator::op_times(wtf_calculator* ins)
{
	ins->current_eval_times.push_back(ins->times.size());
	ins->times.push_back({});
}

void wtf_calculator::op_loops(wtf_calculator* ins)
{
	unsigned i=0;
	for (const auto& s : ins->times)
	{
		std::println("times:{}: {} elements", i, s.size());
		i++;
	}
}

void wtf_calculator::op_desc_loop(wtf_calculator* ins)
{
	auto index = (unsigned)std::any_cast<number_t>(ins->stack.back());
	ins->stack.pop_back();

	if (index >= ins->times.size())
	{
		WC_EXCEPTION(exec, "No times at index {} exists", index);
	}

	const auto& times_stack = ins->times[index];
	wtf_calculator::display_stack(times_stack);
}

void wtf_calculator::op_end_times(wtf_calculator* ins)
{
	if (ins->current_eval_times.empty())
	{
		WC_STD_EXCEPTION("Unexpected operation 'end-times'");
	}
	ins->current_eval_times.pop_back();
}

void wtf_calculator::op__use_times(wtf_calculator* ins)
{
	auto index = (unsigned)std::any_cast<number_t>(ins->stack.back());
	ins->stack.pop_back();
	auto loops = (unsigned)std::any_cast<number_t>(ins->stack.back());
	ins->stack.pop_back();

	std::string name = "times:";
	name += std::to_string(index);
	for (unsigned i=0; i < loops; i++)
	{
		ins->secondary_stack.push_front(ins->operations.find("_pop_locals"));
		ins->secondary_stack.push_front(name);
		for (auto it = ins->times[index].rbegin(); it != ins->times[index].rend(); it++)
		{
			ins->secondary_stack.push_front(*it);
		}
		ins->secondary_stack.push_front(ins->operations.find("_push_locals"));
		ins->secondary_stack.push_front(name);
		ins->secondary_stack.push_front(static_cast<number_t>(wtf_calculator::scope_type::loop));
	}
}

void wtf_calculator::op_noverbose(wtf_calculator* ins)
{
	ins->suppress_verbose = true;
}

void wtf_calculator::op_verbose(wtf_calculator* ins)
{
	ins->suppress_verbose = false;
}

void wtf_calculator::op_print(wtf_calculator* ins)
{
	auto what = std::any_cast<std::string&&>(std::move(ins->stack.back()));
	ins->stack.pop_back();

	for (unsigned i=0; i < what.size(); i++)
		if (what[i] == '`')
			what[i] = ' ';

	std::print("{}", what);
}

void wtf_calculator::op_println(wtf_calculator* ins)
{
	op_print(ins);
	std::println("");
}
