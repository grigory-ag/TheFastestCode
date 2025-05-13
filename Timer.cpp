#include <iostream>
#include <ctime>

static clock_t timer_start;

// Функции для использования в других программах
extern "C" {
    void start_timer() {
        timer_start = clock();
    }
    
    double stop_timer() {
        clock_t timer_end = clock();
        return static_cast<double>(timer_end - timer_start) / CLOCKS_PER_SEC;
    }
}

// Отдельная программа для тестирования таймера
#ifdef TEST_TIMER
int main() {
    start_timer();
    // Тестовая нагрузка
    volatile double sum = 0;
    for(int i = 0; i < 1'000'000; i++) {
        sum += i * 0.1;
    }
    double elapsed = stop_timer();
    
    std::cout << "Elapsed time: " << elapsed << "s\n";
    std::cout << "I am FASTEEER!" << std::endl;
    return 0;
}
#endif