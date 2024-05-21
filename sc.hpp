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
		using operation_t = std::pair<std::string, std::vector<operand_type>>;

	private:
		const std::array<operation_t, 19> operations {{
				    // 5: 0 -> 4
				{"+", {operand_type::number, operand_type::number}},
				{"-", {operand_type::number, operand_type::number}},
				{"*", {operand_type::number, operand_type::number}},
				{"/", {operand_type::number, operand_type::number}},
				{"^", {operand_type::number, operand_type::number}},

				    // 4: 5 -> 8
				{"replace", {operand_type::number, operand_type::number}},
				{"swap", {operand_type::number, operand_type::number}},
				{"pop", {operand_type::number}}, {"top", {operand_type::number}},

					// 5: 9 -> 13
				{"neg", {operand_type::number}},
				{"sin", {operand_type::number}}, {"cos", {operand_type::number}},
				{"floor", {operand_type::number}}, {"ceil", {operand_type::number}},

					// 5: 14 -> 18
				{"help", {}}, {"stack", {}}, {"quit", {}}, {"clear", {}},
				{"file", {operand_type::string}},
		}};

		std::vector<element_t> stack;
		bool verbose = false;

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
