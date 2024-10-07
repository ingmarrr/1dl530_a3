#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <omp.h>
#include <time.h>

#include "range.hpp"


const char* USAGE =
	"Usage: ./sieve [-h] <nb_threads> <max>\n"
	"  -h			: Print this help message and exit\n"
	"  nb_threads	: Number of threads \n"
	"  max			: Max number \n";

#define error(...) \
std::cerr << "[error] " << __VA_ARGS__ << std::endl; \
std::cerr << USAGE << std::endl; \
exit(1); \

pthread_mutex_t lock;
omp_lock_t lock1;


auto sieve_seq_mark(std::vector<bool>& composites, int max) -> void
{
	for (auto k = 2; k*k < max; ++k)
	{
		if (!composites[k])
		{
			for (auto a = k*k; a <= max; a += k)
			{
				composites[a] = true;
			}
		}
	}
}

auto sieve_par_seeds(const std::vector<bool>& composites, int sqrt_max) -> std::vector<int>
{
	auto out = std::vector<int>();
	for (auto i : irange(2, sqrt_max))
	{
		if (!composites[i])
		{
			out.push_back(i);
		}
	}

	return out;
}


struct ThreadArgs {
	int start;
	int end;
	std::vector<bool>* composites;
	const std::vector<int>* seeds;
};

void* sieve_par_mark(void* arg)
{
	ThreadArgs* args = static_cast<ThreadArgs*>(arg);
	for (auto prime : *(args->seeds))
	{

		auto first = std::max(prime*prime, (args->start+prime-1)/prime*prime);
		pthread_mutex_lock(&lock);
		for (auto a = first; a <= args->end; a += prime)
		{	
			
			(*(args->composites))[a] = true;
			
		}
		pthread_mutex_unlock(&lock);
	}
	return nullptr;
	pthread_exit(0);
}

auto sieve_par(int max, int n_threads) -> std::vector<uint64_t>
{
	auto composites = std::vector<bool>(max + 1, false);
	auto sqrt_max   = static_cast<int>(std::sqrt(max));

	sieve_seq_mark(composites, max);

	auto seeds      = sieve_par_seeds(composites, sqrt_max);
	auto chunk_size = (max - sqrt_max) / n_threads;
	auto workers    = std::vector<pthread_t>(n_threads);
	auto args       = std::vector<ThreadArgs>(n_threads);

	for (int tid : erange(0, n_threads))
	{
		int start	= sqrt_max+1+tid*chunk_size;
		int nums	= tid == n_threads-1 ? max-start+1 : chunk_size;
		auto end    = start+nums;
		args[tid] = {start, end, &composites, &seeds};
		pthread_create(&workers[tid], nullptr, sieve_par_mark, &args[tid]);
	}

	for (auto& worker : workers)
	{
		pthread_join(worker, nullptr);
	}

	auto out    = std::vector<uint64_t>();
	for (auto ix : irange(2, max))
	{
		if (!composites[ix])
		{
			out.push_back(ix);
		}
	}

	return out;
}

auto sieve_par_omp(int max, int n_threads) -> std::vector<uint64_t>
{
	auto composites = std::vector<bool>(max + 1, false);
	auto sqrt_max   = static_cast<int>(std::sqrt(max));

	sieve_seq_mark(composites, max);

	auto seeds      = sieve_par_seeds(composites, sqrt_max);
	auto chunk_size = (max - sqrt_max) / n_threads;

	omp_set_num_threads(n_threads);
	#pragma omp parallel for shared(composites, seeds) firstprivate(sqrt_max, chunk_size)
	for (int tid = 0; tid < n_threads; tid++)
	{
		int start	= sqrt_max+1+tid*chunk_size;
		int nums	= tid == n_threads-1 ? max-start+1 : chunk_size;
		auto end    = start+nums;

		for (auto prime : seeds)
		{
			auto first = std::max(prime*prime, (start+prime-1)/prime*prime);
			omp_set_lock(&lock1);
			for (auto a = first; a <= end; a += prime)
			{	
				composites[a] = true;
			}
			omp_unset_lock(&lock1);
		}
	}

	auto out    = std::vector<uint64_t>();
	for (auto ix : irange(2, max))
	{
		if (!composites[ix])
		{
			out.push_back(ix);
		}
	}

	return out;
}

auto sieve_seq(int max) -> std::vector<uint64_t>
{
	auto composites = std::vector<bool>(max+1, false);

	for (auto k = 2; k*k < max; ++k)
	{
		if (!composites[k])
		{
			for (auto a = k*k; a <= max; a += k)
			{
				composites[a] = true;
			}
		}
	}

	std::vector<uint64_t> out{};
	for (auto ix : irange(2, max))
	{
		if (!composites[ix])
		{
			out.push_back(ix);
		}
	}

	return out;
}


auto main(int argc, char* argv[]) -> int
{
	int		max;
	int 	nb_threads;
	char	dummy;
	pthread_mutex_init(&lock, NULL);
	omp_init_lock(&lock1);

	if (argc != 3 || sscanf(argv[2], "%d%c", &max, &dummy) != 1)
	{
		error("invalid argument, expected max (integer).");
	}else if (argc != 3 || sscanf(argv[1], "%d%c", &nb_threads, &dummy) != 1)
	{
		error("invalid argument, expected number of threads (integer).");
	}

	std::cout << std::endl;
	std::cout << "[info] running ./sieve with "<< nb_threads << " threads and " << max << " max integer" << std::endl;

	double par_start = omp_get_wtime();
	auto p_res	= sieve_par(max, nb_threads);
	double par_run_time = omp_get_wtime() - par_start;

	// for (auto ix : erange(0, p_res.size()))
	// {
	// 	std::cout << " [par_posix]: " << p_res[ix] << ",";
	// }
	// std::cout << std::endl << std::endl;
	std::cout <<"[info] The elapsed par_POSIX time is: " << par_run_time << " seconds" << std::endl << std::endl;


	std::cout << "[info] running ./sieve with "<< nb_threads << " threads and " << max << " max integer" << std::endl;

	double par_omp_start = omp_get_wtime();
	auto p_omp_res	= sieve_par_omp(max, nb_threads);
	double par_omp_run_time = omp_get_wtime() - par_omp_start;

	// for (auto ix : erange(0, p_omp_res.size()))
	// {
	// 	std::cout << " [par_omp]: " << p_omp_res[ix] << ",";
	// }
	// std::cout << std::endl << std::endl;
	std::cout <<"[info] The elapsed par_OpenMP time is: " << par_omp_run_time << " seconds" << std::endl << std::endl;


	std::cout << "[info] running ./sieve with one thread and " << max << " max integer" << std::endl;

	double seq_start = omp_get_wtime();
	auto s_res	= sieve_seq(max);
	double seq_run_time = omp_get_wtime() - seq_start;

	// for (auto ix : erange(0, s_res.size()))
	// {
	// 	std::cout << " [seq]: " << s_res[ix] << ",";
	// }
	// std::cout << std::endl;
	std::cout <<"[info] The elapsed seq time is: " << seq_run_time << " seconds" << std::endl;

	return 0;
}
