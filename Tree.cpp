#include "Tree.h"

using namespace std;

string Tree::toString() {
    ostringstream oss;

    oss << nodes.size();
    for( int i = 0; i < nodes.size(); i++ ) {
        oss << " " << nodes[i].toString();
    }

    return oss.str();
}

void Tree::load(stringstream &stream) {
    int nodeCount = -1;
    stream >> nodeCount;
    nodes.clear();
    for( int i = 0; i < nodeCount; i++ ) {
        Node node;
        node.load(stream);
        nodes.push_back(node);
    }
}
