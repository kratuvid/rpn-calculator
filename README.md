# Wtf Calculator
Another RPN calculator

# Arguments
```
Wtf Calculator: Another RPN calculator
	-h, --help: Show this
	-e, --expr [EXPRESSION]: Calculates EXPRESSION
	-f, --file [FILE]: Read expressions from FILE
	-s, --stdin: Read expression from standard input until EOF
	-r, --repl: Start the REPL
	-p, --prefix: Use prefix notation
	-t, --time: Show runtime
	-v, --verbose: Be verbose
```

# Todo
- [ ] optimized larger loops
- [ ] impl division and modulo
- [ ] parse decimals
- [ ] new constructor that asks the precision to take until. for example until when the repeating 0.2 (in binary) be parsed in
- [ ] generalise arbit(float) constructor to any base_t size
- [ ] add round function
- [ ] convert to float
- [ ] build if headers change
- [ ] custom memory allocator
- [x] use std::allocator? - no
- [x] replace std::any with std::variant
- [x] make wc a module - compiler error - not working - wait for better compiler support?
- [x] separate arbit from namespace wc
- [x] separate main.cpp from wc
