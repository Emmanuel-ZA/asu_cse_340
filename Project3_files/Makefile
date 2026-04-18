CC = g++
CFLAGS = -Wall -Wextra -g

a.out: lexer.cc inputbuf.cc compiler.cc demo.cc
	$(CC) $(CFLAGS) -o a.out lexer.cc inputbuf.cc compiler.cc demo.cc

clean:
	rm -f a.out
