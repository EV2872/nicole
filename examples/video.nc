auto a = [[1, 2, 3], [4, 5, 6], [7, 8, 9]];

if (a[0][2] == 3) {
    print("Hola\n");
}

def fibonacci(n: int): int {
  if (n == 0 or n == 1) {
    return n;
  }
  return fibonacci(n - 2) + fibonacci(n - 1);
}

def factorial(n: int): int {
  if (n == 0 or n == 1) {
    return 1;
  }
  return n * factorial(n - 1);
}

print(factorial(5), " ", fibonacci(10), "\n");

print("concat1" + "concat2" + "\n");

let m: int = 5;
let n: int = 10;
for (auto i = 0; i < m; ++i) {
    for (auto j = 0; j < n; ++j) {
        print("*");
    }
    print("\n");
}

struct Complex {
    real: double
    img: double

    constructor(x: double, y: double) {
        real := x;
        img := y;
    }

    destructor {}
}

auto number = Complex{3.14, 9.8};

let number2: Complex = Complex{3.14, 9.8};

print(number.real, ", ", number.img, "\n");

number.real := -number.real;

let var: bool = number.img == 9.8;
if (var) {
    print(number.real, ", ", number.img, "\n");
}

print("number2 ", number2.real, "\n");

def equalComplex(x: Complex, y: Complex): bool {
  return (x.real == y.real and x.img == y.img);
}

print(equalComplex(number, number2));

/*
while (!!var or !var) {
    print("Infinite Loop\n");
}
*/