
#include "randutils.hpp"
#include <iostream>
#include <omp.h>
#include <ostream>
#define NUM_THREADS 8

const char* USAGE =
	"Usage: ./matrix [-h] <nb_threads> <dim>\n"
	"  -h			: Print this help message and exit\n"
	"  nb_threads	: Number of threads \n"
	"  dim			: Dimension used for matrices \n";

#define error(...) \
std::cerr << "[error] " << __VA_ARGS__ << std::endl; \
std::cerr << USAGE << std::endl; \
exit(1); \

void gaussian_v1_seq(int n, long long** A, long* b, long* x)
{
	int row = 0, col = 0;

	for (row = n-1; row >= 0; row--) {
		x[row] = b[row];
		for (col = row+1; col < n; col++) {
			x[row] -= A[row][col] * x[col];
		}
		x[row] /= A[row][row];
	}
}


void gaussian_v2_seq(int n, long long** A, long* b, long* x)
{
	int row = 0, col = 0;

	for (row = 0; row < n; row++) {
		x[row] = b[row];
	}
	for (col = n-1; col >= 0; col--) {
		x[col] /= A[col][col];
		for (row = 0; row < col; row++) {
			x[row] -= A[row][col] * x[col];
		}
	}
}

void gaussian_v1_par(int n, long long** A, long* b, long* x)
{
	int row = 0, col = 0;

	#pragma omp parallel for num_threads(NUM_THREADS) \
		private(row, col) \
		shared(A, b, x)
	{
		for (row = n-1; row >= 0; row--) {
			x[row] = b[row];
			for (col = row+1; col < n; col++) {
				x[row] -= A[row][col] * x[col];
			}

			x[row] /= A[row][row];
		}
	}
}

void gaussian_v2_par(int n, long long** A, long* b, long* x)
{
	int row = 0, col = 0;

	for (row = 0; row < n; row++) {
		x[row] = b[row];
	}

	#pragma omp parallel for num_threads(NUM_THREADS) \
		private(row, col) \
		shared(A, b, x)
	for (col = n-1; col >= 0; col--) {
		x[col] /= A[col][col];

		for (row = 0; row < col; row++) {
			x[row] -= A[row][col] * x[col];
		}
	}
}

int main(int argc, char* argv[])
{
	int		dim;
	int 	nb_threads;
	char	dummy;

	if (argc != 3 || sscanf(argv[2], "%d%c", &dim, &dummy) != 1)
	{
		error("invalid argument, expected dimension (integer).");
	}else if (argc != 3 || sscanf(argv[1], "%d%c", &nb_threads, &dummy) != 1)
	{
		error("invalid argument, expected threads' number.");
	}

	std::cout << "[info] dimensions: " << dim << std::endl;
	std::cout << "[info] number of threads: " << nb_threads << std::endl;

	std::cout << std::endl;

	std::vector<long long> A_data(dim * dim);
	long long** A = new long long*[dim];
	for (int i = 0; i < dim; i++) {
		A[i] = &A_data[i * dim];
	}

	double mx_start = omp_get_wtime();

	// Create a single random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<long long> dis(1, 10000);

	#pragma omp parallel num_threads(nb_threads)
	{
		// Each thread gets its own generator seeded differently
		std::mt19937 local_gen(rd() + omp_get_thread_num());
		std::uniform_int_distribution<long long> local_dis(1, 10000);

		#pragma omp for collapse(2)
		for (int i = 0; i < dim; i++) {
			for (int j = 0; j < dim; j++) {
				A[i][j] = local_dis(local_gen);
			}
		}
	}
	double mx_end = omp_get_wtime() - mx_start;
	std::cout << "[info] matrix: " << mx_end << "s" << std::endl ;
	std::cout.flush();

	long* b = new long[dim];

	std::vector<omp_sched_t> schedules = {omp_sched_static, omp_sched_dynamic, omp_sched_guided, omp_sched_auto};
	std::vector<std::string> schedule_names = {"static", "dynamic", "guided", "auto"};

	for (size_t i = 0; i < schedules.size(); ++i) 
	{
		omp_set_schedule(schedules[i], 0);
		std::cout << std::endl << "[schedule] " << schedule_names[i] << std::endl;

		double gv1_seq_start = omp_get_wtime();
		long* x = new long[dim];
		gaussian_v1_seq(dim, A, b, x);
		double gv1_seq_end = omp_get_wtime() - gv1_seq_start;
		std::cout << "[info] v1 seq: " << gv1_seq_end << "s" << std::endl ;
		std::cout.flush();

		double gv2_seq_start = omp_get_wtime();
		x = new long[dim];
		gaussian_v2_seq(dim, A, b, x);
		double gv2_seq_end = omp_get_wtime() - gv2_seq_start;
		std::cout << "[info] v2 seq: " << gv2_seq_end << "s" << std::endl ;
		std::cout.flush();

		double gv1_par_start = omp_get_wtime();
		x = new long[dim];
		gaussian_v1_par(dim, A, b, x);
		double gv1_par_end = omp_get_wtime() - gv1_par_start;
		std::cout << "[info] v1 par: " << gv1_par_end << "s" << std::endl ;
		std::cout.flush();

		double gv2_par_start = omp_get_wtime();
		x = new long[dim];
		gaussian_v2_par(dim, A, b, x);
		double gv2_par_end = omp_get_wtime() - gv2_par_start;
		std::cout << "[info] v2 par: " << gv2_par_end << "s" << std::endl ;
		std::cout.flush();

		delete[] x;
	}

	return 0;
}
