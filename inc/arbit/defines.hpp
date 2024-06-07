#pragma once

#define ARBIT_EXCEPTION(type, ...)										\
	throw arbit::exception::generate(std::source_location::current(), arbit::error_type::type, __VA_ARGS__)
