#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <vector>
#include <sstream>
#include "Node.h"

using namespace std;

class Tree {
public :
    std::vector<Node> nodes;

    string toString();
    void load(std::stringstream &stream);
};

#endif // TREE_H_INCLUDED
