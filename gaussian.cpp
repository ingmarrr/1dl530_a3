
#define NUM_THREADS 8

void gaussian_v1_seq(int n, long** A, long* b, long* x)
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


void gaussian_v2_seq(int n, long** A, long* b, long* x)
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

void gaussian_v1_par(int n, long** A, long* b, long* x)
{
	int row = 0, col = 0;

	#pragma omp parallel for num_threads(NUM_THREADS) \
		private(row, col) \
		shared(A, b, x)
	for (row = n-1; row >= 0; row--) {
		x[row] = b[row];
		for (col = row+1; col < n; col++) {
			x[row] -= A[row][col] * x[col];
		}

		/* #pragma omp single */
		{
			x[row] /= A[row][row];
		}
	}
}

void gaussian_v2_par(int n, long** A, long* b, long* x)
{
	int row = 0, col = 0;

	for (row = 0; row < n; row++) {
		x[row] = b[row];
	}

	#pragma omp parallel for num_threads(NUM_THREADS)
	for (col = n-1; col >= 0; col--) {
		x[col] /= A[col][col];
		for (row = 0; row < col; row++) {
			x[row] -= A[row][col] * x[col];
		}
	}
}

int main(int argc, char* argv[])
{
	return 0;
}
