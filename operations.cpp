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
		ins->stack.pop_back();
		if (std::fpclassify(a) == FP_ZERO)
			throw sc::exception("Cannot divide by 0", sc::error_type::exec);
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

		auto num = std::any_cast<number_t>(back);
		std::cout << num;

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
---
var: n, s: set a variable s with n
vars: list all variables
del: s: delete variable s
delall: delete all variables
---
begin: n, s: begin a function declaration
end: end the function declaration
describe: s: show the elements of the function
funcs: briefly list out all functions
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
			std::cout << '$' << v.first << ": " << v.second << std::endl;
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

	void simple_calculator::op_begin(simple_calculator* ins)
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

	void simple_calculator::op_describe(simple_calculator* ins)
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
			for (const auto& elem : function_stack)
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
			std::cout << std::endl;
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
}; // namespace sc
