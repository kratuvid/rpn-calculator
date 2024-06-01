#include "wc.hpp"
#include "arbit.hpp"

#include <random>
#include <chrono>

int main(int argc, char** argv)
{
	try
	{
		[[maybe_unused]] const auto max_i = std::numeric_limits<int>::max(), min_i = std::numeric_limits<int>::min();
		[[maybe_unused]] const auto max_ll = std::numeric_limits<long long>::max();
		[[maybe_unused]] const auto max_u = std::numeric_limits<unsigned>::max();

		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<int> dist0(min_i, max_i), dist_loop(10, 20), dist1(min_i, max_i);

		const char print_way = 'x';

		int loop_max = 1e5;
		for (int i=0; i < loop_max; i++)
		{
			std::list<int> s0, s1;
			for (int j=0; j < dist_loop(engine); j++)
				s0.push_back(dist0(engine));
			for (int j=0; j < dist_loop(engine); j++)
				s1.push_back(dist1(engine));

			[[maybe_unused]] auto t0 = {0x53u}, t1 = {0xffffffc9, 0xffffffdc};

			wc::arbit n0(s0, {});
			wc::arbit n1(s1, {});

			const auto first = *s1.begin();

			// n0.raw_print(print_way);
			// std::print(" * ");
			// n1.raw_print(print_way);
			// std::print(" = ");
			// nr.raw_print(print_way, 1);
			
			bool neg = n0.is_negative();
			bool neg_rhs = first < 0;
			bool expected = neg ^ neg_rhs;

			n0 *= first;

			bool got = n0.is_negative();
			if (got != expected)
			{
				std::println("{} * {} = {}", neg, neg_rhs, got);
				n0.raw_print(print_way);
				std::print(" <> {:#x}", (wc::arbit::base_t)first);
				// nr.raw_print(print_way, 1);
			
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
				std::println(" <> {:#x}", (wc::arbit::base_t)first);
				// nr.raw_print(print_way, 1);

				std::cout << '\r';
				std::cout << (n0.bytes() / 4) << " <> 0x" << std::hex << (wc::arbit::base_t)first << std::dec << " ~ ";
				// std::cout << " = " << (nr.bytes() / 4) << " ~ ";
				std::cout << (i / double(loop_max)) * 100 << "%...";
				std::cout.flush();
			}
		}
		std::cout << std::endl;
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

	std::println("Arbit heap statistics: Max: {}B sitting on {} entries, current: {}B. "
				 "Mallocs: {}, reallocs: {}, frees: {}",
				 wc::arbit::max_heap(), wc::arbit::max_entries_heap(), wc::arbit::net_heap(),
				 wc::arbit::mallocs_heap(), wc::arbit::reallocs_heap(), wc::arbit::frees_heap());

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
