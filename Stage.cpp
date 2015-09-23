#include "Stage.h"

using namespace std;

string Stage::toString() {
    ostringstream oss;

    oss << stageThresh << " " << trees.size();
    for( int i = 0; i < trees.size(); i++ ) {
        oss << " " << trees[i].toString();
    }
    return oss.str();
}

void Stage::load(stringstream &stream) {
    int treeSize = -1;
    stream >> stageThresh >> treeSize;
    trees.clear();
    for( int i = 0; i < treeSize; i++ ) {
        Tree tree;
        tree.load(stream);
        trees.push_back(tree);
    }
}
