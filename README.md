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
- [ ] arbitrary precision numbers
- [ ] fixed and decimal numbers
- [ ] optimized larger loops
- [ ] implement decimal at all sorts of places
- [x] check if move cons is being called at relevant places
- [x] fix shrink_if_can
- [x] find out a way to prevent copy in operator-= and other cases - not possible
- [x] add support for operations directly with integers?
- [x] shrink on product
