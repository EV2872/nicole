struct A {
    x: int
    constructor(x_: int) {
        x := x_;
    }

    destructor {}
}

struct B {
    x: A*
    constructor(x_: A*) {
        x := x_;
    }

    destructor {}
}

auto a = new A{5};

auto b = B{a};

auto c = b.x;

auto d = *c;

print(d.x);