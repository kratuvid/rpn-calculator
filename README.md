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
- [ ] find out a way to prevent copy in operator-= and other cases
- [ ] implement decimal at all sorts of places
- [ ] fix shrink_if_can