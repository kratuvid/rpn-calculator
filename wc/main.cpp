#include "wc.hpp"
#include "arbit.hpp"

#include <list>
#include <random>
#include <chrono>

int main(int argc, char** argv)
{
	try
	{
		using base_t = wc::arbit::base_t;
		
		[[maybe_unused]] const auto max_i = std::numeric_limits<int>::max(), min_i = std::numeric_limits<int>::min();
		[[maybe_unused]] const auto max_ll = std::numeric_limits<long long>::max();
		[[maybe_unused]] const auto max_u = std::numeric_limits<unsigned>::max();

		std::random_device rd;
		std::mt19937 eng(rd());
		
		const char way = 'x';

		if (true)
		{
			std::uniform_int_distribution<base_t> dist(0, 30), dist_decimal(0, ~base_t(0)), dist_bool(0, 1), dist_items(1, 1);

			const int loop_max = 1e5;
			for (int i=0; i < loop_max; i++)
			{
				std::list<base_t> sa, sad, sb, sbd;
				for (int i=0; i < (int)dist_items(eng); i++)
					sa.push_back(dist(eng));
				for (int i=0; i < (int)dist_items(eng); i++)
					sad.push_back(dist_decimal(eng));
				for (int i=0; i < (int)dist_items(eng); i++)
					sb.push_back(dist(eng));
				for (int i=0; i < (int)dist_items(eng); i++)
					sbd.push_back(dist_decimal(eng));
				
				wc::arbit na(sa.begin(), sa.size(), sad.begin(), sad.size()),
					nb(sb.begin(), sb.size(), sbd.begin(), sbd.size());
				if (dist_bool(eng)) na.negate();
				if (dist_bool(eng)) nb.negate();

				auto s = na + nb;
				auto d = na - nb;
				auto p = na * nb;

				const bool neg = na.is_negative(), neg_rhs = nb.is_negative();
				const bool expected = neg ^ neg_rhs, got = p.is_negative();
				if (expected != got && !(na.is_zero() || nb.is_zero()))
				{
					std::println("\r'{}' & '{}'", na.raw_format(way), nb.raw_format(way));
					std::println("Negated: '{}' & '{}'", (-na).raw_format(way), (-nb).raw_format(way));
					std::println("Sum: '{}'", s.raw_format(way));
					std::println("Sum negated: '{}'", (-s).raw_format(way));
					std::println("Difference: '{}'", d.raw_format(way));
					std::println("Difference negated: '{}'", (-d).raw_format(way));
					std::println("Product: '{}'", p.raw_format(way));
					std::println("Product negated: '{}'", (-p).raw_format(way));
					WC_STD_EXCEPTION("No!");
				}

				static auto tp_then = std::chrono::high_resolution_clock::now();
				const auto tp_now = std::chrono::high_resolution_clock::now();
				const auto tp_diff = std::chrono::duration_cast<std::chrono::milliseconds>(tp_now - tp_then).count();
				if (tp_diff > 1000)
				{
					tp_then = tp_now;

					std::println("\r'{}' & '{}'", na.raw_format(way), nb.raw_format(way));
					std::println("Negated: '{}' & '{}'", (-na).raw_format(way), (-nb).raw_format(way));
					std::println("Sum: '{}'", s.raw_format(way));
					std::println("Sum negated: '{}'", (-s).raw_format(way));
					std::println("Difference: '{}'", d.raw_format(way));
					std::println("Difference negated: '{}'", (-d).raw_format(way));
					std::println("Product: '{}'", p.raw_format(way));
					std::println("Product negated: '{}'", (-p).raw_format(way));
					std::println("{}%...\n", i * 100.0 / loop_max);
				}
			}
		}

		/*
		else if (false)
		{
			std::string a, b;
			std::cout << "Provide two numbers to add, subtract and multiply: \n";
			std::cin >> a >> b;

			wc::arbit na(a), nb(b);

			auto s = na + nb;
			auto d = na - nb;
			auto p = na * nb;

			na.raw_print(print_way); std::cout << " {+,-,*} "; nb.raw_print(print_way); std::cout << " = \n";
			s.raw_print(print_way, 1);
			d.raw_print(print_way, 1);
			p.raw_print(print_way, 1);
		}
		else if (true)
		{
			std::uniform_int_distribution<int> dist0(-1e3, 1e3), dist_loop(1, 2), dist1(-1e3, 1e3);

			const int loop_max = 1e6;
			for (int i=0; i < loop_max; i++)
			{
				const auto s0l = dist_loop(engine), s1l = dist_loop(engine);
				std::vector<wc::arbit::base_t> s0, s1;
				while ((int)s0.size() < s0l)
				{
					auto n = dist0(engine);
					if (n == 0)
						continue;
					s0.push_back(n);
				}
				while ((int)s1.size() < s1l)
				{
					auto n = dist1(engine);
					if (n == 0)
						continue;
					s1.push_back(n);
				}

				wc::arbit n0(s0.begin(), s0.size(), s0.end(), 0);
				wc::arbit n1(s1.begin(), s0.size(), s1.end(), 0);

				bool neg = n0.is_negative();
				bool neg_rhs = n1.is_negative();
				bool expected = neg ^ neg_rhs;

				const auto nr = n0 * n1;

				bool got = nr.is_negative();
				if (got != expected)
				{
					n0.raw_print(print_way);
					std::print(" * ");
					n1.raw_print(print_way);
					std::print(" = ");
					nr.raw_print(print_way, 1);
				
					WC_STD_EXCEPTION("No!");
				}

				static auto tp_last = std::chrono::high_resolution_clock::now();
				const auto tp_now = std::chrono::high_resolution_clock::now();
				const auto tp_diff = std::chrono::duration_cast<std::chrono::milliseconds>(tp_now - tp_last).count();
				if (tp_diff > 1000)
				{
					tp_last = tp_now;
					
					std::cout << '\r';
					n0.raw_print(print_way);
					std::print(" * ");
					n1.raw_print(print_way);
					std::print(" = ");
					nr.raw_print(print_way, 1);

					std::cout << '\r';
					std::cout << (n0.bytes() / 4) << " * " << (n1.bytes() / 4);
					std::cout << " = " << (nr.bytes() / 4) << " ~ ";
					std::cout << (i / double(loop_max)) * 100 << "%...";
					std::cout.flush();
				}
			}
			std::cout << std::endl;
		}
		*/
	}
	catch (wc::arbit::exception& e)
	{
		std::println("Fatal arbit exception: {}: {}",
					 wc::arbit::error_type_str[static_cast<int>(e.type)],
					 e.what());
	}
	catch (std::exception& e)
	{
		std::println("Fatal standard exception: {}", e.what());
	}

	auto cons = wc::arbit::stats.get_cons();
	auto heap = wc::arbit::stats.get_heap();
	std::println("# Arbit:\n"
				 "Heap: max: {}B sitting on {} entries, current: {}B, "
				 "mallocs: {}, reallocs: {}, frees: {}\n"
				 "Constructors: copy: {}, move: {}, parse: {}, bare: {}, list: {}",
				 heap.max, heap.max_entries, heap.current,
				 heap.mallocs, heap.reallocs, heap.frees,
				 cons.copy, cons.move, cons.parse, cons.bare, cons.list);

	return 10;

	wc::wtf_calculator app;
	try
	{
		app.start(argc, argv);
	}
	catch (const wc::exception& e)
	{
		if (e.type == wc::error_type::init_help)
			return 0;

		std::println(stderr, "Fatal exception: {}: {}",
					 wc::error_type_str[static_cast<int>(e.type)], e.what());
		return 2;
	}
	catch (const std::exception& e)
	{
		std::println(stderr, "Fatal standard exception: {}", e.what());
		return 1;
	}
}
