#include <iostream>
#include <sstream>
#include "FaceDetection.h"

using namespace std;

int main()
{
    cout << "Hello !" << endl;




    string fileName;

    for( int i = 1; i < 6; i++ ) {


    FaceDetection faceDetection("data/detection.xml");
        bool detected = false;

        cout << "Start detection !" << endl;

        stringstream stream;
        stream << "images/" << i << ".bmp";
        stream >> fileName;

        cout << "File : " << fileName << endl;
        detected = faceDetection.faceDetect(fileName);

        if( detected ) {
            faceDetection.showOriginal();
            faceDetection.showFace();
            faceDetection.saveResult("data/out.bmp");
        } else {
            cout << "Sorry, no face detected." << endl;
        }
    }

    return 0;
}

