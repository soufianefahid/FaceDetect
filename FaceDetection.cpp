#include "FaceDetection.h"
#include <iostream>
#include <fstream>

FaceDetection::FaceDetection(string path) {
    this->loadXML(path);
}

FaceDetection::FaceDetection() {
    string path = "data/detection.dat";
    this->loadXML(path);
}


void FaceDetection::computeIntegralImage(CImg <unsigned char> src) {


    int width = src.width();
    int height = src.height();

    integralImage=vector<vector<unsigned long> >(height,vector<unsigned long>(width,0));
    integralImage2=vector<vector<unsigned long> >(height,vector<unsigned long>(width,0));

    unsigned char red = 0;
    unsigned char green = 0;
    unsigned char blue = 0;

    for (int i=1; i < height; i++) {

        for (int j=1; j < width; j++) {

            red = src(j, i, 0, 0);
            green = src(j, i, 0, 1);
            blue = src(j, i, 0, 2);

            unsigned char grey = (int)(0.2989*red + 0.587*green + 0.114*blue);

            integralImage[i][j] = integralImage[i-1][j] + integralImage[i][j-1] - integralImage[i-1][j-1] + grey;
            integralImage2[i][j] = integralImage2[i-1][j] + integralImage2[i][j-1] - integralImage2[i-1][j-1] + grey*grey  ;
        }
    }

}

bool FaceDetection::Windowdetection(int width, int height) {

    double scaleWidth = width/20.0;
    double scaleHeight = height/20.0;
    //c'est pour choisir le plus grand carré qui sera notre window de detection et on va diminuer la taille avec
    double startScale = scaleHeight < scaleWidth ? scaleHeight : scaleWidth;

    //de cette fenêtre avec le scale update
    double scaleUpdate = 1.0 / 1.2;

    int windowWidth, endX, endY, step;

    for (double scale = startScale; scale > 1; scale *= scaleUpdate) {
        windowWidth = 20*scale;
        // les conditions d'arrêt pour le parcours de l'image avec notre window
        endX = width - windowWidth;
        endY = height - windowWidth;

        step = ( max (scale , 2.0) );
        //step = 1;

        double inverse_area = 1.0 / (windowWidth * windowWidth);
        for (int y = 0; y < endY; y += step) {
            for (int x = 0; x < endX; x += step) {

                bool passed = this->detectOnWindow(x, y, scale, windowWidth, inverse_area);
                if (passed) {
                        //chaque carré qui contient un visage va faire partie de notre results
                            //results est une classe qui va contenir les rectangles des visages
                    results.push_back(Result(x,y,windowWidth));
                }

            } // end x
        } // end y

    }  // end scale

    if( results.size() == 0 ) {
        return false;
    }
    return true;
}

bool FaceDetection::detectOnWindow(int x, int y, double scale, int windowWidth, double inv_area) {

    double mean  = (integralImage[y+windowWidth][x+windowWidth] + integralImage[y][x] - integralImage[y+windowWidth][x] - integralImage[y][x+windowWidth])*inv_area;
    double vnorm = (integralImage2[(y+windowWidth)][x+windowWidth]
              + integralImage2[y][x]
              - integralImage2[y+windowWidth][x]
              - integralImage2[y][x+windowWidth])*inv_area - (mean*mean);

    //on calcule l'ecart type
    vnorm = vnorm > 1 ? sqrt(vnorm) : 1;

    double stageThresh, stageSum, treeSum, nodeThresh, leftVal, rightVal, rectSum;
    int countStages = detection_data.size(), countTrees = 0, leftIdX, rightIdX, countRects;

    bool stop;

    for (int i_stage = 0; i_stage < countStages; i_stage++) {

        Stage stage = detection_data[i_stage];

        vector<Tree> trees = stage.trees;

        stageThresh = stage.stageThresh;
        stageSum = 0;

        countTrees = trees.size();

        for (int i_tree = 0; i_tree < countTrees; i_tree++) {
            Tree tree = trees[i_tree];
            Node currentNode = tree.nodes[0];
            treeSum = 0;
            stop = false;
            while (!stop) {

                nodeThresh = currentNode.nodeThresh;
                leftVal = currentNode.leftVal;
                rightVal = currentNode.rightVal;
                leftIdX = currentNode.leftIdX;
                rightIdX = currentNode.rightIdX;
                vector<Rect> rects = currentNode.rects;

                rectSum = 0;
                countRects = rects.size();

                for (int i_rect = 0; i_rect < countRects; i_rect++) {

                    long rectX = (int)(rects[i_rect].rx*scale+x);
                    long rectY = (int)(rects[i_rect].ry*scale+y);
                    long rectWidth = (int)(rects[i_rect].rw*scale);
                    long rectHeight = (int)(rects[i_rect].rh*scale);
                    int rectWeight = rects[i_rect].wt;

                    double r_sum = (double)(integralImage[(rectY+rectHeight)][rectX + rectWidth]
                              + integralImage[rectY][rectX]
                              - integralImage[(rectY+rectHeight)][rectX]
                              - integralImage[rectY][rectX+rectWidth])*rectWeight;

                    rectSum += r_sum;
                }
                rectSum *= inv_area;

                stop = true;

                if (rectSum >= nodeThresh*vnorm) {

                    if (rightIdX == -1) {
                        //il n'y a plus aucune arborescence de plus
                        treeSum = rightVal;

                    } else {

                        stop = false;
                        //si il y a un noeud il devient notre noeud courant
                        currentNode = tree.nodes[rightIdX];

                    }

                } else {

                    if (leftIdX == -1) {
                        //il n'y a plus aucune arborescence de plus

                        treeSum = leftVal;

                    } else {

                        stop = false;
                        currentNode = tree.nodes[leftIdX];
                    }
                }
            }

            stageSum += treeSum;

        }
        if (stageSum < stageThresh) {
            //Si il est rejetter par un classifier il va tre va dire que le window ne contient aucun visage
            return false;
        }
    }
    //si il termine tous les stages a veut dire qu'il y a un visage
    return true;
}

bool FaceDetection::faceDetect(string imgPath) {

	image.load(imgPath.c_str());

    int width = image.width();
    int height = image.height();

    //On va choisir le ratio pour determiner la nouvelle taille de l'image a traiter on peut ne pas le redimensionner mais
    //les calcules vont divergger en terme de temps
    //la taille maximale qu'on doit pas dépasser est de 384*384 px pour que les calcules ne divergent
    int diffWidth = 320 - width;
    int diffHeight = 240 - height;
    float ratio = -1;

    if (diffWidth > diffHeight) {
        ratio = (float)width / 320;
    } else {
        ratio = (float)height / 240;
    }
    ratio=width/384;
    if (ratio != 0) {

        CImg <unsigned char> cropArea = image;
        cropArea = cropArea.get_crop(0,0, width, height);
        CImg <unsigned char> reducedimage = cropArea.resize(width /ratio ,height / ratio,-100,-100, 3, 0,0,0,0,0);

        this->computeIntegralImage(reducedimage);

        bool detected = this->Windowdetection(reducedimage.width(), reducedimage.height());
        if (detected)
        {
            for( int i = 0; i < results.size(); i++ ) {
                //pour dessiner les rectangles autour des visages avec la taille réelle
                results[i].x *= ratio;
                results[i].y *= ratio;
                results[i].width *= ratio;
            }
            return true;
        }
    } else {
        this->computeIntegralImage(image);
        return this->Windowdetection(image.width(), image.height());
    }

    return false;
}

void FaceDetection::showOriginal() {
   //l'affichage de l'image avant la detection
    CImgDisplay disp(this->image);
    system("PAUSE");
}

void FaceDetection::showFace() {

    char c_green[] = {0, 255, 0};
    CImg <unsigned char> faceArea = this->image;
   //dessiner les rectangles autour des visages
    for( int i = 0; i < results.size(); i++ ) {
faceArea = faceArea.draw_line(results[i].x,results[i].y, results[i].x, results[i].y+results[i].width, c_green, 1);
   faceArea = faceArea.draw_line(results[i].x,results[i].y, results[i].x+results[i].width, results[i].y, c_green, 1);
   faceArea = faceArea.draw_line(results[i].x+results[i].width,results[i].y, results[i].x+results[i].width, results[i].y+results[i].width, c_green, 1);
   faceArea = faceArea.draw_line(results[i].x,results[i].y+results[i].width, results[i].x+results[i].width, results[i].y+results[i].width, c_green, 1);
    }
    //affichage des résultats aprés la detection
    CImgDisplay disp(faceArea);
    system("PAUSE");
}

void FaceDetection::saveResult(string imgPath) {
    char c_green[] = {0, 255, 0};
    CImg <unsigned char> faceArea = this->image;
    for( int i = 0; i < results.size(); i++ ) {
faceArea = faceArea.draw_line(results[i].x,results[i].y, results[i].x, results[i].y+results[i].width, c_green, 1);
   faceArea = faceArea.draw_line(results[i].x,results[i].y, results[i].x+results[i].width, results[i].y, c_green, 1);
   faceArea = faceArea.draw_line(results[i].x+results[i].width,results[i].y, results[i].x+results[i].width, results[i].y+results[i].width, c_green, 1);
   faceArea = faceArea.draw_line(results[i].x,results[i].y+results[i].width, results[i].x+results[i].width, results[i].y+results[i].width, c_green, 1);
    }
    faceArea.save_bmp(imgPath.c_str());
}

void FaceDetection::loadXML(string path) {

    TiXmlNode* node = 0;
    TiXmlNode* stageNode = 0;
    TiXmlNode* treeElement = 0;
    TiXmlNode* nodeElement = 0;
    TiXmlNode* rectElement = 0;

    TiXmlDocument doc;
    bool loadOkay  = doc.LoadFile(path.c_str());
    if (!loadOkay) {
        cout << "Error loading XML file !" << endl;
        exit(1);
    }

    node = doc.RootElement();

    node = node->FirstChild( "haarcascade_profileface" );
    string size = node->FirstChild( "size" )->ToElement()->GetText(); // Size of Window
    {
        stringstream oss;
        oss << size;
        oss >> windowWidth;
        oss.str(std::string());
    }

    node = node->FirstChild( "stages" );

    for( stageNode = node->FirstChild( "_" ); stageNode; stageNode = stageNode->NextSibling( "_" ) ) {
        Stage stage;
        string stage_threshold = stageNode->FirstChild( "stage_threshold" )->ToElement()->GetText();

        stage.stageThresh = atof(stage_threshold.c_str());

        for( treeElement = stageNode->FirstChild( "trees" )->FirstChild( "_" ); treeElement; treeElement = treeElement->NextSibling( "_" ) ) {

            Tree tree;

            for( nodeElement = treeElement->FirstChild( "_" ); nodeElement; nodeElement = nodeElement->NextSibling( "_" ) ) {

                Node node;

                string threshold = nodeElement->FirstChild( "threshold" )->ToElement()->GetText();
                string left_val = nodeElement->FirstChild( "left_val" )->ToElement()->GetText();
                string right_val = nodeElement->FirstChild( "right_val" )->ToElement()->GetText();
                string left_idx = nodeElement->FirstChild( "left_idx" )->ToElement()->GetText();
                string right_idx = nodeElement->FirstChild( "right_idx" )->ToElement()->GetText();

                node.nodeThresh = atof(threshold.c_str());
                node.leftVal = atof(left_val.c_str());
                node.rightVal = atof(right_val.c_str());
                node.leftIdX = atoi( left_idx.c_str() );
                node.rightIdX = atoi( right_idx.c_str() );

                for( rectElement = nodeElement->FirstChild( "feature" )->FirstChild( "rects" )->FirstChild( "_" ); rectElement; rectElement = rectElement->NextSibling( "_" ) ) {
                    Rect rect;
                    string rectValues = rectElement->ToElement()->GetText();
                    {
                        stringstream oss;
                        oss << rectValues;
                        oss >> rect.rx >> rect.ry >> rect.rw >> rect.rh >> rect.wt;
                    }
                    node.rects.push_back(rect);
                }

                tree.nodes.push_back(node);
            }
            stage.trees.push_back(tree);
        }

        this->detection_data.push_back(stage);
    }
}

