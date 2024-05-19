#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <string_view>
#include <cstring>
#include <exception>
#include <regex>
#include <stack>
#include <array>

class SimpleCalculator
{
public:
	using number_t = long long;
	enum class operator_t
	{
		add, subtract,
		multiply, divide
	};
	union element_t {
		number_t n;
		operator_t o;
	};

private:
	static constexpr std::array<std::string_view, 4> operations {
		"+", "-", "*", "/"
	};
	static constexpr std::array<unsigned, 4> operand_size {
		1, 1, 1, 1
	};
	
	std::stack<std::pair<element_t, bool>> elements;
	
private:
	void show_help(char* name)
	{
		std::cerr << name << ": Arbitrary length calculator" << std::endl
				  << "\t-h, --help: Show this" << std::endl
				  << "\t-e, --expr=[EXPRESSION]: Calculates EXPRESSION and quits"
				  << std::endl;
	}
	
	void parse_arguments(int argc, char** argv)
	{
		for (int i=1; i < argc; i++)
		{
			if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				show_help(argv[0]);
				throw std::runtime_error("");
			}
			
			else if (strncmp(argv[i], "--expr", 2+4) == 0 || strncmp(argv[i], "-e", 1+1) == 0)
			{
				std::regex reg(R"(--expr=(.+))"), reg2(R"(-e=(.+))");
				std::cmatch match;
				if (std::regex_match(argv[i], match, reg) || std::regex_match(argv[i], match, reg2))
				{
					auto e = match[1].str();
					expr(e);
				}
				else
				{
					throw std::runtime_error("Please supply an expression to calculate");
				}
			}

			else
			{
				std::ostringstream oss;
				oss << "Unknown argument: '" << argv[i] << "'";
				show_help(argv[0]);
				throw std::invalid_argument(oss.str());
			}
		}
	}

	void perform_operation(operator_t operation, std::stack<number_t>& operands, number_t& result)
	{
		auto o = operands.top();
		operands.pop();
		switch (operation)
		{
		case operator_t::add: result += o;
			break;
		case operator_t::subtract: result -= o;
			break;
		case operator_t::multiply: result *= o;
			break;
		case operator_t::divide:
			if (o == 0)
				throw std::runtime_error("Cannot divide by 0");
			result /= o;
			break;
		}
	}

	void reverse_polish()
	{
		number_t result = 0;

		while (!elements.empty())
		{
			auto e = elements.top();
			elements.pop();

			if (!e.second)
			{
				int i = static_cast<int>(e.first.o);
				if (elements.size() < operand_size[i])
				{
					std::ostringstream oss;
					oss << "Operation '" << operations[i] << "' requires "
						<< operand_size[i] << " elements but only "
						<< elements.size() << " are left";
					throw std::runtime_error(oss.str());
				}
				else
				{
					std::stack<number_t> operands;
					for (unsigned j=0; j < operand_size[i]; j++)
					{
						auto e = elements.top();
						elements.pop();
						
						if (!e.second)
						{
							std::ostringstream oss;
							oss << "Unexpected operation '" << operations[static_cast<int>(e.first.o)]
								<< "' while processing current operation '" << operations[i]
								<< "'";
							throw std::runtime_error(oss.str());
						}
						else
						{
							operands.push(e.first.n);
						}
					}
					
					perform_operation(e.first.o, operands, result);
				}
			}
		}
		
		std::cout << "Result: " << result << std::endl;
	}

public:
	SimpleCalculator(int argc, char** argv)
	{
		parse_arguments(argc, argv);
	}

	void expr(std::string_view e)
	{
		bool okay = true;
		
		std::vector<std::string> sub;
		{
			std::string tmp;
			for (char c : e)
			{
				if (isspace(c))
				{
					if (!tmp.empty())
						sub.push_back(std::move(tmp));
				}
				else
				{
					tmp += c;
				}
			}
			if (!tmp.empty())
				sub.push_back(std::move(tmp));
		}

		for (const auto& s : sub)
		{
			okay = true;
			auto elem {std::make_pair(static_cast<element_t>(0), false)};
			
			elem.second = false;
			bool is_op = false;
			for (unsigned i=0; i < operations.size(); i++)
			{
				if (s == operations[i])
				{
					if (i >= 0 && i < 4)
					{
						elem.first.o = static_cast<operator_t>(i);
						is_op = true;
						break;
					}
				}
			}

			if (!is_op)
			{
				try
				{
					elem.second = true;
					elem.first.n = std::stoll(s);
				}
				catch (const std::out_of_range& e) {
					okay = false;
				}
				catch (const std::invalid_argument& e) {
					okay = false;
				}
			}

			if (okay)
				elements.push(std::move(elem));
			else
			{
				std::ostringstream oss;
				oss << "Garbage input: '" << s << "'";
				throw std::invalid_argument(oss.str());
			}
		}

		reverse_polish();
	}

	void repl()
	{
	}
};

int main(int argc, char** argv)
{
	try
	{
		SimpleCalculator sc(argc, argv);
		sc.repl();
	}
	catch (const std::exception& e)
	{
		if (e.what()[0] != '\0')
			std::cerr << "Fatal exception: " << e.what() << std::endl;
		return 1;
	}
}
