#include <iostream>
#include <algorithm>
#include <cstring>
//#include <ctime>

long long int vec[100][20000];
int cache_hits = 0;

long long Ackerman(long long m, long long n) {
	if (m >= 100 || n >= 20000) {
		return -1;
	}

	if (vec[m][n] != -1) {
		cache_hits++;
		return vec[m][n];
	}

	long long int result;
	if (m == 0) {
		result = n + 1;
	}
	else if (n == 0) {
		result = Ackerman(m - 1, 1);
	}
	else {
		long long int inner_result = Ackerman(m, n - 1);
		result = Ackerman(m - 1, inner_result);
	}
	return vec[m][n] = result;
}

int main() {
	memset(vec, -1, sizeof(vec));
	//clock_t t0 = clock();
	int ans = Ackerman(3, 11);
	//clock_t t1 = clock();
	//double ms = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
	std::cout << "Ackerman = " << ans << " CACHE = " << cache_hits << std::endl;  //<< " (" << ms << "ms" << ")"
}