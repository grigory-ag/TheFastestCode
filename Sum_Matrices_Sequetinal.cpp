#include <iostream>
#include <fstream>
#include <immintrin.h>
#include <ctime>
#include <limits>

using namespace std;

#define freq CLOCKS_PER_SEC

int main() {
    int N;
    double *M1, *M2, *M3;
    
    // Read matrix M1
    ifstream M1file("M1.dat");
    M1file >> N;
    M1file.ignore(numeric_limits<streamsize>::max(), '\n');  // Skip comment line
    M1 = static_cast<double*>(_mm_malloc(N*N*sizeof(double), 32));
    for (int i = 0; i < N*N; ++i) 
        if (!(M1file >> M1[i])) {
            cerr << "Error reading M1 at index " << i << endl;
            _mm_free(M1);
            return 1;
        }
    M1file.close();

    // Read matrix M2
    ifstream M2file("M2.dat");
    M2file >> N;
    M2file.ignore(numeric_limits<streamsize>::max(), '\n');  // Skip comment line
    M2 = static_cast<double*>(_mm_malloc(N*N*sizeof(double), 32));
    for (int i = 0; i < N*N; ++i)
        if (!(M2file >> M2[i])) {
            cerr << "Error reading M2 at index " << i << endl;
            _mm_free(M2);
            return 1;
        }
    M2file.close();

    // Allocate memory for result
    M3 = static_cast<double*>(_mm_malloc(N*N*sizeof(double), 32));

    // AVX2 computation with prefetching
    clock_t start = clock();
    const int prefetch_offset = 64;  // Optimal for Zen 3 architecture
    int i;
    for (i = 0; i < N*N - 15; i += 16) {
        // Prefetch next data block
        _mm_prefetch(M1 + i + prefetch_offset, _MM_HINT_T0);
        _mm_prefetch(M2 + i + prefetch_offset, _MM_HINT_T0);
        
        // Process 16 elements per iteration
        _mm256_store_pd(M3+i,   _mm256_add_pd(_mm256_load_pd(M1+i),   _mm256_load_pd(M2+i)));
        _mm256_store_pd(M3+i+4, _mm256_add_pd(_mm256_load_pd(M1+i+4), _mm256_load_pd(M2+i+4)));
        _mm256_store_pd(M3+i+8, _mm256_add_pd(_mm256_load_pd(M1+i+8), _mm256_load_pd(M2+i+8)));
        _mm256_store_pd(M3+i+12,_mm256_add_pd(_mm256_load_pd(M1+i+12), _mm256_load_pd(M2+i+12)));
    }
    
    // Process remaining elements
    for (; i < N*N; ++i) {
        M3[i] = M1[i] + M2[i];
    }
    clock_t end = clock();
    
    cout << "Elapsed time: " << (double)(end - start)/freq << "s\n";
    cout << "I am FASTEEER!" << endl;

    // Save result to M3.bat
    ofstream Sav("M3.bat");
    Sav << N << " // Matrix Order\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            Sav << M3[i*N + j] << " ";
        }
        Sav << "\n";
    }
    Sav.close();

    _mm_free(M1);
    _mm_free(M2);
    _mm_free(M3);
    return 0;
}