#include "Node.h"

using namespace std;

string Node::toString() {
    ostringstream oss;

    oss << nodeThresh << " " << leftVal << " " << rightVal << " " << leftIdX << " " << rightIdX << " " << rects.size();
    for( int i = 0; i < rects.size(); i++ ) {
        oss << " " << rects[i].toString();
    }

    return oss.str();
}

void Node::load(stringstream &stream) {
    int rectCount = -1;
    stream >> nodeThresh >> leftVal >> rightVal >> leftIdX >> rightIdX >> rectCount;
    rects.clear();
    for( int i = 0; i < rectCount; i++ ) {
        Rect rect;
        rect.load(stream);
        rects.push_back(rect);
    }
}
