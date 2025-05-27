class Complex:
    def __init__(self, x: float, y: int):
        self.real = x
        self.img = y

    def __del__(self):
        pass

def main():
    for j in range(1000):
        x = 0
        y = Complex(3.14, x)

        for k in range(10000):
            x = j * k
            y.img = x
            print(f"complex = {y.real}, {y.img}")

if __name__ == "__main__":
    main()
