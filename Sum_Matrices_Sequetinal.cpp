#include <iostream>
#include <fstream>
#include <immintrin.h>
#include <ctime>
#include <limits>

using namespace std; // Добавлено пространство имён

extern "C" {
    void start_timer();
    double stop_timer();
}

int main() {
    int N;
    double *M1, *M2, *M3;
    
    // Исправлено: явное указание std::ifstream
    ifstream M1file("M1.dat");
    M1file >> N;
    M1file.ignore(numeric_limits<streamsize>::max(), '\n');
    M1 = static_cast<double*>(_mm_malloc(N*N*sizeof(double), 32));
    for (int i = 0; i < N*N; ++i) {
        if (!(M1file >> M1[i])) {
            cerr << "Error reading M1 at index " << i << endl;
            _mm_free(M1);
            return 1;
        }
    }
    M1file.close();

    // Исправлено: объявление M2file
    ifstream M2file("M2.dat");
    M2file >> N;
    M2file.ignore(numeric_limits<streamsize>::max(), '\n');
    M2 = static_cast<double*>(_mm_malloc(N*N*sizeof(double), 32));
    for (int i = 0; i < N*N; ++i) {
        if (!(M2file >> M2[i])) {
            cerr << "Error reading M2 at index " << i << endl;
            _mm_free(M2);
            return 1;
        }
    }
    M2file.close();

    M3 = static_cast<double*>(_mm_malloc(N*N*sizeof(double), 32));

    start_timer();
    
    const int prefetch_offset = 64;
    int i;
    for (i = 0; i < N*N - 15; i += 16) {
        _mm_prefetch(M1 + i + prefetch_offset, _MM_HINT_T0);
        _mm_prefetch(M2 + i + prefetch_offset, _MM_HINT_T0);
        
        _mm256_store_pd(M3+i,   _mm256_add_pd(_mm256_load_pd(M1+i),   _mm256_load_pd(M2+i)));
        _mm256_store_pd(M3+i+4, _mm256_add_pd(_mm256_load_pd(M1+i+4), _mm256_load_pd(M2+i+4)));
        _mm256_store_pd(M3+i+8, _mm256_add_pd(_mm256_load_pd(M1+i+8), _mm256_load_pd(M2+i+8)));
        _mm256_store_pd(M3+i+12,_mm256_add_pd(_mm256_load_pd(M1+i+12), _mm256_load_pd(M2+i+12)));
    }
    
    for (; i < N*N; ++i) {
        M3[i] = M1[i] + M2[i];
    }
    
    double elapsed = stop_timer();

    cout << "Elapsed time: " << elapsed << "s\n";
    cout << "I am FASTEEER!" << endl;

    // Исправлено: объявление Sav
    ofstream Sav("M3.dat");
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