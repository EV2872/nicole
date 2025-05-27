struct complex {
    real: double
    img: int

    constructor(x: double, y: int) {
        real := x;
        img := y;
    }

    destructor {}
}

for (let j: int = 0; j < 1000; ++j) {
    auto x = 0;
    auto y = complex{3.14, x};
    for (let k: int = 0; k < 10000; ++k) {
        x := j * k;
        y.img := x;
        print("complex = ", y.real, ", ", y.img, "\n");
    }
}