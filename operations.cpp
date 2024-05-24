#include "sc.hpp"

namespace sc
{
	void simple_calculator::op_add(simple_calculator* ins)
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

	void simple_calculator::op_subtract(simple_calculator* ins)
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

	void simple_calculator::op_multiply(simple_calculator* ins)
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

	void simple_calculator::op_divide(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();
		if (std::fpclassify(a) == FP_ZERO)
			throw sc::exception("Cannot divide by 0", sc::error_type::exec);
		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = b / a;

		if (ins->verbose && !ins->suppress_verbose)
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

		if (ins->verbose && !ins->suppress_verbose)
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
			else
				throw std::logic_error("There shouldn't be non-number on the stack. This is a program error");
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

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << "replace " << b << " > " << a << std::endl;

		ins->stack.push_back(std::make_any<number_t>(a));
	}

	void simple_calculator::op_swap(simple_calculator* ins)
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

	void simple_calculator::op_pop(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose && !ins->suppress_verbose)
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

	void simple_calculator::op__view(simple_calculator* ins)
	{
		ins->display_stack(ins->stack);
	}

	void simple_calculator::op_top(simple_calculator* ins)
	{
		const auto back = ins->stack.back();

		auto num = std::any_cast<number_t>(back);
		std::cout << num;

		std::cout << std::endl;
	}

	void simple_calculator::op_neg(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = -a;

		if (ins->verbose && !ins->suppress_verbose)
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

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = sin(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_cos(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::cos(a);

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = cos(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_floor(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::floor(a);

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = floor(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_ceil(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto r = std::ceil(a);

		if (ins->verbose && !ins->suppress_verbose)
			std::cerr << ins->stack.size()+1 << "> " << r << " = ceil(" << a << ")" << std::endl;

		ins->stack.push_back(std::make_any<number_t>(r));
	}

	void simple_calculator::op_var(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		bool is_local = false, exists = true;

		if (ins->variables_local.size() > 0)
		{
			auto& local = std::get<1>(ins->variables_local.back());

			auto it_local = local.find(a);
			if (it_local == local.end())
			{
				is_local = true;
				exists = false;
				local[a] = b;
			}
		}
		else
		{
			auto it_global = ins->variables.find(a);
			if (it_global == ins->variables.end())
			{
				exists = false;
				ins->variables[a] = b;
			}
		}

		if (exists)
		{
			std::ostringstream oss;
			oss << "Variable '" << a << "' already exists at scope ";
			if (is_local) oss << "local";
			else oss << "global";
			oss << ". You probably meant to use operation 'set'";
			throw sc::exception(oss.str(), sc::error_type::exec);
		}

		if (ins->verbose && !ins->suppress_verbose)
		{
			std::cerr << ins->stack.size() << "> new ";
			if (is_local)
			{
				std::cerr << "local:" << ins->variables_local.size()-1 << ' ';
			}
			std::cerr << '$' << a << " = " << b << std::endl;
		}
	}

	void simple_calculator::op_set(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		bool is_local = false, found = false;
		int local_index;

		local_index = (int)ins->variables_local.size() - 1;
		for (auto it = ins->variables_local.rbegin(); it != ins->variables_local.rend(); it++, local_index--)
		{
			auto scope = std::get<0>(*it);
			auto& local = std::get<1>(*it);

			auto it_local = local.find(a);
			if (it_local != local.end())
			{
				found = is_local = true;
				it_local->second = b;
				break;
			}

			if (scope != scope_type::loop)
				break;
		}

		if (!found)
		{
			auto it_global = ins->variables.find(a);
			if (it_global != ins->variables.end())
			{
				found = true;
				it_global->second = b;
			}
		}

		if (!found)
		{
			std::ostringstream oss;
			oss << "No such variables '" << a << "' exists in relevant scopes";
			throw sc::exception(oss.str(), sc::error_type::exec);
		}

		if (ins->verbose && !ins->suppress_verbose)
		{
			std::cerr << ins->stack.size() << "> ";
			if (is_local)
			{
				std::cerr << "local:" << local_index << ' ';
			}
			std::cerr << '$' << a << " = " << b << std::endl;
		}
	}

	void simple_calculator::op_varg(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		auto it_global = ins->variables.find(a);
		if (it_global == ins->variables.end())
		{
			ins->variables[a] = b;
		}
		else
		{
			std::ostringstream oss;
			oss << "Variable '" << a << "' already exists at scope global. You probably meant to use operation 'set'";
			throw sc::exception(oss.str(), sc::error_type::exec);
		}

		if (ins->verbose && !ins->suppress_verbose)
		{
			std::cerr << ins->stack.size() << "> new $" << a << " = " << b << std::endl;
		}
	}

	void simple_calculator::op_vars(simple_calculator* ins)
	{
		for (const auto& v : ins->variables)
		{
			std::cout << '$' << v.first << ": " << v.second << std::endl;
		}

		unsigned i = 0;
		for (const auto& locals_raw : ins->variables_local)
		{
			auto scope = static_cast<int>(std::get<0>(locals_raw));
			const auto& locals = std::get<1>(locals_raw);
			for (const auto& v : locals)
			{
				std::cout << "local:" << scope << ',' << i << " $" << v.first << ": "
						  << v.second << std::endl;
			}
			i++;
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
			throw sc::exception(oss.str(), sc::error_type::exec);
		}
		else
		{
			ins->variables.erase(it);
		}
	}

	void simple_calculator::op_delall(simple_calculator* ins)
	{
		ins->variables.clear();
	}

	void simple_calculator::op_defun(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		if (!ins->current_eval_function.empty())
		{
			std::ostringstream oss;
			oss << "Cannot begin parsing '" << a << "' as another function is currently being";
			throw sc::exception(oss.str(), sc::error_type::exec);
		}

		ins->functions[a] = function_t(b, {});
		ins->current_eval_function = a;
	}

	void simple_calculator::op_end(simple_calculator* ins)
	{
		if (ins->current_eval_function.empty())
		{
			throw sc::exception("Unexpected call to operation end", sc::error_type::exec);
		}

		ins->current_eval_function.clear();
	}

	void simple_calculator::op_desc(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		const auto it = ins->functions.find(a);
		if (it == ins->functions.end())
		{
			std::ostringstream oss;
			oss << "No such function '" << a << "' exists";
			throw sc::exception(oss.str(), sc::error_type::exec);
		}
		else
		{
			const auto& function_stack = std::get<1>(it->second);
			ins->display_stack(function_stack);
		}
	}

    void simple_calculator::op_funcs(simple_calculator* ins)
	{
		for (const auto& f : ins->functions)
		{
			std::cout << '@' << f.first << ": " << std::get<0>(f.second) << " arguments, "
					  << std::get<1>(f.second).size() << " elements" << std::endl;
		}
	}

	void simple_calculator::op__push_locals(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		auto b = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

		if (ins->verbose && !ins->suppress_verbose)
		{
			std::cerr << ins->stack.size() << "> begin "
					  << a << ':' << b << ',' << ins->variables_local.size()
					  << std::endl;
		}

		ins->variables_local.push_back({static_cast<scope_type>(b), {}});
	}

	void simple_calculator::op__pop_locals(simple_calculator* ins)
	{
		auto a = std::any_cast<std::string&&>(std::move(ins->stack.back()));
		ins->stack.pop_back();

		if (ins->variables_local.empty())
		{
			throw std::runtime_error("Operation '_pop_locals' executed on an empty list. This is a program error");
		}

		if (ins->verbose && !ins->suppress_verbose)
		{
			auto scope = static_cast<int>(std::get<0>(ins->variables_local.back()));
			auto num = std::get<1>(ins->variables_local.back()).size();
			std::cerr << ins->stack.size() << "> end "
					  << a << ':' << scope << ',' << ins->variables_local.size()-1;
			if (num > 0)
				std::cerr << " - freed " << num << " variables";
			std::cerr << std::endl;
		}

		ins->variables_local.pop_back();
	}

	void simple_calculator::op_times(simple_calculator* ins)
	{
		auto a = ins->resolve_variable_if(ins->stack.back());
		ins->stack.pop_back();

	    ins->current_eval_times = ins->times.size();
		ins->times.push_back({(unsigned)a, {}});
	}

	void simple_calculator::op_loops(simple_calculator* ins)
	{
		unsigned i=0;
		for (const auto& l : ins->times)
		{
			std::cout << "times:" << i << ": " << std::get<0>(l) << " times, "
					  << std::get<1>(l).size() << " elements" << std::endl;
			i++;
		}
	}

	void simple_calculator::op_desc_loop(simple_calculator* ins)
	{
		auto a = (unsigned)std::any_cast<number_t>(ins->stack.back());
		ins->stack.pop_back();

		if (a >= ins->times.size())
		{
			std::ostringstream oss;
			oss << "No loop at index " << a << " exists";
			throw sc::exception(oss.str(), sc::error_type::exec);
		}

		const auto& times_stack = std::get<1>(ins->times[a]);
		ins->display_stack(times_stack);
	}

	void simple_calculator::op_end_times(simple_calculator* ins)
	{
		if (ins->current_eval_times == -1)
		{
			throw std::runtime_error("Unexpected operation 'end-times'");
		}
		ins->current_eval_times = -1;
	}

	void simple_calculator::op_noverbose(simple_calculator* ins)
	{
		ins->suppress_verbose = true;
	}

	void simple_calculator::op_verbose(simple_calculator* ins)
	{
		ins->suppress_verbose = false;
	}
}; // namespace sc
