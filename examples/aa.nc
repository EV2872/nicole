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

def equalComplex(x: Complex, y: Complex): bool {
  return (x.real == y.real and x.img == y.img);
}

print(equalComplex(number, number2));