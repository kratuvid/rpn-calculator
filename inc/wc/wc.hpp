#pragma once

#include "wc/defines.hpp"

import <array>;
import <cctype>;
import <chrono>;
import <cmath>;
import <cstring>;
import <deque>;
import <exception>;
import <format>;
import <fstream>;
import <iostream>;
import <list>;
import <print>;
import <source_location>;
import <string>;
import <string_view>;
import <tuple>;
import <unordered_map>;
import <variant>;

#include <readline/readline.h>
#include <readline/history.h>

class why_calculator
{
public:
	enum class error_type { init, init_help, parse, eval, exec, file, repl_quit };
	static constexpr std::array<std::string_view, 7> error_type_str { "init", "init_help", "parse", "eval", "exec", "file", "repl_quit" };
	class exception;

	enum class operand_type { number, string };
	enum class scope_type { function, loop };

	using number_t = long double;

	using operation_t = std::tuple<std::vector<operand_type>, void(*)(why_calculator*)>;
	using operations_iter_t = std::unordered_map<std::string, operation_t>::const_iterator;

	struct variable_ref_t {
		std::string name;
		variable_ref_t() = delete;
		variable_ref_t(const std::string& name) :name(name) {}
		variable_ref_t(std::string&& name) :name(name) {}
	};
	struct function_ref_t {
		std::string name;
		function_ref_t() = delete;
		function_ref_t(const std::string& name) :name(name) {}
		function_ref_t(std::string&& name) :name(name) {}
	};

	using element_t = std::variant<number_t, std::string, operations_iter_t, variable_ref_t, function_ref_t>;
	
	template<typename T> using stack_base_t = std::deque<T>;
	using stack_t = stack_base_t<element_t>;

	using function_t = std::tuple<unsigned, stack_t>;

private:
	const std::unordered_map<std::string, operation_t> operations {{
			{"+", {{operand_type::number, operand_type::number}, op_add}},
			{"-", {{operand_type::number, operand_type::number}, op_subtract}},
			{"*", {{operand_type::number, operand_type::number}, op_multiply}},
			{"/", {{operand_type::number, operand_type::number}, op_divide}},
			{"^", {{operand_type::number, operand_type::number}, op_power}},

			{"replace", {{operand_type::number, operand_type::number}, op_replace}},
			{"swap", {{operand_type::number, operand_type::number}, op_swap}},
			{"pop", {{operand_type::number}, op_pop}},
			{"top", {{operand_type::number}, op_top}},
			{"topb", {{operand_type::number}, op_topb}},

			{"neg", {{operand_type::number}, op_neg}},
			{"sin", {{operand_type::number}, op_sin}}, {"cos", {{operand_type::number}, op_cos}},
			{"floor", {{operand_type::number}, op_floor}}, {"ceil", {{operand_type::number}, op_ceil}},

			{"help", {{}, op_help}}, {"stack", {{}, op_stack}}, {"quit", {{}, op_quit}},
			{"clear", {{}, op_clear}}, {"file", {{operand_type::string}, op_file}},
			{"_view", {{}, op__view}},

			{"var", {{operand_type::number, operand_type::string}, op_var}},
			{"set", {{operand_type::number, operand_type::string}, op_set}},
			{"varg", {{operand_type::number, operand_type::string}, op_varg}},
			{"vars", {{}, op_vars}},
			{"del", {{operand_type::string}, op_del}},
			{"delall", {{}, op_delall}},

			{"defun", {{operand_type::number, operand_type::string}, op_defun}},
			{"end", {{}, op_end}},
			{"desc", {{operand_type::string}, op_desc}},
			{"funcs", {{}, op_funcs}},
			{"_push_locals", {{operand_type::number, operand_type::string}, op__push_locals}},
			{"_pop_locals", {{operand_type::string}, op__pop_locals}},

			{"times", {{}, op_times}},
			{"desc-loop", {{operand_type::number}, op_desc_loop}},
			{"loops", {{}, op_loops}},
			{"end-times", {{}, op_end_times}},
			{"_use_times", {{operand_type::number, operand_type::number}, op__use_times}},

			{"noverbose", {{}, op_noverbose}},
			{"verbose", {{}, op_verbose}},

			{"print", {{operand_type::string}, op_print}},
			{"println", {{operand_type::string}, op_println}}
		}
	};

	stack_t stack, secondary_stack;
	std::deque<stack_t> times;
	std::unordered_map<std::string, function_t> functions;
	std::unordered_map<std::string, number_t> variables {{
			{"pi", 3.141592653589793238L},
			{"e", 2.718281828459045235L}
		}
	};
	std::list<std::tuple<scope_type, decltype(variables)>> variables_local;

	std::list<unsigned> current_eval_times;
	std::string current_eval_function;
	bool verbose = false, suppress_verbose = false;
	bool is_prefix = false;

	bool is_time = false;
	std::chrono::high_resolution_clock::time_point tp_begin;

private:
	static void op_add(why_calculator* ins);
	static void op_subtract(why_calculator* ins);
	static void op_multiply(why_calculator* ins);
	static void op_divide(why_calculator* ins);
	static void op_power(why_calculator* ins);

	static void op_replace(why_calculator* ins);
	static void op_swap(why_calculator* ins);
	static void op_pop(why_calculator* ins);
	static void op_top(why_calculator* ins);
	static void op_topb(why_calculator* ins);

	static void op_neg(why_calculator* ins);
	static void op_sin(why_calculator* ins);
	static void op_cos(why_calculator* ins);
	static void op_floor(why_calculator* ins);
	static void op_ceil(why_calculator* ins);

	static void op_help(why_calculator* ins);
	static void op_stack(why_calculator* ins);
	static void op_quit(why_calculator* ins);
	static void op_clear(why_calculator* ins);
	static void op_file(why_calculator* ins);
	static void op__view(why_calculator* ins);

	static void op_var(why_calculator* ins);
	static void op_set(why_calculator* ins);
	static void op_varg(why_calculator* ins);
	static void op_vars(why_calculator* ins);
	static void op_del(why_calculator* ins);
	static void op_delall(why_calculator* ins);

	static void op_defun(why_calculator* ins);
	static void op_end(why_calculator* ins);
	static void op_desc(why_calculator* ins);
	static void op_funcs(why_calculator* ins);
	static void op__push_locals(why_calculator* ins);
	static void op__pop_locals(why_calculator* ins);

	static void op_times(why_calculator* ins);
	static void op_loops(why_calculator* ins);
	static void op_desc_loop(why_calculator* ins);
	static void op_end_times(why_calculator* ins);
	static void op__use_times(why_calculator* ins);

	static void op_noverbose(why_calculator* ins);
	static void op_verbose(why_calculator* ins);

	static void op_print(why_calculator* ins);
	static void op_println(why_calculator* ins);

private:
	static void show_help(char* name);
	void parse_arguments(int argc, char** argv);

	void execute();
	void evaluate();
	bool dereference_variable(const variable_ref_t& what, number_t& out);
	void ensure_clean_stack();
	number_t resolve_variable_if(const element_t& e);

	void parse(std::string_view what);
	void file(std::string_view what);
	void file(std::istream& is);
	void repl();

	static void display_stack(const stack_t& what_stack);

public:
	why_calculator();
	void start(int argc, char** argv);
	~why_calculator();
};

class why_calculator::exception : public std::exception
{
public:
	error_type type;
	std::string msg;

	exception(std::string_view msg, error_type type)
		:type(type), msg(msg)
	{}

	const char* what() const noexcept override
	{ return msg.c_str(); }
};
