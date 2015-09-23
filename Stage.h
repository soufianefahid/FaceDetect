#ifndef STAGE_H_INCLUDED
#define STAGE_H_INCLUDED

#include <vector>
#include <sstream>
#include "Tree.h"

using namespace std;

class Stage {
public :
    std::vector<Tree> trees;
    double stageThresh;

    string toString();
    void load(std::stringstream &stream);
};

#endif // STAGE_H_INCLUDED
