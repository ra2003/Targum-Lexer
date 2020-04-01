#CC = clang-6.0
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -flto -s -O2
TFLAGS = -Wall -Wextra -pedantic -std=c99 -flto -g -O2
# -fsanitize=undefined 

LIB_NAME = targum_lexer
DEPS = -ldl

SRCS = stringobj/stringobj.c
SRCS += vector/vector.c
SRCS += map/map.c
SRCS += linkmap/linkmap.c
SRCS += variant/variant.c
SRCS += cfg/cfg.c
SRCS += lex/lex.c
SRCS += targum_err.c
SRCS += targum_lexer.c

OBJS = $(SRCS:.c=.o)

targum_static:
	+$(MAKE) -C stringobj
	+$(MAKE) -C vector
	+$(MAKE) -C map
	+$(MAKE) -C linkmap
	+$(MAKE) -C variant
	+$(MAKE) -C cfg
	+$(MAKE) -C lex
	$(CC) $(CFLAGS) -c $(SRCS)
	gcc-ar rcs lib$(LIB_NAME).a $(OBJS)

debug:
	+$(MAKE) -C stringobj debug
	+$(MAKE) -C vector debug
	+$(MAKE) -C map debug
	+$(MAKE) -C linkmap debug
	+$(MAKE) -C variant debug
	+$(MAKE) -C cfg debug
	+$(MAKE) -C lex debug
	$(CC) $(TFLAGS) -c $(SRCS)
	gcc-ar rcs lib$(LIB_NAME).a $(OBJS)

test:
	$(CC) $(TFLAGS) test_driver.c -L. -l$(LIB_NAME) -o test_driver -ldl -Wl,--export-dynamic

clean:
	+$(MAKE) -C stringobj clean
	+$(MAKE) -C vector clean
	+$(MAKE) -C map clean
	+$(MAKE) -C linkmap clean
	+$(MAKE) -C variant clean
	+$(MAKE) -C cfg clean
	+$(MAKE) -C lex clean
	$(RM) *.o
