#ifndef POINT_H
#define POINT_H

#include <iostream>

using namespace std;

struct Point {
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) {
    }

    void print() {
        cout << "Point(" << x << "," << y << ")" << endl;
    }
};

#endif // POINT_H
