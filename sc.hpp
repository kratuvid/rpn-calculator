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

#include <readline/readline.h>
#include <readline/history.h>

#include "utility.hpp"

namespace sc
{
	class simple_calculator
	{
	public:
		enum class operand_type { number, string };
		using number_t = long double;
		using element_t = std::any;
		using operation_func_t = void (*) (simple_calculator*);
		using operation_t = std::tuple<std::string, std::vector<operand_type>, operation_func_t>;

	private:
		const std::array<operation_t, 19> operations {{
				{"+", {operand_type::number, operand_type::number}, op_add},
				{"-", {operand_type::number, operand_type::number}, op_subtract},
				{"*", {operand_type::number, operand_type::number}, op_multiply},
				{"/", {operand_type::number, operand_type::number}, op_divide},
				{"^", {operand_type::number, operand_type::number}, op_power},

				{"replace", {operand_type::number, operand_type::number}, op_replace},
				{"swap", {operand_type::number, operand_type::number}, op_swap},
				{"pop", {operand_type::number}, op_pop},
				{"top", {operand_type::number}, op_top},

				{"neg", {operand_type::number}, op_neg},
				{"sin", {operand_type::number}, op_sin}, {"cos", {operand_type::number}, op_cos},
				{"floor", {operand_type::number}, op_floor}, {"ceil", {operand_type::number}, op_ceil},

				{"help", {}, op_help}, {"stack", {}, op_stack}, {"quit", {}, op_quit},
				{"clear", {}, op_clear}, {"file", {operand_type::string}, op_file},
		}};

		std::vector<element_t> stack;
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

	private:
		void show_help(char* name);
		void parse_arguments(int argc, char** argv);

		void perform_operation(const operation_t* op);
		void evaluate();

		void expr(std::string_view what);
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
