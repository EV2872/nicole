class Complex {
    constructor(x, y) {
        this.real = x;
        this.img = y;
    }
    // No hay destructor explícito en JS
}

for (let j = 0; j < 1000; ++j) {
    let x = 0;
    let y = new Complex(3.14, x);

    for (let k = 0; k < 10000; ++k) {
        x = j * k;
        y.img = x;
        console.log(`complex = ${y.real}, ${y.img}`);
    }
}
