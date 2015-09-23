#ifndef FACEDETECTION_H_INCLUDED
#define FACEDETECTION_H_INCLUDED

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "Stage.h"
#include "Tree.h"
#include "Node.h"
#include "Rect.h"
#include "Result.h"
#include "CImg.h"
#include "tinyxml/tinyxml.h"

using namespace std;
using namespace cimg_library;


class FaceDetection {
private :
    vector<Stage> detection_data;
    CImg<unsigned char> image;
    vector< vector<unsigned long> > integralImage;
    vector< vector<unsigned long> > integralImage2;
    vector<Result> results;
    int windowWidth;

    void computeIntegralImage(CImg <unsigned char> src);
    bool Windowdetection(int width, int height);
    bool detectOnWindow(int x, int y, double scale, int w, double inverse_area);
    void init(string path);
    void loadXML(string path);


public :
    FaceDetection();
    FaceDetection(string path);
    bool faceDetect(string imgPath);
    void showOriginal();
    void showFace();
    void saveResult(string imgPath);
};

#endif // FACEDETECTION_H_INCLUDED
