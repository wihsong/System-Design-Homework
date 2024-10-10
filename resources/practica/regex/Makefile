EXECBIN  = memory_parsing
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:%.c=%.o)
FORMATS  = $(SOURCES:%.c=%.fmt)
HELPER = ../../asgn2/starter_code/x86/asgn2_helper_funcs.a

CC       = clang
FORMAT   = clang-format
CFLAGS   = -Wall -Wpedantic -Werror -Wextra -g

.PHONY: all clean format

all: $(EXECBIN)

$(EXECBIN): $(OBJECTS) $(HELPER)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXECBIN) $(OBJECTS)

format: $(FORMATS)

%.fmt: %.c
	$(FORMAT) -i $<
	touch $@


