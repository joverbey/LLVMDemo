CC = g++
CFLAGS = -Wall -O3 `llvm-config --cxxflags`
INCLUDES =
LFLAGS = `llvm-config --ldflags --system-libs --libs core`
LIBS = 
SRCS = $(wildcard *.cc)
HEADERS = $(wildcard *.h)
OBJS = parser.yy.o lexer.yy.o $(SRCS:.cc=.o) runtime.o
MAIN = vlang

.PHONY: all clean test

all: $(MAIN)

$(OBJS): $(HEADERS) parser.y lexer.l

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

parser.yy.o: parser.y
	yacc -o parser.yy.cc -d parser.y
	$(CC) -O3 -c parser.yy.cc -o $@
	rm -f parser.yy.cc

lexer.yy.o: lexer.l
	lex -o lexer.yy.cc lexer.l
	$(CC) -O3 -c lexer.yy.cc -o $@
	rm -rf lexer.yy.cc

runtime.o: runtime.c
	$(CC) -c runtime.c

test: $(MAIN)
	./$(MAIN) <example.input 2>output.ll
	llc -o output.s output.ll
	as -o output.o output.s
	gcc runtime.c output.o
	./a.out

clean:
	$(RM) *.o *.yy.cc *.yy.h *.yy.hh *~ $(MAIN) output.* a.out
