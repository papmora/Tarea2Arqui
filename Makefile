
.PHONY: all clean

all: sum_array

sum_array: sum_array.c
	mpicc -o $@ $^ -Wall -fopenmp

run: sum_array
	mpirun -np 2 -hosts master,worker1 ./sum_array (edited)

clean:
	rm -f sum_array files/sum_array_*.txt plot.html