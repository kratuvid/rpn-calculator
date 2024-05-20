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
#include <readline/history.h>

#include "utility.hpp"

namespace sc
{
	class simple_calculator
	{
	public:
		using number_t = double;
		enum class operator_t
		{
			add, subtract, multiply, divide, power,
			stack, quit, replace, swap, pop, clear,
			neg,
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
			"stack", "quit", "replace", "swap", "pop", "clear",
			"neg",
			"help",
			"sin", "cos",
			"floor", "ceil"
		};
		static constexpr std::array<unsigned, operations_size> operand_size {
			2, 2, 2, 2, 2,
			0, 0, 2, 2, 1, 0,
			1,
			0,
			1, 1,
			1, 1
		};
	
		std::vector<std::pair<element_t, bool>> stack;
		bool verbose = false;

	private:
		void show_help(char* name);	
		void parse_arguments(int argc, char** argv);
		
		void perform_operation(operator_t operation);
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
