#include <iostream>
//#include "class.h"

using namespace std;

int main() {
    auto lambda_a = [](int &b) {
        b = -2;
        //return b*2;
    };
    int ulal = 3;
    lambda_a(ulal);
    return 0;
}
