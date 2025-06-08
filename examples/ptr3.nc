struct A {
    x: int
    constructor(x_: int) {
        x := x_;
    }

    virtual destructor {
        print("Hola1\n");
    }
}

struct B extends A {
    y: str
    
    constructor(x_: int, y_: str): super(x_) {
        y := y_;
    }

    destructor {
        print("Hola2\n");
    }
}

let x: A* = new B{10, "hola"};

auto aux = *x;