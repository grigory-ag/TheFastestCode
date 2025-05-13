#include <iostream>
#include <fstream>
#include <vector>
#include <cblas.h>
#include <immintrin.h>
#include <omp.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <thread>

extern "C" {
    void start_timer();
    double stop_timer();
}

double* alloc_aligned(size_t size) {
    return static_cast<double*>(_mm_malloc(size * sizeof(double), 32));
}

bool read_matrix(const std::string& filename, int& N, double*& matrix) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    iss >> N;

    matrix = alloc_aligned(N * N);
    for (int i = 0; i < N * N; ++i) {
        if (!(file >> matrix[i])) return false;
    }

    file.close();
    return true;
}

int main() {
    // Оптимизация количества потоков OpenBLAS
    openblas_set_num_threads(std::thread::hardware_concurrency());

    int N;
    double *A = nullptr, *B = nullptr;

    // Чтение матриц
    if (!read_matrix("M1.dat", N, A) || !read_matrix("M2.dat", N, B)) {
        std::cerr << "Error reading matrix files!" << std::endl;
        return 1;
    }

    // Выделение памяти для C
    double* C = alloc_aligned(N * N);
    #pragma omp parallel for
    for (int i = 0; i < N * N; ++i) {
        C[i] = 0.0;
    }

    // Выполнение умножения
    start_timer();
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                N, N, N, 1.0, A, N, B, N, 0.0, C, N);
    double elapsed = stop_timer();

    // Вывод результатов
    std::cout << "Elapsed time: " << elapsed << "s\n";
    std::cout << "Performance: " << (2.0 * N * N * N) / (elapsed * 1e9) << " GFLOPS\n";
    std::cout << "I am FASTEEER!" << std::endl;

    // Сохраняем результат
    std::ofstream out("M3.dat");
    out << N << " // Matrix Order\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            out << C[i * N + j] << " ";
        }
        out << "\n";
    }

    // Очистка
    _mm_free(A);
    _mm_free(B);
    _mm_free(C);

    return 0;
}