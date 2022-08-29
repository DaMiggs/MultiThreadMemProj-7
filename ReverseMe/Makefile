CC = gcc
CFLAGS = -I. -g -w -pthread -lm
OBJ = reverse.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

reverse: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
		rm -rf reverse
		rm -rf *.o

x:
		make
		./reverse -input /Users/miguel/Desktop/essay.txt

t:
		make
		./reverse -input /Users/miguel/Desktop/essay.txt -nthreads 16 -mem 2000

z:
		make
		./reverse -input /Users/miguel/Desktop/first.txt

y:
		make
		./reverse -input /Users/miguel/Desktop/first.txt -nthreads 20 -mem 200

n:
		make
		./reverse -input /Users/miguel/Desktop/input.txt

m:
		make
		./reverse -input /Users/miguel/Desktop/input.txt -nthreads 10 -mem 25

