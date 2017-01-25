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

#include <DarkView/MainWidget.h>

#include <iostream>
#include <algorithm>

MainWidget::MainWidget( QWidget *parent) : QGLWidget( QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::AlphaChannel | QGL::DirectRendering | QGL::SampleBuffers), parent ), sm(font), sp(font) {

    //font.Load( "fonts/arial.font" );
    font.Load( ":/fonts/arial.font" );

    mouse_button = Qt::NoButton;
    mouse_x = 0;
    mouse_y = 0;

    output = 0;
    interp_points = 0;

    started = false;
    need_reset = false;

    setMouseTracking(true);

    cur_selection = std::make_pair(-1,-1);
}

MainWidget::~MainWidget(){ }


void MainWidget::Start(){
    started = true;
}

void MainWidget::SetData( DataIndirector * _output ){
    output = _output;
    sm.Reset();
    sp.Reset();
}

void MainWidget::resizeGL ( int , int ){
    ProgressiveReset( );
}

void MainWidget::ProgressiveReset(){
    need_reset = true;
    /*
    sm.ProgressiveReset();
    unsigned int white = 0xffffffff;
    progressive_tex.TexImage2D( GL_RGBA, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &white );
    */
}

void MainWidget::initializeGL(){
    unsigned int white = 0xffffffff;
    progressive_tex.SetMinMagFilter( GL_LINEAR, GL_LINEAR );
    progressive_tex.SetWrapST( GL_REPEAT, GL_REPEAT );
    progressive_tex.TexImage2D( GL_RGBA, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &white );

}

void MainWidget::paintGL(){

    if( need_reset )
    {
        sm.ProgressiveReset();
        unsigned int white = 0xffffffff;
        progressive_tex.TexImage2D( GL_RGBA, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &white );
        need_reset = false;
    }

    // Clear the display
    glClearColor(1,1,1,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if( !started ){
        update();
        return;
    }

    int width  = size().width();
    int height = size().height();
    float aspect = (float)width/(float)height;

    glViewport( 0,0,width,height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glColor3f(1,1,1);
    progressive_tex.DrawTexture();

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    //glScalef( 1.0f, -1.0f, 1.0f );
    //glOrtho( -1, 1, -1, 1, -1, 1 );
    glScalef( 1.0f, 1.0f, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glLineWidth(1.0f);
    glPointSize(1.0f);

    sm.SetMouse( (float)mouse_x/(float)width*2.0f-1.0f, 1.0f-2.0f*(float)mouse_y/(float)height );
    if( sm.ProgressiveDraw( *output ) ){
        progressive_tex.CopyTexImage2D( GL_RGBA, 0, 0, size().width(), size().height() );
    }

    sm.ProgressiveBorder( *output );
    sm.ProgressiveLabels( aspect, *output );

    std::pair<int,int> sel = cur_selection;
    if(sel.first == -1 || sel.second == -1 ){
        sel = sm.GetSelected();
    }

    glLineWidth(4.0f);
    glPointSize(7.0f);

    sp.SetBorderWidth(3.0f);
    sp.SetBorderColor(0,0,0);
    sp.SetCenter( -0.56f, 0.52f );
    sp.SetSize(0.75f);
    sp.Set( *output, sel.first, sel.second, true );
    sp.SetAspect( aspect );
    sp.Draw();

    update();

}

void MainWidget::UpdateSelection( std::pair<int,int> sel ){
    cur_selection = sel;
}

void MainWidget::mouseDoubleClickEvent ( QMouseEvent * event ){

    int x = event->x();
    int y = size().height()-event->y()-1;

    // process event

    mouse_button = event->button();
    mouse_x = x;
    mouse_y = y;

}

void MainWidget::mouseMoveEvent ( QMouseEvent * event ){

    cur_selection = std::make_pair(-1,-1);

    int x = event->x();
    int y = size().height()-event->y()-1;

    mouse_x = x;
    mouse_y = y;

}

void MainWidget::mousePressEvent ( QMouseEvent * event ){

    int x = event->x();
    int y = size().height()-event->y()-1;

    sm.Select();

    mouse_button = event->button();
    mouse_x = x;
    mouse_y = y;

}

void MainWidget::mouseReleaseEvent ( QMouseEvent * event ){

    int x = event->x();
    int y = size().height()-event->y()-1;

    // process event

    mouse_button = Qt::NoButton;
    mouse_x = x;
    mouse_y = y;

}

void MainWidget::keyPressEvent ( QKeyEvent * ){ }
void MainWidget::keyReleaseEvent ( QKeyEvent * ){ }


