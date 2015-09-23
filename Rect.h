#ifndef RECT_H_INCLUDED
#define RECT_H_INCLUDED

#include <sstream>

using namespace std;

class Rect {
public :
    int rx;
    int ry;
    int rw;
    int rh;
    int wt;

    string toString();
    void load(std::stringstream &stream);
};

#endif // RECT_H_INCLUDED
