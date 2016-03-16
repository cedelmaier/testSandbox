#include <tuple>
#include <vector>
#include <iostream>
#include <memory>

// Base class for EVERYBODY
class base {
public:
    // Constructor with default arguments, and initialization
    base(int pFoo = 0) : foo(pFoo) {}

    int foo;
};

template<class T>
class basespecies {
public:
    typedef std::shared_ptr<T> ptype;
    basespecies(double pBar = 0.0) : bar(pBar) {}

    // virtual function
    virtual void doSomething() {}

    std::vector<ptype> particles;
    double bar;
};

class derivedSpecies : public basespecies<base> {
public:
    template<typename... ARGS>
    derivedSpecies(double pNewbar = 0, ARGS&&... args) : basespecies<base> { std::forward<ARGS>(args)... }, newbar(pNewbar) {}

    // virtual function that gets overridden
    virtual void doSomething() override {
        /*try {
            *(int*) 0 = 0;
        }
        catch (const std::exception &e){
            std::cout << e.what();
        }*/
    }

    double newbar;
};

// Container for multiple types of derivedSpecies
template<typename... SYSTYPES>
class systemArch {
public:
    systemArch() {
        nsys = std::tuple_size<std::tuple<SYSTYPES...> >::value;
    }

    int nsys;
    std::tuple<SYSTYPES...> systems;
};

int main() {
    // Can we make a base?
    base* testBase = new base(1);

    // Can we make a basespecies?
    basespecies<base>* testBaseSpecies = new basespecies<base>(1.0);

    // Can we make a derived species?
    derivedSpecies* testDerivedSpecies = new derivedSpecies(2.0, 1.0);

    testDerivedSpecies->doSomething();

    // Can we make the generic holder for dervied species?
    systemArch<derivedSpecies, int>* testSystem = new systemArch<derivedSpecies, int>();

    std::cout << "n sys " << testSystem->nsys << std::endl;

    // Start playing around with how to access things in the tuple
    auto& system1 = std::get<0>(testSystem->systems);
    std::cout << "system1: {newbar: " << system1.newbar << "}\n";
    system1.newbar = 2.0;
    std::cout << "system1: {newbar: " << system1.newbar << "}\n";

    auto& system2 = std::get<0>(testSystem->systems);
    std::cout << "system2: {newbar: " << system2.newbar << "}\n";
    system2.newbar = 32.0;
    std::cout << "system2: {newbar: " << system2.newbar << "}\n";

    return 0;
}
