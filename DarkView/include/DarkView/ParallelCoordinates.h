/*
**  Data Scalable Approach for Parallel Coordinates
**  Copyright (C) 2016 - Hoa Nguyen
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PARALLELCOORDINATES_H
#define PARALLELCOORDINATES_H

#include <QGLWidget>
#include <QPixmap>
#include <QScreen>
#include <QtGui>
#include <Data/PhysicsData.h>
#include <DarkView/DataIndirector.h>
#include <DarkView/MainWidget.h>
#include <GL/oglFont.h>
#include <DarkView/DimensionalityReduction.h>
#include <math.h>

class ParallelCoordinates : public QGLWidget {
    
    Q_OBJECT

public:
    ParallelCoordinates(MainWidget * mw, oglWidgets::oglFont & font, QWidget *parent);

    void SetData( DataIndirector * _data );
    virtual QSize minimumSizeHint() const { return QSize(  50,  50); }
    virtual QSize sizeHint()        const { return QSize(1280, 820); }

signals:
    void UpdatedSelection( std::pair<int,int> );

public slots:
    void Reset();
    void Start();

private slots:
     void saveScreenshot();

protected:
    virtual void resizeGL ( int width, int height );
    virtual void initializeGL();
    virtual void paintGL();
    virtual void mousePressEvent ( QMouseEvent * event ) ;
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );

protected:
    DimensionalityReduction drPCA;

    DataIndirector * data;   
    MainWidget * mw;
    int curDraw;
    std::vector< std::pair<float,int> > dimLoc;
    std::vector<float> dim_min;
    std::vector<float> dim_max;
    int dim;
    int selected;
    float d_scale;
    oglWidgets::oglFont * font;
    bool started;
    float rangeV;

    // intersection computing function between two lines
    float xInts, yInts;
    bool intsComp(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

    // for knnPCA function and draw_boundary function.
    int knum;
    int frame;

    std::vector< std::pair<float,float> > disKnn;
    std::vector< std::pair<float,float> > indKnn;
    std::vector< float > meanPCAx;
    std::vector< float > meanPCAy;
    std::vector< float > primPCAx;
    std::vector< float > primPCAy;

    int steV;
    std::vector< std::pair<float,float> > intsPoints;
    std::vector< std::pair<float,float> > elemPoints;    
    std::vector< std::pair<float, float> > posPoints;
    std::vector< std::pair<float, float> > negPoints;

    // positive intersection points
    std::vector< std::pair<float,float> > posints;
    std::vector< std::pair<float,float> > posints1;
    std::vector< std::pair<float,float> > posints2;

    // negative intersection points
    std::vector< std::pair<float,float> > negints;
    std::vector< std::pair<float,float> > negints1;
    std::vector< std::pair<float,float> > negints2;
    std::vector< std::pair<float,float> > negints3;
    void knnPCA(int jd, int md);
    void DrawBoundary(int d0, int d1, float x0, float x1, float startx, float engrx, float starty, float engry, int numgr, float colgr);
    void DrawPosBoundary(int d0, int d1, float x0, float x1, float maxX, float minX, float maxY, float minY, float colgr, int grnum);
    float AngleCompute(float xs, float ys);

    // draw contours for different k values
    // void Kcontour(int kn, int kcolor);

    // number of triangle mesh
    int nseg; // number of segments per group
    int verseg; // number of segments per polygon
    int ntri; // number of segments per side
    // marching Triangles algorithm for triangle ABC
    // threshold for marching algorithm
    float thresholdTri;
    bool point_in_tri(float sx, float sy, float ax, float ay, float bx, float by, float cx, float cy);
    void marchingTriangles(float ax, float ay, float avalue, float bx, float by, float bvalue, float cx, float cy, float cvalue, float colgr, int posneg);

    // for testing
    float th1, th2, th3, th4;
    int in;
    QString fileName;
    // radius
    float rad;
    // threshold for negative color map;
    float ng1, ng2, ng3, ng4, ng5, ng6, ng7, ng8, ng9;
    // method for positive or negative relationship
    int hasPos;
    int hasNeg;

    // to process triangle mesh
    void triangleMesh(float x0, float x1, float stgrx, float engrx, float stgry, float engry, int m);
    // threshold for positive color map
    float ngpos1, ngpos2, ngpos3, ngpos4, ngpos5, ngpos6, ngpos7, ngpos8, ngpos9;

    Data::PhysicsData datfile;
    int numtest;
    float ngVector[10];
    float ngposVector[10];
    float ngVectorH[10];
    float radpos;

    // 1 single K case or variation K case
    int kcase;

    // locate item interaction
    // locate item function
    // 1 item
    bool locItem;
    bool selLine;
    float xItem;
    float yItem;
    int selectedDim1;
    int selectedDim2;
    float disPLine(float xp, float yp, float m, float b);
    float preCor, curCor;
    float selectedx1, selectedy1, selectedx2, selectedy2;
    void selectedLine();
    void DrawSelectedLine();
    // items range
    bool locItemRange;
    bool selLineRange;
    int itemStart;
    int itemEnd;
    int selectedItem;
    bool stopRange;
    std::vector<float> selItems;
    void DrawCircle(float cx, float cy, float r, int num_segments, std::string t);
    int menu;

    // interaction to select trend group
    int selectedGroup;
    // start and end points of neg group1
    float stgrx, stgry, engrx, engry;
    float colorgr1;
    // start and end points of neg group2
    float stgrx1, stgry1, engrx1, engry1;
    float colorgr2;

    // start and end points of neg group3
    float stgrx2, stgry2, engrx2, engry2;
    float colorgr4;
    // start and end points of pos group
    float maxPPX, maxPPY, minPPX, minPPY;
    float colorgr3;

    float maxPPX1, maxPPY1, minPPX1, minPPY1;
    float colorgrPos1;
    float maxPPX2, maxPPY2, minPPX2, minPPY2;
    float colorgrPos2;

    float startSelx1, startSely1, startSelx2, startSely2;
    bool startSelVal;

    void DrawSelectedHistogram(int jd0, int jd1);
    float thresCorr;

    // kmean Clustering
    int numClusters;
    int numPosClusters;
    std::vector<int> clusters;
    std::vector< std::pair<float, float> > centroid;

    //void initializeKM();
    void minmaxItem(int j);
    int indMaxElem, indMinElem;
    float maxElemX, maxElemY, minElemX, minElemY;

    void kMeanCluster(int j);
    float dist(float x1, float y1, float x2, float y2);
    float xe, ye;
    void xyElement(float * elem, int i, int d0, int d1, float x0);

    std::vector< float > pcorx;
    std::vector< float > pcory;

    std::vector<float> eigenNeg;
    std::vector<float> eigenNeg1;
    std::vector<float> eigenNeg2;
    std::vector<float> eigenNeg3;
    std::vector<float> eigenPos;
    std::vector<float> eigenPos1;
    std::vector<float> eigenPos2;

    // texture rendering
    int xSize , ySize; //size of texture
    //our OpenGL texture handle
    //unsigned int texture;
    GLuint texture;
    //new array
    char* colorBits;
    void CreateTexture();
    int viewport[4];
    void clusterPos1(int jd0, int jd1, int itStart, int itEnd);
    float a0, a1, a2, a3;
    float b0, b1, b2, b3;
    void clusterPos2(int jd0, int jd1, int itStart, int itEnd);
    void clusterPos3(int jd0, int jd1, int itStart, int itEnd);

    // histogram curve for lines density
    void histCurve();
    int numbin;
    float binrange;
    int bin0[100];
    int bin1[100];
    float bin0x[100];
    float bin0y[100];
    float bin1x[100];
    float bin1y[100];
};

#endif // PARALLELCOORDINATES_H
