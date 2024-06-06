project('wtf-calculator', 'cpp', default_options: ['cpp_std=c++23'])
# add_project_arguments('-Wno-shift-count-overflow', language: 'cpp')

executable(
	'wc',
	'wc/main.cpp', 'wc/operations.cpp', 'wc/wc.cpp',
	'arbit/basic.cpp', 'arbit/bits.cpp', 'arbit/construction.cpp', 'arbit/heap.cpp', 'arbit/operations.cpp',
	dependencies: dependency('readline')
)
executable('mul', 'tests/mul.cpp')
executable('fp', 'tests/fp.cpp')
executable('floats', 'tests/floats.cpp')