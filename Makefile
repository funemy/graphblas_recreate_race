CFLAGS = -D_GNU_SOURCE -std=gnu99 -fopenmp -g -ggdb -O1

GRAPHBLAS=../deps/GraphBLAS/build/libgraphblas.a

CC=clang

all:
	$(CC) $(CFLAGS) -o recreate_race recreate_race.c $(GRAPHBLAS) -lpthread -lm -lomp

clean:
	rm recreate_race
