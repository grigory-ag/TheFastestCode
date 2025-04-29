#include <iostream>
#include <ctime>

using namespace std;

int main() {
    clock_t start = clock();
    clock_t end = clock();

    cout << "Elapsed: " << (double)(end - start) / CLOCKS_PER_SEC << "s\n";
    cout << "I am FASTEEER!" << endl;

    return 0;
}
