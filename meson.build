project('wtf-calculator', 'cpp', default_options: ['cpp_std=c++23'])
# add_project_arguments('-Wno-shift-count-overflow', language: 'cpp')

executable('wc', 'main.cpp', 'operations.cpp', 'wc.cpp', 'arbit.cpp', dependencies: dependency('readline'))
executable('mul', 'tests/mul.cpp')
executable('fp', 'tests/fp.cpp')
