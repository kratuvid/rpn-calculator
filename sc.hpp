#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cctype>
#include <string_view>
#include <cstring>
#include <exception>
#include <regex>
#include <vector>
#include <stack>
#include <array>
#include <cmath>
#include <list>
#include <any>
#include <tuple>
#include <unordered_map>
#include <deque>

#include <readline/readline.h>
#include <readline/history.h>

#include "utility.hpp"

namespace sc
{
	class simple_calculator
	{
	public:
		enum class operand_type { number, string };

		using element_t = std::any;

		using operation_t = std::tuple<std::vector<operand_type>, void(*)(simple_calculator*)>;
		using function_t = std::tuple<unsigned, std::deque<element_t>>;

		using number_t = long double;
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
		using operations_iter_t = std::unordered_map<std::string, operation_t>::const_iterator;

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

				{"neg", {{operand_type::number}, op_neg}},
				{"sin", {{operand_type::number}, op_sin}}, {"cos", {{operand_type::number}, op_cos}},
				{"floor", {{operand_type::number}, op_floor}}, {"ceil", {{operand_type::number}, op_ceil}},

				{"help", {{}, op_help}}, {"stack", {{}, op_stack}}, {"quit", {{}, op_quit}},
				{"clear", {{}, op_clear}}, {"file", {{operand_type::string}, op_file}},
				{"_view", {{}, op__view}},

				{"var", {{operand_type::number, operand_type::string}, op_var}},
				{"varg", {{operand_type::number, operand_type::string}, op_varg}},
				{"vars", {{}, op_vars}},
				{"del", {{operand_type::string}, op_del}},
				{"delall", {{}, op_delall}},

				{"defun", {{operand_type::number, operand_type::string}, op_defun}},
				{"end", {{}, op_end}},
				{"desc", {{operand_type::string}, op_desc}},
				{"funcs", {{}, op_funcs}},
				{"_push_locals", {{operand_type::string}, op__push_locals}},
				{"_pop_locals", {{operand_type::string}, op__pop_locals}},
		    }
		};

		std::deque<element_t> stack;
		std::deque<element_t> secondary_stack;
		std::unordered_map<std::string, function_t> functions;
		std::unordered_map<std::string, number_t> variables;
		std::list<std::unordered_map<std::string, number_t>> variables_local;

		std::string current_eval_function;
		bool verbose = false;

	private:
		static void op_add(simple_calculator* ins);
		static void op_subtract(simple_calculator* ins);
		static void op_multiply(simple_calculator* ins);
		static void op_divide(simple_calculator* ins);
		static void op_power(simple_calculator* ins);

		static void op_replace(simple_calculator* ins);
		static void op_swap(simple_calculator* ins);
		static void op_pop(simple_calculator* ins);
		static void op_top(simple_calculator* ins);

		static void op_neg(simple_calculator* ins);
		static void op_sin(simple_calculator* ins);
		static void op_cos(simple_calculator* ins);
		static void op_floor(simple_calculator* ins);
		static void op_ceil(simple_calculator* ins);

		static void op_help(simple_calculator* ins);
		static void op_stack(simple_calculator* ins);
		static void op_quit(simple_calculator* ins);
		static void op_clear(simple_calculator* ins);
		static void op_file(simple_calculator* ins);
		static void op__view(simple_calculator* ins);

		static void op_var(simple_calculator* ins);
		static void op_varg(simple_calculator* ins);
		static void op_vars(simple_calculator* ins);
		static void op_del(simple_calculator* ins);
		static void op_delall(simple_calculator* ins);

		static void op_defun(simple_calculator* ins);
		static void op_end(simple_calculator* ins);
		static void op_desc(simple_calculator* ins);
		static void op_funcs(simple_calculator* ins);
		static void op__push_locals(simple_calculator* ins);
		static void op__pop_locals(simple_calculator* ins);

	private:
		void show_help(char* name);
		void parse_arguments(int argc, char** argv);

		void execute();
		void evaluate();
		void ensure_pop_locals();
		number_t resolve_variable_if(const element_t& e);

		void parse(std::string_view what);
		void file(std::string_view what);
		void file(std::istream& is);
		void repl();

	public:
		simple_calculator(int argc, char** argv)
		{
			parse_arguments(argc, argv);
		}
	};
};
