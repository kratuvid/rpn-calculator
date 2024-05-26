#include "wc.hpp"

namespace wc
{
	void wtf_calculator::op_add(wtf_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = b + a;

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " + " << a << std::endl;

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
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " - " << a << std::endl;

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
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " * " << a << std::endl;

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
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " / " << a << std::endl;

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
			std::cerr << ins->stack.size()+1 << "> " << r << " = " << b << " ^ " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void wtf_calculator::op_stack(wtf_calculator* ins)
	{
		for (unsigned i = 0; i < ins->stack.size(); i++)
		{
			const auto& e = ins->stack[i];
			if (e.type() == typeid(number_t))
			{
				std::cout << i << ": " << std::any_cast<number_t>(e);
			}
			else
				WC_STD_EXCEPTION("There shouldn't be non-number on the stack. This is a program error");
			std::cout << std::endl;
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
			std::cerr << ins->stack.size()+1 << "> " << "replace " << b << " > " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(a));
	}

	void wtf_calculator::op_swap(wtf_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+2 << "> " << "swap " << b << " <> " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(a));
		ins->stack.push_back(std::make_any<number_t>(b));
	}

	void wtf_calculator::op_pop(wtf_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size() << "> " << "pop " << a << std::endl;
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
		const auto back = ins->stack.back();

		auto a = std::any_cast<number_t>(back);
		std::cout << a;

		std::cout << std::endl;
	}

	void wtf_calculator::op_neg(wtf_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = -a;

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = -(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void wtf_calculator::op_help(wtf_calculator* ins)
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
help: show this screen)" << std::endl;
	}

	void wtf_calculator::op_sin(wtf_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::sin(a);

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = sin(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void wtf_calculator::op_cos(wtf_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::cos(a);

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = cos(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void wtf_calculator::op_floor(wtf_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::floor(a);

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = floor(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void wtf_calculator::op_ceil(wtf_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::ceil(a);

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = ceil(" << a << ")" << std::endl;

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
			WC_EXCEPTION(exec, "Variable '" << name << "' already exists at scope ";
						 if (is_local) oss << "local";
						 else oss << "global";
						 oss << ". You probably meant to use operation 'set'");
		}

		if (ins->verbose && !ins->suppress_verbose)
		{
			std::cerr << ins->stack.size() << "> new ";
			if (is_local)
			{
				std::cerr << "local:" << ins->variables_local.size()-1 << ' ';
			}
			std::cerr << '$' << name << " = " << value << std::endl;
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
			WC_EXCEPTION(exec, "No such variables '" << name << "' exists in relevant scopes");
		}

		if (ins->verbose && !ins->suppress_verbose)
		{
			std::cerr << ins->stack.size() << "> ";
			if (is_local)
			{
				std::cerr << "local:" << local_index << ' ';
			}
			std::cerr << '$' << name << " = " << value << std::endl;
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
			WC_EXCEPTION(exec, "Variable '" << name << "' already exists at scope global."
						 "You probably meant to use operation 'set'");
		}

		if (ins->verbose && !ins->suppress_verbose)
		{
			std::cerr << ins->stack.size() << "> new $" << name << " = " << value << std::endl;
		}
	}

	void wtf_calculator::op_vars(wtf_calculator* ins)
	{
		for (const auto& [name, value] : ins->variables)
		{
			std::cout << '$' << name << ": " << value << std::endl;
		}

		unsigned i = 0;
		for (const auto& [scope, locals] : ins->variables_local)
		{
			for (const auto& [name, value] : locals)
			{
				std::cout << "local:" << static_cast<int>(scope) << ',' << i << " $" << name << ": "
						  << value << std::endl;
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
			WC_EXCEPTION(exec, "No such variable '" << name << "' exists");
		}
		else
		{
			ins->variables.erase(it);
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
			WC_EXCEPTION(exec, "Cannot begin parsing '" << name
						 << "' as another function is currently being");
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
			WC_EXCEPTION(exec, "No such function '" << name << "' exists");
		}
		else
		{
			const auto& func_stack = std::get<1>(it->second);
			ins->display_stack(func_stack);
		}
	}

    void wtf_calculator::op_funcs(wtf_calculator* ins)
	{
		for (const auto& [name, stuff] : ins->functions)
		{
			std::cout << '@' << name << ": " << std::get<0>(stuff) << " arguments, "
					  << std::get<1>(stuff).size() << " elements" << std::endl;
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
			std::cerr << ins->stack.size() << "> begin "
					  << name << " - " << scope << ',' << ins->variables_local.size()
					  << std::endl;
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
			std::cerr << ins->stack.size() << "> end "
					  << name << " - " << scope << ',' << ins->variables_local.size()-1;
			if (freed > 0)
				std::cerr << " - freed " << freed << " variables";
			std::cerr << std::endl;
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
		for (const auto& l : ins->times)
		{
			std::cout << "times:" << i << ": " << l.size() << " elements" << std::endl;
			i++;
		}
	}

	void wtf_calculator::op_desc_loop(wtf_calculator* ins)
	{
		auto index = (unsigned)std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		if (index >= ins->times.size())
		{
			WC_EXCEPTION(exec, "No times at index " << index << " exists");
		}

		const auto& times_stack = ins->times[index];
		ins->display_stack(times_stack);
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

		std::cout << what;
	}

	void wtf_calculator::op_println(wtf_calculator* ins)
	{
		op_print(ins);
		std::cout << std::endl;
	}
}; // namespace wc
