import <list>;
import <random>;
import <chrono>;
import <limits>;

import arbit;

#include "wc/wc.hpp"

int main(int argc, char** argv)
{
	try
	{
		using base_t = arbit::base_t;

		[[maybe_unused]] const auto max_i = std::numeric_limits<int>::max(), min_i = std::numeric_limits<int>::min();
		[[maybe_unused]] const auto max_ll = std::numeric_limits<long long>::max();
		[[maybe_unused]] const auto max_u = std::numeric_limits<unsigned>::max();

		std::random_device rd;
		std::mt19937 eng(rd());

		const char way = 'x';

		if (true)
		{
			float a, b;

			std::print("Enter 'a': "); std::cin >> a;
			std::print("Enter 'b': "); std::cin >> b;

			arbit na(a), nb(b);
			auto s = na + nb;
			auto d = na - nb;
			auto p = na * nb;
			std::println("{}: '{}'\n\t'{}'\n", a, na.raw_format(way), (-na).raw_format(way));
			std::println("{}: '{}'\n\t'{}'\n", b, nb.raw_format(way), (-nb).raw_format(way));
			std::println("+: '{}'\n\t'{}'\n", s.raw_format(way), (-s).raw_format(way));
			std::println("-: '{}'\n\t'{}'\n", d.raw_format(way), (-d).raw_format(way));
			std::println("*: '{}'\n\t'{}'\n", p.raw_format(way), (-p).raw_format(way));
		}
		else if (true)
		{
			std::uniform_real_distribution<float> dist(-1, 1);
			std::uniform_int_distribution<unsigned> dist_mul(0, 1e9);

			float a = 0.f, b = 1.f, c = -1.f, d = 1024.f, e = 0.00000000314159f, f = -4e-39f;
			arbit na(a), nb(b), nc(c), nd(d), ne(e), nf(f);
			std::println("{}: '{}'\n\t'{}'\n", a, na.raw_format(way), (-na).raw_format(way));
			std::println("{}: '{}'\n\t'{}'\n", b, nb.raw_format(way), (-nb).raw_format(way));
			std::println("{}: '{}'\n\t'{}'\n", c, nc.raw_format(way), (-nc).raw_format(way));
			std::println("{}: '{}'\n\t'{}'\n", d, nd.raw_format(way), (-nd).raw_format(way));
			std::println("{}: '{}'\n\t'{}'\n", e, ne.raw_format(way), (-ne).raw_format(way));
			std::println("{}: '{}'\n\t'{}'\n", f, nf.raw_format(way), (-nf).raw_format(way));

			std::println("---");
			for (int i=0; i < 20; i++)
			{
				float g = dist(eng) * dist_mul(eng) / 1e20f;
				arbit ng(g);
				std::println("{}: '{}'\n\t'{}'\n", g, ng.raw_format(way), (-ng).raw_format(way));
			}
		}
		else if (true)
		{
			arbit na(0u, 0b00110011001100110011001 << 9), nb(2u);

			for (int i=0; i < 30; i++)
			{
				auto p = na * nb;

				std::println("\r{}", i);
				std::println("---\n'{}'\n\t'{}'", na.raw_format(way), nb.raw_format(way));
				std::println("'{}'\n\t'{}'\n", (-na).raw_format(way), (-nb).raw_format(way));
				std::println("*:\n'{}'", p.raw_format(way));
				std::println("'{}'\n", (-p).raw_format(way));

				na = p;
			}
		}
		else if (false)
		{
			std::uniform_int_distribution<base_t> dist(0, ~unsigned(0)/3), dist_decimal(0, ~base_t(0)), dist_bool(0, 1), dist_items(1, 2);

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

				arbit na(sa.begin(), sa.size(), sad.begin(), sad.size()),
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
					std::println("\r---\n'{}'\n\t'{}'", na.raw_format(way), nb.raw_format(way));
					std::println("'{}'\n\t'{}'\n", (-na).raw_format(way), (-nb).raw_format(way));
					std::println("+:\n'{}'", s.raw_format(way));
					std::println("'{}'\n", (-s).raw_format(way));
					std::println("-:\n'{}'", d.raw_format(way));
					std::println("'{}'\n", (-d).raw_format(way));
					std::println("*:\n'{}'", p.raw_format(way));
					std::println("'{}'", (-p).raw_format(way));
					WC_STD_EXCEPTION("No!");
				}

				static auto tp_then = std::chrono::high_resolution_clock::now();
				const auto tp_now = std::chrono::high_resolution_clock::now();
				const auto tp_diff = std::chrono::duration_cast<std::chrono::milliseconds>(tp_now - tp_then).count();
				if (tp_diff > 1000)
				{
					tp_then = tp_now;

					std::println("\r---\n'{}'\n\t'{}'", na.raw_format(way), nb.raw_format(way));
					std::println("'{}'\n\t'{}'\n", (-na).raw_format(way), (-nb).raw_format(way));
					std::println("+:\n'{}'", s.raw_format(way));
					std::println("'{}'\n", (-s).raw_format(way));
					std::println("-:\n'{}'", d.raw_format(way));
					std::println("'{}'\n", (-d).raw_format(way));
					std::println("*:\n'{}'", p.raw_format(way));
					std::println("'{}'", (-p).raw_format(way));
					std::println("\n{}%...\n", i * 100.0 / loop_max);
				}
			}
		}

		/*
		else if (false)
		{
			std::string a, b;
			std::cout << "Provide two numbers to add, subtract and multiply: \n";
			std::cin >> a >> b;

			arbit na(a), nb(b);

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
				std::vector<arbit::base_t> s0, s1;
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

				arbit n0(s0.begin(), s0.size(), s0.end(), 0);
				arbit n1(s1.begin(), s0.size(), s1.end(), 0);

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
	catch (arbit::exception& e)
	{
		std::println("Fatal arbit exception: {}: {}",
					 arbit::error_type_str[static_cast<int>(e.type)],
					 e.what());
	}
	catch (std::exception& e)
	{
		std::println("Fatal standard exception: {}", e.what());
	}

	auto cons = arbit::stats.get_cons();
	auto heap = arbit::stats.get_heap();
	std::println("# Arbit:\n"
				 "Heap: max: {}B sitting on {} entries, current: {}B, "
				 "mallocs: {}, reallocs: {}, frees: {}\n"
				 "Constructors: copy: {}, move: {}, normal: {}",
				 heap.max, heap.max_entries, heap.current,
				 heap.mallocs, heap.reallocs, heap.frees,
				 cons.copy, cons.move, cons.normal);

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
