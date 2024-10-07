#include <omp.h>
#include <bits/stdc++.h>
#include "randutils.hpp"

const char* USAGE =
	"Usage: ./matrix [-h] <nb_threads> <dim>\n"
	"  -h			: Print this help message and exit\n"
	"  nb_threads	: Number of threads \n"
	"  dim			: Dimension used for matrices \n";

#define error(...) \
std::cerr << "[error] " << __VA_ARGS__ << std::endl; \
std::cerr << USAGE << std::endl; \
exit(1); \

void mulMat(int threads, long long **mat1, long long **mat2, int dim, int flag)
{
    long long** c = new long long*[dim];
 
    for (int i = 0; i < dim; i++) 
    {
        c[i] = new long long[dim];
    }

    if (flag == 0)
    {
        std::cout << "[info] running ./matrix with one thread and matrix dimension as " << dim << std::endl;
        std::cout << "Multiplication of the given two matrices is:\n";

        for (int i = 0; i < dim; i++) 
        {
            for (int j = 0; j < dim; j++) 
            {
                c[i][j] = 0;

                for (int k = 0; k < dim; k++) 
                {
                    c[i][j] += mat1[i][k] * mat2[k][j];
                }

                //std::cout << c[i][j] << "\t";
            }

            //std::cout << std::endl;
        }
    }else if (flag == 1)
    {
        std::cout << "[info] running ./matrix with "<< threads << " threads and matrix dimension as " << dim << std::endl;
        std::cout << "Multiplication of the given two matrices is:\n";

        #pragma omp parallel for shared(mat1, mat2, c, dim)
        for (int i = 0; i < dim; i++) 
        {
            for (int j = 0; j < dim; j++) 
            {
                c[i][j] = 0;

                for (int k = 0; k < dim; k++) 
                {
                    c[i][j] += mat1[i][k] * mat2[k][j];
                }

                //std::cout << c[i][j] << "\t";
            }

            //std::cout << std::endl;
        }
    }else if (flag == 2)
    {
        std::cout << "[info] running ./matrix with "<< threads << " threads and matrix dimension as " << dim << std::endl;
        std::cout << "Multiplication of the given two matrices is:\n";

        #pragma omp parallel for collapse(2) shared(mat1, mat2, c, dim)
        for (int i = 0; i < dim; i++) 
        {
            for (int j = 0; j < dim; j++) 
            {
                c[i][j] = 0;

                for (int k = 0; k < dim; k++) 
                {
                    c[i][j] += mat1[i][k] * mat2[k][j];
                }

                //std::cout << c[i][j] << "\t";
            }

            //std::cout << std::endl;
        }
    }else if (flag == 3)
    {
        std::cout << "[info] running ./matrix with "<< threads << " threads and matrix dimension as " << dim << std::endl;
        std::cout << "Multiplication of the given two matrices is:\n";

        #pragma omp parallel for collapse(2) shared(mat1, mat2, c, dim)
        for (int i = 0; i < dim; i++) 
        {
            for (int j = 0; j < dim; j++) 
            {
                c[i][j] = 0;

                #pragma omp parallel for
                for (int k = 0; k < dim; k++) 
                {
                    c[i][j] += mat1[i][k] * mat2[k][j];
                }

                //std::cout << c[i][j] << "\t";
            }

            //std::cout << std::endl;
        }
    }

    std::cout << std::endl;
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

	std::cout << std::endl;

    long long** a = new long long*[dim];
    long long** b = new long long*[dim];
 
    for (int i = 0; i < dim; i++) 
    {
        a[i] = new long long[dim];
        b[i] = new long long[dim];
    }
 
    // Traverse the 2D array
    for (int i = 0; i < dim; i++) 
    {
        for (int j = 0; j < dim; j++) 
        {
            randutils::mt19937_rng rng;
            a[i][j] = rng.uniform(1, 10000);
            b[i][j] = rng.uniform(1, 10000);
        }
    }
    
    std::cout << "First matrix: " << std::endl;
    // for (int i = 0; i < dim; i++) 
    // {
    //     for (int j = 0; j < dim; j++) 
    //     {
    //         std::cout << a[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }
    std::cout << std::endl;

    std::cout << "Second matrix: " << std::endl;
    // for (int i = 0; i < dim; i++) 
    // {
    //     for (int j = 0; j < dim; j++) 
    //     {
    //         std::cout << b[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }
    std::cout << std::endl;

    double seq_start = omp_get_wtime();
    mulMat(nb_threads, a, b, dim, 0);
    double seq_run_time = omp_get_wtime() - seq_start;
    std::cout <<"[info] The elapsed seq time is: " << seq_run_time << " seconds" << std::endl << std::endl;
    
    double par_i_start = omp_get_wtime();
    mulMat(nb_threads, a, b, dim, 1);
    double par_i_run_time = omp_get_wtime() - par_i_start;
    std::cout <<"[info] The elapsed par_i time is: " << par_i_run_time << " seconds" << std::endl;
    
    double par_ii_start = omp_get_wtime();
    mulMat(nb_threads, a, b, dim, 2);
    double par_ii_run_time = omp_get_wtime() - par_ii_start;
    std::cout <<"[info] The elapsed par_ii time is: " << par_ii_run_time << " seconds" << std::endl;
    
    double par_iii_start = omp_get_wtime();
    mulMat(nb_threads, a, b, dim, 3);
    double par_iii_run_time = omp_get_wtime() - par_iii_start;
    std::cout <<"[info] The elapsed par_iii time is: " << par_iii_run_time << " seconds" << std::endl;

    return 0;
}