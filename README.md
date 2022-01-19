# final
References for root finding:

Lobachevskiy

Graeffe

recent coolest: Jenkins-Traub

Jenkins-Traub for real numbers

others

Sturm theory
Files:

The project contains header, source and test files for solution of the Polinomial root problem solution.

Methods:

    Create/change polynome as array:

    setPoly(double* arr, size_t n);

    Set interval where root is seeked:

    setInterval(double a, double b);

    find one (maximal) root on interval:

    // name = Graeffe, Traub etc double findOneRoot_name();

    find N roots on interval:

    int findRoots_name(arr* roots, size_t N); // or return vector

TODO: a) complex roots

b) complex coefficients of polynome
Roots - optimization tasks

int allMax_Fib(RealFunc f, TDBL a, TDBL b, TDBL* mins); //https://en.wikipedia.org/wiki/Golden-section_search

int allMax_GoldenSection(RealFunc f, TDBL a, TDBL b, TDBL* mins); //https://en.wikipedia.org/wiki/Ternary_search int allMax_Brent(RealFunc f, TDBL a, TDBL b, TDBL* mins); //https://en.wikipedia.org/wiki/Brent%27s_method int allMax_Newton(RealFunc f, TDBL a, TDBL b, TDBL* mins); //https://en.wikipedia.org/wiki/Newton%27s_method_in_optimization
References for optimization tasks:

Include file: funcs.h

//https://en.wikipedia.org/wiki/Golden-section_search //https://en.wikipedia.org/wiki/Ternary_search

//https://en.wikipedia.org/wiki/Brent%27s_method

//https://en.wikipedia.org/wiki/Newton%27s_method_in_optimization
