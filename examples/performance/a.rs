struct Complex {
    real: f64,
    img: i32,
}

impl Complex {
    fn new(x: f64, y: i32) -> Self {
        Complex { real: x, img: y }
    }
}

fn main() {
    for j in 0..1000 {
        let mut x: i32 = 0;
        let mut y = Complex::new(3.14, x);

        for k in 0..10000 {
            x = j * k;
            y.img = x;
            println!("complex = {}, {}", y.real, y.img);
        }
    }
}
