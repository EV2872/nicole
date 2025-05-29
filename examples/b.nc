struct A {
    x: int
    constructor(x_: int) {
        x := x_;
    }

    destructor {}
}

struct B {
    x: A
    constructor(x_: int) {
        x := A{x_};
    }

    destructor {}
}

auto x = B{10};

print(x.x.x);

x.x.x := 20;

print(x.x.x);


auto ww = "hola";

ww[0] := '7';

print(ww);

ww := "adios";

ww[0] := 'M';

print(ww);