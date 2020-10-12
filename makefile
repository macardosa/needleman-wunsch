FC=gcc
target=align
obj=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(target) clean

%.o:%.c
	$(FC) -c $<

$(target):$(obj)
	$(FC) -o $@ $^

test:
	./$(target)

clean:
	rm *.o

install:
	cp $(target) ~/.bin
