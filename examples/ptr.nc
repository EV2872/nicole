struct A {
    x: int
    constructor(x_: int) {
        x := x_;
    }

    destructor {}
}

auto a = new A{5};



auto b = *a;

print(b.x, "\n");

b.x := 20;

print(b.x, "\n");

auto c = *a;

print(c.x, "\n");

delete a;

print(a, "\n");

a := new A{60};

auto bb = *a;

print(a);