GCC = gcc
PROG = pxdfview
CFLAGS = -Wall --pedantic
LIBS   = -lncurses

all: $(PROG)

%: %.c
	$(GCC) $(CFLAGS) $< -o $@ $(LIBS)

clean:
	rm *~ $(PROG)
