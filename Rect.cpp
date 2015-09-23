#include "Rect.h"

using namespace std;

string Rect::toString() {
    ostringstream oss;

    oss << rx << " " << ry << " " << rw << " " << rh << " " << wt;

    return oss.str();
}

void Rect::load(stringstream &stream) {
    stream >> rx >> ry >> rw >> rh >> wt;
}
