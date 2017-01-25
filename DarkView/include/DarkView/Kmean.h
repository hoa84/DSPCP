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

#ifndef KMEAN_H
#define KMEAN_H

#include <QGLWidget>

#include <Data/PhysicsData.h>
#include <DarkView/DataIndirector.h>
#include <DarkView/MainWidget.h>
#include <GL/oglFont.h>

class Kmean : public QGLWidget {
    Q_OBJECT

public:
    Kmean(MainWidget * mw, oglWidgets::oglFont & font, QWidget *parent);

    void SetData( DataIndirector * _data );
    virtual QSize minimumSizeHint() const { return QSize(  50,  50); }
    virtual QSize sizeHint()        const { return QSize(1280, 820); }

signals:
    void UpdatedSelection( std::pair<int,int> );

public slots:
    void Reset();
    void Start();

protected:

    virtual void resizeGL ( int width, int height );
    virtual void initializeGL();
    virtual void paintGL();
    virtual void mousePressEvent ( QMouseEvent * event ) ;
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );

protected:
    DataIndirector * data;   
    MainWidget * mw;
    int curDraw;
    std::vector< std::pair<float,int> > dimLoc;
    std::vector<float> dim_min;
    std::vector<float> dim_max;
    int   dim;
    int selected;
    float d_scale;
    oglWidgets::oglFont * font;
    void DrawElement( float * elem );
    bool started;
    float rangeV;
    float CubicComp(float t, float p0,  float p1, float p2, float p3);
    float QuadraticComp(float t, float p0, float p1, float p2);
    float eleRange;

    // points for curve
    float p0, p1, p2, p3;
    void quadCurve(float a, float b, float c, float maxpoint);
    void cubicCurve(float a, float b, float c, float d, float maxpoint);
    float a, b, c, d;
    float e, f, g;
    void cubicAxis();
    void quadAxis();

    // vector to contain parameters of fitting curve.
    std::vector<float> qua_a, qua_b, qua_c;
    std::vector<float> cub_a, cub_b, cub_c, cub_d;
    float dis;
    int curveDegree;
    std::vector<float> fitErr;
    std::vector<float> maxPoint;

    // locate item function
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
    float flip;
    float flip2;
    int numCor;    

    float cubicFitCurve(int step, float * elem, int d0, int d1, float x0);
    float quadFitCurve(int step, float * elem, int d0, int d1, float x0);
    std::vector<int> curvePos;
    std::vector<float> selItems;

    // clustering
    int numClusters;
    std::vector<int> clusters;
    std::vector< std::pair<float, float> > centroid;

    // void initializeKM();
    void minmaxItem(int j);
    int indMaxElem, indMinElem;
    float maxElemX, maxElemY, minElemX, minElemY;

    void kMeanCluster();
    float dist(float x1, float y1, float x2, float y2);
    float xe, ye;
    void xyElement(float * elem, int i, int d0, int d1, float x0);    
    float cubicFitCurveKM(int step, float * elem, int d0, int d1, float x0);
    float quadFitCurveKM(int step, float * elem, int d0, int d1, float x0);
};

#endif // KMEAN_H
