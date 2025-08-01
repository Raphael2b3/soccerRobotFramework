#include <iostream>
#include <typeinfo>

class B {
public:
    virtual const char* getClassName() {
        return typeid(*this).name();
    }
};

class A : public B {
};

int main() {
    A a;
    B* b = &a;
    std::cout << "Typ: " << b->getClassName() << std::endl;

    return 0;
}