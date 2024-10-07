#
# # example Makefile to build an executable named myprog from myprog.cpp
# #
SIEVE=sieve
MATRIX=matrix
PROG=$(MATRIX)
NB_THREADS=512
MAX=100000000	# used for ex. 1
DIM=1000		# used for ex. 3

# given the number of threads as the second arg and number of trepezes as the third
run: build
	@./bin/$(PROG) $(NB_THREADS) $(NB_TRAPS)

build: $(PROG).cpp
	@g++ -std=c++11 -Wall -fopenmp $(PROG).cpp -o bin/$(PROG)

build-run: $(PROG).cpp
	@g++ -std=c++11 -Wall -fopenmp $(PROG).cpp -o bin/$(PROG)
	@./bin/$(PROG) $(NB_THREADS) $(NB_TRAPS)

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


