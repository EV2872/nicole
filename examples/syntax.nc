auto a = [[[1, 2], [1, 2]]];

a[0][0][0] := 10;

print((a[0][0][0] * 10) / 20, "\n");



def max(x: int, y: int): int {
    return (x > y) ? x : y;
}

print(max(55, 35), "\n");


struct Person {
    name: str
    age: int

    constructor(x: str, y: int) {
        name := x;
        age := y;
    }

    destructor {}
}

auto object = Person{"Evian", 22};

if (object.age >= 18) {
    print(object.name, " ", object.age, "\n");
}