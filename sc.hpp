#pragma once

#include <iostream>
#include <string>
#include <sstream>
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

#include <readline/readline.h>

#include "utility.hpp"

namespace sc
{
	class simple_calculator
	{
	public:
		using number_t = long double;
		enum class operator_t
		{
			add, subtract, multiply, divide, power,
			stack, current, quit, set, clear,
			zero, one,
			help,
			sin, cos,
			floor, ceil,
			_length
		};
		union element_t {
			number_t n;
			operator_t o;
		};

	private:
		static constexpr size_t operations_size = 17;
		static constexpr std::array<std::string_view, operations_size> operations {
			"+", "-", "*", "/", "^",
			"stack", "current", "quit", "set", "clear",
			"zero", "one",
			"help",
			"sin", "cos",
			"floor", "ceil"
		};
		static constexpr std::array<unsigned, operations_size> operand_size {
			1, 1, 1, 1, 1,
			0, 0, 0, 1, 0,
			0, 0,
			0,
			0, 0,
			0, 0
		};
	
		std::vector<std::pair<element_t, bool>> elements;
		number_t current = 0;
		bool verbose = false;

	private:
		void show_help(char* name);	
		void parse_arguments(int argc, char** argv);
		
		void perform_operation(operator_t operation, std::stack<number_t>& operands);
		void evaluate();
		
		void expr(std::string_view what);
		void repl();
	
	public:
		simple_calculator(int argc, char** argv)
		{
			static_assert(operations_size == static_cast<int>(operator_t::_length));
			parse_arguments(argc, argv);
		}
	};
};
