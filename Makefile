#
# # example Makefile to build and run the programs from Assignment 3
# #
SIEVE=sieve
MAX=100000000	
SIEVE_ARGS=$(NB_THREADS) $(MAX) # used for ex. 1

GOF=Game_Of_Life
SIZE=4096
STEPS=2000
GOF_ARGS=$(SIZE) $(STEPS) # used for ex. 2

MATRIX=matrix
DIM=1000		
MATRIX_ARGS=$(NB_THREADS) $(DIM) # used for ex. 3

GAUSSIAN=gaussian
DIM=42000		
GAUSSIAN_ARGS=$(NB_THREADS) $(DIM) # used for ex. 4

ARGS=$(GAUSSIAN_ARGS)
PROG=$(GAUSSIAN)
NB_THREADS=64
CC=clang++

# given the number of threads as the second arg and number of trepezes as the third
run: build
	@./bin/$(PROG) $(NB_THREADS) $(NB_TRAPS)

build: $(PROG).cpp
	@$(CC) -std=c++11 -Wall -fopenmp $(PROG).cpp -o bin/$(PROG)
	@export OMP_NUM_THREADS=$(NB_THREADS)

build-run: $(PROG).cpp
	@$(CC) -std=c++11 -Wall -fopenmp $(PROG).cpp -o bin/$(PROG)
	@./bin/$(PROG) $(ARGS)

clean:
	@rm -rf bin
	@mkdir bin


run-win: build-win
	@./$(PROG).exe $(CC_GOF)

build-win: $(PROG).cpp
	@g++ -std=c++11 -Wall -fopenmp $(PROG).cpp -o $(PROG)
	@set OMP_NUM_THREADS=$(NB_THREADS)

build-run-win: $(PROG).cpp
	@g++ -std=c++11 -Wall -fopenmp $(PROG).cpp -o $(PROG)
	@set OMP_NUM_THREADS=$(NB_THREADS)
	@./$(PROG).exe $(ARGS)