#include <iostream>

#include "Matrix.h"

using namespace std;
using namespace matrix;

int rand_int(int min, int max) 
{
    return min + rand() % (max - min);
}

double rand_double(double min, double max) 
{
    return min + (double)rand() / RAND_MAX * (max - min);
}

int main() 
{
    setlocale(LC_CTYPE, "Rus");

    auto func  = [](size_t x, size_t y)->double { return rand_int(0, 10); };
    auto func1 = [](size_t x, size_t y)->int { return rand_int(0, 10); };

    Matrix<double> f(2, 2, func);
    Matrix<double> s(2, 2, func);

    Matrix<double> summ = f + s;

    cout << f << endl << '+' << endl << s << endl << '=' << endl;
    cout << summ;

    Matrix<int> f1(2, 2, func1);
    Matrix<int> s1(2, 2, func1);

    Matrix<int> summ1 = f1 + s1;

    cout << f1 << endl << '+' << endl << s1 << endl << '=' << endl;
    cout << summ1;

    return 0;
}
