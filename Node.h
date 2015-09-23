#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <vector>
#include <sstream>
#include "Rect.h"

using namespace std;

class Node {
public :
    double nodeThresh;
    double leftVal;
    double rightVal;
    int leftIdX;
    int rightIdX;
    std::vector<Rect> rects;

    string toString();
    void load(std::stringstream &stream);
};

#endif // NODE_H_INCLUDED
