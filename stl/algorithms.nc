def fibonacci(n: int): int {
  if (n == 0 or n == 1) {
    return n;
  }
  return fibonacci(n - 2) + fibonacci(n - 1);
}

def factorial(n: int): int {
  if (n == 0 or n == 1) {
    return 1;
  }
  return n * factorial(n - 1);
}
