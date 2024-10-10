SOURCES  = queue.c rwlock.c
OBJECTS  = $(SOURCES:.c=.o)

CC       = clang
FORMAT   = clang-format

CFLAGS   = -Wall -Wpedantic -Werror -Wextra -g

.PHONY: all clean format

all: $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) *.fmt

format: $(SOURCES:.c=.fmt)

%.fmt: %.c
	$(FORMAT) -i $<
	touch $@
