SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))

.PHONY: all clean

all: ad7190.so

ad7190.so: $(OBJS)
	gcc $^ -shared -o $@

%.o: %.c *.h
	gcc -fPIC -I. -c $< -o $@

clean:
	rm -f *.so *.o
