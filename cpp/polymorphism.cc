#include <iostream>
using namespace std;
class Base {
 public:
  virtual void fun() {
    std::cout << "This is Base::fun()" << std::endl;;
  }
  void fun2() {
    std::cout << "This is Base::fun2()" << std::endl;;
  }
  virtual void fun3() {
    std::cout << "This is Base::fun2()" << std::endl;;
  }

};

class Derived : public Base {
 public:
  void fun() override {
    std::cout << "This is Derived::fun()" << std::endl;
  }
  void fun3() override {
    std::cout << "This is Derived::fun()" << std::endl;
  }
};

class Derived2 : public Base {
 public:
  void fun() override {
    std::cout << "This is Derived::fun()" << std::endl;
  }
};


class Normal {
 public:
  void fun() {
    std::cout << "This is Normal::fun()" << std::endl;
  }
};

int main(int argc, char** argv) {
  std::cout << "sizeof(Base)=" << sizeof(Base)
            << "; sizeof(Derived)=" << sizeof(Derived)
            << "; sizoef(Normal)=" << sizeof(Normal) << std::endl;
  return 0;
  /**
  Base *base = nullptr;
  if (strcmp(argv[1], "base") == 0) {
    base = new Base();
  } else {
    base = new Derived();
  }
  base->fun();
  return 0;
  **/
}
