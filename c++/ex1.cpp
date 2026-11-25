#include <iostream>
#include <cmath>
using namespace std;

class Circle {
    private:
    int power(int base, int exponent) {
        return pow(base, exponent);
    }
    public:
    int narmol(){
        return power(2, 3);
    }
};

int main() {
    Circle c;
    cout << "The result of 2 raised to the power of 3 is: " << c.narmol() << endl;
    return 0;
}