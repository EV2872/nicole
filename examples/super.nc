struct A {
    x: int
    constructor(x_: int) {
        x := x_;
    }

    destructor {
        print("Hola1\n");
    }
}

struct B extends A {
    constructor(x_: int): super(x_) {}

    destructor {
        print("Hola2\n");
    }
}


auto cc = B{10};

print(cc.x);