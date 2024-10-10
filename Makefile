#
# # example Makefile to build an executable named myprog from myprog.cpp
# #
SIEVE=sieve

MATRIX=matrix
NB_THREADS=512
MAX=100000000	# used for ex. 1
DIM=1000		# used for ex. 3

GAUSSIAN=gaussian
DIM=42000
NB_THREADS=8
G_ARGS=$(NB_THREADS) $(DIM)

ARGS=$(G_ARGS)
PROG=$(GAUSSIAN)
CC=clang++

# given the number of threads as the second arg and number of trepezes as the third
run: build
	@./bin/$(PROG) $(NB_THREADS) $(NB_TRAPS)

build: $(PROG).cpp
	@$(CC) -std=c++11 -Wall -fopenmp $(PROG).cpp -o bin/$(PROG)

build-run: $(PROG).cpp
	@$(CC) -std=c++11 -Wall -fopenmp $(PROG).cpp -o bin/$(PROG)
	@./bin/$(PROG) $(ARGS)

clean:
	@rm -rf bin
	@mkdir bin

run-win: build-win
	@./$(PROG).exe $(NB_THREADS) $(NB_TRAPS)

build-win: $(PROG).cpp
	@g++ -std=c++11 -Wall -fopenmp $(PROG).cpp -o $(PROG)

build-run-win: $(PROG).cpp
	@g++ -std=c++11 -Wall -fopenmp $(PROG).cpp -o $(PROG)
	@set OMP_NUM_THREADS=$(NB_THREADS)
	@./$(PROG).exe $(NB_THREADS) $(DIM)


