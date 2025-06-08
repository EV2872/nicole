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
    y: str
    
    constructor(x_: int, y_: str): super(x_) {
        y := y_;
    }

    destructor {
        print("Hola2\n");
    }
}

struct C extends B {
    z: double
    constructor(x_: int, y_: str, z_: double): super(x_, y_) {
        z := z_;
    }

    destructor {
        print("Hola3\n");
    }
}

auto cc = C{10, "HOLA", 3.14};

print(cc.x, "\n");
print(cc.y, "\n");
print(cc.z, "\n");

cc.x := 30;
cc.y[2] := '9';
cc.z := 10.88;
print(cc.x, "\n");
print(cc.y, "\n");
print(cc.z, "\n");