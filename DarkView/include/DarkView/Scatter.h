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

#ifndef SCATTER_H
#define SCATTER_H

#include <QGLWidget>

#include <Data/PhysicsData.h>
#include <DarkView/DataIndirector.h>
#include <DarkView/MainWidget.h>
#include <GL/oglFont.h>
#include <DarkView/DimensionalityReduction.h>

class Scatter : public QGLWidget {

    Q_OBJECT

public:
    Scatter(MainWidget * mw, oglWidgets::oglFont & font, QWidget *parent);

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
    DimensionalityReduction drPCA;

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
    float eleRange;
    // points for curve
    float a, b, c, d;
    float e, f, g;
    void cubicAxis();
    void quadAxis();
    // vector to contain parameters of fitting curve.
    std::vector<float> qua_a, qua_b, qua_c;
    std::vector<float> cub_a, cub_b, cub_c, cub_d;
    std::vector<int> cub_ind;


    int curveDegree;
    std::vector<float> fitErr;

    float cubicFitCurve(int step, float * elem, int d0, int d1, float x0, float x0_1, float x1, float x1_1);
    float quadFitCurve(int step, float * elem, int d0, int d1, float x0, float x1);

    // selected pair
    int sDim1;
    int sDim2;
    int selectedPair;
    bool drawDV;

    void detailView(int sd1, int sd2);

    int fitMeth;

    // controlpoints
    float xp0, xp1, xp2, xp3;
    float yp0, yp1, yp2, yp3;

    int ctrlSelected;
    float QuadraticComp(float t, float p0, float p1, float p2);
    float CubicComp(float t, float p0, float p1, float p2, float p3);
    int userDefaultDraw;
    int userFreeDraw;
    float curPointX, curPointY;
    std::vector< std::pair<float,float> > userPoints;
    int nseg;
    void knn_pca();
    int knum;
    std::vector< std::pair<float,float> > disKnn;
    std::vector< std::pair<float,float> > indKnn;

    float AngleCompute(float xs, float ys);
};

#endif // SCATTER_H
