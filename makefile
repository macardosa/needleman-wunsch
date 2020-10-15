FC=gcc
target=align
obj=$(patsubst %.c,%.o,$(wildcard *.c))

all: $(target) clean

%.o:%.c
	$(FC) -c $<

$(target):$(obj)
	$(FC) -o $@ $^

test:
	./$(target) s1.fasta s2.fasta

clean:
	rm *.o

install:
	cp $(target) ~/.bin
