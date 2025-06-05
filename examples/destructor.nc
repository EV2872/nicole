struct A {
    x: int
    constructor(x_: int) {
        x := x_;
    }

    destructor {
        print("Hola1\n");
    }
}

struct B {
    x: A
    constructor(x_: A) {
        x := x_;
    }

    destructor {
        print("Hola2\n");
    }
}

auto cc = B{A{10}};

