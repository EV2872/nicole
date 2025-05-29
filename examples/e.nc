struct A {
    x: int
    constructor(x_: int) {
    }

    destructor {}
}

struct B extends A {
    constructor(x_: int): super() {
        x := x_;
    }

    destructor {}
}


auto xx = B{10};

print(xx.x);