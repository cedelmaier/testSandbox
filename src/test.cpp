#include <tuple>
#include <type_traits>
#include <vector>
#include <iostream>
#include <memory>

// Template functions for everybody to use for tuple stuff
// Taken from Stack Overflow entirely
// XXX: Not C++14 compatible!!!!!!
namespace detail {
    template<bool b, typename T=void>
    using EnableIf = typename std::enable_if<b,T>::type;
    
    template<int Index, template<typename T>class Search, int Which, typename, class First, class... Types>
    struct get_internal:
        get_internal<Index + 1, Search, Which, void, Types...>
    {};

    template<int Index, template<typename T>class Search, int Which, class First, class... Types>
    struct get_internal<Index, Search, Which, EnableIf<!Search<First>::value>, First, Types...>:
        get_internal<Index + 1, Search, Which, void, Types...>
    {};
    template<int Index, template<typename T>class Search, int Which, class First, class... Types>
    struct get_internal<Index, Search, Which, EnableIf<Search<First>::value>, First, Types...>:
        get_internal<Index + 1, Search, Which-1, void, Types...>
    {};
    template<int Index, template<typename T>class Search, class First, class... Types>
    struct get_internal<Index, Search, 0, EnableIf<Search<First>::value>, First, Types...>:
        std::integral_constant<int, Index>
    {};

    template<template<typename>class Test, int Which=0, class... Types>
    auto get(std::tuple<Types...>& tuple)->
    decltype(std::get<get_internal<0,Test,Which,void,Types...>::value>(tuple))
    {
        return std::get<get_internal<0,Test,Which,void,Types...>::value>(tuple);
    }
    template<template<typename>class Test, int Which=0, class... Types>
    auto get(std::tuple<Types...> const& tuple)->
    decltype(std::get<get_internal<0,Test,Which,void,Types...>::value>(tuple))
    {
        return std::get<get_internal<0,Test,Which,void,Types...>::value>(tuple);
    }
    template<template<typename>class Test, int Which=0, class... Types>
    auto get(std::tuple<Types...>&& tuple)->
    decltype(std::move(std::get<get_internal<0,Test,Which,void,Types...>::value>(tuple)))
    {
        return std::move(std::get<get_internal<0,Test,Which,void,Types...>::value>(tuple));
    }

    template<typename T>
    struct is_type {
    template<typename U>
    using test = std::is_same<T,U>;
    };

    template<class T, int Which=0, class... Types>
    T& get(std::tuple<Types...>& tuple)
    {
        return get<is_type<T>::template test,Which>(tuple);
    }
    template<class T, int Which=0, class... Types>
    T const& get(std::tuple<Types...> const& tuple)
    {
        return get<is_type<T>::template test,Which>(tuple);
    }
    template<class T, int Which=0, class... Types>
    T&& get(std::tuple<Types...>&& tuple)
    {
        return std::move(get<is_type<T>::template test,Which>(tuple));
    }
}

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

    void setVals(double pBar = 0.0) {
        bar = pBar;
    }

    void print() {
        std::cout << "{bar: " << bar << "}";
    }

    std::vector<ptype> particles;
    double bar;
};

class derivedSpecies : public basespecies<base> {
public:
    template<typename... ARGS>
    derivedSpecies(double pNewbar = 0, ARGS&&... args) : basespecies<base> { std::forward<ARGS>(args)... }, newbar(pNewbar) {}

    // virtual function that gets overridden
    template<typename... ARGS>
    void setVals(double pNewbar, ARGS&&... args) {
        newbar = pNewbar;
        basespecies::setVals(std::forward<ARGS>(args)...);
    }

    void print() {
        std::cout << "{newbar: " << newbar << "}, ";
        basespecies<base>::print();
        std::cout << std::endl;
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

    // Can we make the generic holder for dervied species?
    systemArch<derivedSpecies, int>* testSystem = new systemArch<derivedSpecies, int>();

    std::cout << "n sys " << testSystem->nsys << std::endl;

    // Start playing around with how to access things in the tuple
    auto& system1 = std::get<0>(testSystem->systems);
    system1.setVals(8.0, 9.0);
    std::cout << "system1: {newbar: " << system1.newbar << "}";
    std::cout << ", {bar: " << system1.bar << "}\n";

    // Make sure that assignment is respected
    auto& system2 = std::get<0>(testSystem->systems);
    system2.print();

    // Try out our fancy tuple shit
    auto& system3 = detail::get<derivedSpecies>(testSystem->systems);
    system3.print();
    system3.setVals(42.0, 24.0);
    system3.print();

    auto& system4 = detail::get<derivedSpecies>(testSystem->systems);
    system4.print();

    return 0;
}
