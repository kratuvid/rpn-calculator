#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <cctype>
#include <string_view>
#include <cstring>
#include <exception>
#include <vector>
#include <array>
#include <cmath>
#include <list>
#include <any>
#include <tuple>
#include <unordered_map>
#include <deque>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <print>
#include <iostream>

#include <readline/readline.h>
#include <readline/history.h>

#include "utility.hpp"

namespace wc
{
	class wtf_calculator
	{
	public:
		enum class operand_type { number, string };
		enum class scope_type { function, loop };

		using number_t = long double;
		using element_t = std::any;

		template<typename T> using stack_base_t = std::deque<T>;
		using stack_t = stack_base_t<element_t>;

		using function_t = std::tuple<unsigned, stack_t>;
		using operation_t = std::tuple<std::vector<operand_type>, void(*)(wtf_calculator*)>;
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
		static void op_add(wtf_calculator* ins);
		static void op_subtract(wtf_calculator* ins);
		static void op_multiply(wtf_calculator* ins);
		static void op_divide(wtf_calculator* ins);
		static void op_power(wtf_calculator* ins);

		static void op_replace(wtf_calculator* ins);
		static void op_swap(wtf_calculator* ins);
		static void op_pop(wtf_calculator* ins);
		static void op_top(wtf_calculator* ins);
		static void op_topb(wtf_calculator* ins);

		static void op_neg(wtf_calculator* ins);
		static void op_sin(wtf_calculator* ins);
		static void op_cos(wtf_calculator* ins);
		static void op_floor(wtf_calculator* ins);
		static void op_ceil(wtf_calculator* ins);

		static void op_help(wtf_calculator* ins);
		static void op_stack(wtf_calculator* ins);
		static void op_quit(wtf_calculator* ins);
		static void op_clear(wtf_calculator* ins);
		static void op_file(wtf_calculator* ins);
		static void op__view(wtf_calculator* ins);

		static void op_var(wtf_calculator* ins);
		static void op_set(wtf_calculator* ins);
		static void op_varg(wtf_calculator* ins);
		static void op_vars(wtf_calculator* ins);
		static void op_del(wtf_calculator* ins);
		static void op_delall(wtf_calculator* ins);

		static void op_defun(wtf_calculator* ins);
		static void op_end(wtf_calculator* ins);
		static void op_desc(wtf_calculator* ins);
		static void op_funcs(wtf_calculator* ins);
		static void op__push_locals(wtf_calculator* ins);
		static void op__pop_locals(wtf_calculator* ins);

		static void op_times(wtf_calculator* ins);
		static void op_loops(wtf_calculator* ins);
		static void op_desc_loop(wtf_calculator* ins);
		static void op_end_times(wtf_calculator* ins);
		static void op__use_times(wtf_calculator* ins);

		static void op_noverbose(wtf_calculator* ins);
		static void op_verbose(wtf_calculator* ins);

		static void op_print(wtf_calculator* ins);
		static void op_println(wtf_calculator* ins);

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
		wtf_calculator();
		void start(int argc, char** argv);
		~wtf_calculator();
	};
};
