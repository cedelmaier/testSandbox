#include <tuple>
#include <vector>
#include <iostream>
#include <memory>

class base {
public:
    base(int pFoo = 0) : foo(pFoo) {}

    int foo;
};

template<class T>
class basespecies {
public:
    typedef std::shared_ptr<T> ptype;
    basespecies(double pBar = 0.0) : bar(pBar) {}

    std::vector<ptype> particles;
    double bar;
};

class derivedSpecies : public basespecies<base> {
public:
    template<typename... ARGS>
    derivedSpecies(double pNewbar = 0, ARGS&&... args) : basespecies<base> { std::forward<ARGS>(args)... }, newbar(pNewbar) {}

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

    // Can we make the generic holder for dervied species?
    systemArch<derivedSpecies, int>* testSystem = new systemArch<derivedSpecies, int>();

    std::cout << "n sys " << testSystem->nsys << std::endl;

    return 0;
}
