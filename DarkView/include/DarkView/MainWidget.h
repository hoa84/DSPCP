/*
**  Data Scalable Approach for Parallel Coordinates
**  Copyright (C) 2016 - Hoa Nguyen, Paul Rosen
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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGLWidget>
#include <QMenuBar>
#include <QtGui>

#include <time.h>

#include <Data/PhysicsData.h>
#include <DarkView/SmallMultiples.h>

#include <GL/oglFont.h>
#include <GL/oglTexture2D.h>

class MainWidget : public QGLWidget
{
    Q_OBJECT

public:

    MainWidget( QWidget * parent = 0);
    ~MainWidget();

    virtual QSize minimumSizeHint() const { return QSize(  50,  50); }
    virtual QSize sizeHint()        const { return QSize(1280, 100); }

public slots:
    void UpdateSelection( std::pair<int,int> );   
    void Start();

protected:

    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL ( int width, int height );


protected:

    oglWidgets::oglTexture2D progressive_tex;
    DataIndirector * output;

    bool need_reset;

    SmallMultiples sm;
    ScatterPlot    sp;

    bool started;

public:
    oglWidgets::oglFont        font;

public:

    void SetData( DataIndirector * _output );
    void ProgressiveReset();

public:
    virtual void mouseDoubleClickEvent ( QMouseEvent * event );
    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void keyReleaseEvent ( QKeyEvent * event );


protected:
    // Interaction based variables
    int mouse_button, mouse_x, mouse_y;
    int interp_points;

    std::pair<int,int> cur_selection;

};


#endif
