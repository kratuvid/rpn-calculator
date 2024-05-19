#include "sc.hpp"

namespace sc
{
	void simple_calculator::show_help(char* name)
	{
		std::cerr << name << ": Arbitrary length calculator" << std::endl
				  << "\t-h, --help: Show this" << std::endl
				  << "\t-e, --expr=[EXPRESSION]: Calculates EXPRESSION and quits" << std::endl
				  << "\t-r, --repl: Start the REPL"
				  << std::endl;
	}

	void simple_calculator::parse_arguments(int argc, char** argv)
	{
		std::list<std::string> list_expr;
		bool is_repl = false;
			
		for (int i=1; i < argc; i++)
		{
			if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				show_help(argv[0]);
				throw sc::exception("", sc::error_type::init_help);
			}
			
			else if (strncmp(argv[i], "--expr", 2+4) == 0 || strncmp(argv[i], "-e", 1+1) == 0)
			{
				std::regex reg(R"(--expr=(.+))"), reg2(R"(-e=(.+))");
				std::cmatch match;
				if (std::regex_match(argv[i], match, reg) || std::regex_match(argv[i], match, reg2))
				{
					auto e = match[1].str();
					list_expr.push_back(e);
				}
				else
				{
					throw sc::exception("Please supply an expression to calculate", sc::error_type::init);
				}
			}

			else if (strcmp(argv[i], "--repl") == 0 || strcmp(argv[i], "-r") == 0)
			{
				is_repl = true;
			}
			
			else
			{
				std::ostringstream oss;
				oss << "Unknown argument: '" << argv[i] << "'";
				show_help(argv[0]);
				throw sc::exception(oss.str(), sc::error_type::init);
			}
		}

		for (const auto& what : list_expr)
			expr(what);
		std::cout << current << std::endl;

		if (is_repl || argc == 1)
			repl();
	}

	void simple_calculator::perform_operation(operator_t operation, std::stack<number_t>& operands)
	{
		switch (operation)
		{
		case operator_t::add: {			
			auto o = operands.top();
			operands.pop();
			current += o;
		}
			break;
			
		case operator_t::subtract: {
			auto o = operands.top();
			operands.pop();
			current -= o;
		}
			break;
			
		case operator_t::multiply: {
			auto o = operands.top();
			operands.pop();
			current *= o;
		}
			break;
			
		case operator_t::divide: {
			auto o = operands.top();
			operands.pop();
			if (o == 0)
				throw sc::exception("Cannot divide by 0", sc::error_type::expr_divide_by_zero);
			current /= o;
		}
			break;

		case operator_t::stack: {
			std::cout << "Stack size: " << elements.size() << std::endl;
			for (int i = elements.size()-1; i >= 0; i--)
			{
				const auto& e = elements[i];
				if (e.second)
					std::cout << "N: " << e.first.n;
				else
					std::cout << "O: " << operations[static_cast<int>(e.first.o)];
				std::cout << std::endl;
			}
		}
			break;
			
		case operator_t::current: {
			std::cout << current << std::endl;
		}
			break;
			
		case operator_t::quit: {
			throw sc::exception("", sc::error_type::repl_quit);
		}
			break;
			
		case operator_t::set: {
			auto o = operands.top();
			operands.pop();
			current = o;
		}
			break;
			
		case operator_t::clear: {
			elements.clear();
		}
			break;
			
		case operator_t::zero: {
			current = 0.0l;
		}
			break;
			
		case operator_t::one: {
			current = 1.0l;
		}
			break;
			
		case operator_t::help: {
			std::cerr << R"(Operation: operand size: description:
-------------------------------------
+: 1: current = current + operand
-: 1: current = current - operand
*: 1: current = current * operand
/: 1: current = current / operand
---
sin: 1: current = sin(current)
cos: 1: current = cos(current)
floor: 0: current = floor(current)
ceil: 0: current = ceil(current)
---
zero: 0: current = 0
one: 0: current = 1
---
stack: 0: list the stack
current: 0: display the current value
set: 1: current = operand
clear: 0: empty the stack
quit: 0: quit the REPL
---
help: 0: show this screen)" << std::endl;
		}
			break;
			
		case operator_t::sin: {
			current = std::sin(current);
		}
			break;
			
		case operator_t::cos: {
			current = std::cos(current);
		}
			break;
			
		case operator_t::floor: {
			current = std::floor(current);
		}
			break;
			
		case operator_t::ceil: {
			current = std::ceil(current);
		}
			break;
		}
	}

	void simple_calculator::evaluate()
	{
		size_t i = 0;
		auto og_size = elements.size();
		while (i < og_size)
		{
			if (elements.size() > 0 && !elements.back().second)
			{
				auto e = std::move(elements.back());
				elements.pop_back();
			
				int op_i = static_cast<int>(e.first.o);
				if (elements.size() < operand_size[op_i])
				{
					std::ostringstream oss;
					oss << "Operation '" << operations[op_i] << "' requires "
						<< operand_size[op_i] << " elements but only "
						<< elements.size() << " are left";
					throw sc::exception(oss.str(), sc::error_type::expr);
				}
				else
				{
					std::stack<number_t> operands;
					for (unsigned j=0; j < operand_size[op_i]; j++)
					{
						auto e = std::move(elements.back());
						elements.pop_back();
						i++;
						
						if (!e.second)
						{
							std::ostringstream oss;
							oss << "Unexpected operation '" << operations[static_cast<int>(e.first.o)]
								<< "' while processing current operation '" << operations[op_i]
								<< "'";
							throw sc::exception(oss.str(), sc::error_type::expr);
						}
						else
						{
							operands.push(e.first.n);
						}
					}
					
					perform_operation(e.first.o, operands);
				}
			}

			else
			{
			}

			i++;
		}
	}

	void simple_calculator::expr(std::string_view what)
	{
		std::vector<std::string> subs;
		{
			std::string tmp;
			for (char c : what)
			{
				if (isspace(c))
				{
					if (!tmp.empty())
						subs.push_back(std::move(tmp));
				}
				else
				{
					tmp += c;
				}
			}
			if (!tmp.empty())
				subs.push_back(std::move(tmp));
		}

		bool okay = true;
		for (const auto& sub : subs)
		{
			okay = true;
			auto elem {std::make_pair(static_cast<element_t>(0), false)};
			
			elem.second = false;
			bool is_op = false;
			for (unsigned i=0; i < operations.size(); i++)
			{
				if (sub == operations[i])
				{
					elem.first.o = static_cast<operator_t>(i);
					is_op = true;
					break;
				}
			}

			if (!is_op)
			{
				try
				{
					elem.second = true;
					elem.first.n = std::stold(sub);
				}
				catch (const std::out_of_range& e) {
					okay = false;
				}
				catch (const std::invalid_argument& e) {
					okay = false;
				}
			}

			if (okay)
				elements.push_back(std::move(elem));
			else
			{
				std::ostringstream oss; 
				oss << "Garbage sub-expression: '" << sub << "'";
				throw sc::exception(oss.str(), sc::error_type::expr);
			}
		}

		evaluate();
	}
	
	void simple_calculator::repl()
	{
	  begin:
		try
		{
			while (true)
			{
				if (false)
				{					
					std::string what;

					std::cerr << "> ";
					std::getline(std::cin, what);
					
					expr(what);
				}
				else
				{
					char* what = nullptr;
					try {
						what = readline("> ");
						if (!what) throw sc::exception("", sc::error_type::repl_quit);
							
						expr(what);
					} catch (...) {
						if (what) free(what);
						throw;
					}
					if (what) free(what);
				}
				
				std::cout << current << std::endl;
			}
		}
		catch (const sc::exception& e)
		{
			std::ostringstream oss;
			oss << "Error: ";
			oss << sc::error_type_str[static_cast<int>(e.type)] << ": ";
			oss << e.what();
			
			switch (e.type)
			{
			case sc::error_type::expr_divide_by_zero:
			case sc::error_type::expr:
				break;
			case sc::error_type::repl_quit:
				return;
			default:
				throw;
			}
			
			std::cerr << oss.str() << std::endl;
			
			goto begin;
		}
	}	
}; // namespace sc
