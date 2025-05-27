#include <iostream>

struct Complex {
    double real;
    int img;

    Complex(double x, int y)
      : real(x), img(y) {}

    ~Complex() = default;
};

int main() {
    for (int j = 0; j < 1000; ++j) {
        int x = 0;
        Complex y{3.14, x};

        for (int k = 0; k < 10000; ++k) {
            x = j * k;
            y.img = x;
            std::cout << "complex = "
                      << y.real << ", "
                      << y.img << "\n";
        }
    }
    return 0;
}
