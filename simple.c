int my_func() {
  return 3;
}

int foo = my_func();

int main() {
  return my_func() + foo;
}
