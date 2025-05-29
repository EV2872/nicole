struct A {
    x: int
    constructor(x_: int) {
        x := x_;
    }

    destructor {}
}

struct C {
    x: A
    constructor(x_: A) {
        x := x_;
    }

    destructor {}
}

auto cPrueba = C{A{1555}};

print(cPrueba.x.x);