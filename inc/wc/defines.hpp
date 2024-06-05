#pragma once

#define WC_EXCEPTION(type, ...) {						\
		auto msg = std::format(__VA_ARGS__);			\
		throw wc::exception(msg, wc::error_type::type);	\
	}

#define WC_STD_EXCEPTION(...) {					\
		auto msg = std::format(__VA_ARGS__);	\
		throw std::runtime_error(msg);			\
	}
