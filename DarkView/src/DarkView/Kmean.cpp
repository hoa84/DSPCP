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

#include <DarkView/Kmean.h>
#include <GL/oglCommon.h>

#include <QMouseEvent>
#include <iostream>

#include <SCI/Vex3.h>
#include <SCI/Vex4.h>

Kmean::Kmean(MainWidget *_mw, oglWidgets::oglFont & _font, QWidget * parent ) : QGLWidget( QGLFormat(QGL::SingleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::AlphaChannel | QGL::DirectRendering | QGL::SampleBuffers), parent )
{
    curveDegree = 3;
    eleRange = 0.1f;

    // Kmean
    numClusters = 8;

    data = 0;
    curDraw = 0;
    selected = -1;
    d_scale = 0.1f;
    font = &_font;
    started = false;
    setMouseTracking(true);
    mw = _mw;
    p0 = p1 = p2 = p3 = 0.0f;
    a = b = c = d = 0.0f;
    e = f = g = 0.0f;    
    dis = 0.1f;

    rangeV = 0.6f;

    // locate 1 item
    locItem = false;
    // locate a range of items
    locItemRange = false;
    xItem = 5.0f;
    yItem = 5.0f;
    selectedDim1 = 0;
    selectedDim2 = 0;
    selectedItem = -1;
    flip = 1;
    flip2 = 1;
    numCor = 0;
    itemStart = 0;
    itemEnd = 0;
    selLine = false;
    selLineRange = false;
    selectedx1 = selectedy1 = selectedx2 = selectedy2 = 0.0f;
    stopRange = false;
}

// This function to setup or update visual data (indr)
void Kmean::SetData( DataIndirector * _data )
{
    data = _data;

    // clear dimLoc before setup or update data
    dimLoc.clear();

    if(data != 0)
    {
        Reset();
        dim_min.clear();
        dim_max.clear();
        qua_a.clear();
        qua_b.clear();
        qua_c.clear();
        cub_a.clear();
        cub_b.clear();
        cub_c.clear();
        cub_d.clear();

        dim  = data->GetDim();
        //eleRange = 0.5f/dim;

        for(int d = 0; d < dim; d++)
        {
            float minv = data->GetMinimumValue(d);
            float maxv = data->GetMaximumValue(d);

            float range  = (maxv-minv)/2.0f;
            float center = (minv+maxv)/2.0f;

            if( minv == maxv )
            {
                range = 0.5f;
            }

            minv = center - range*1.01f;
            maxv = center + range*1.01f;

            dim_min.push_back(minv);
            dim_max.push_back(maxv);

            float loc = SCI::lerp( -1.0f, 1.0f, ((float)d+0.5f) / ((float)dim) );
            dimLoc.push_back( std::make_pair(loc,d) );
        }
    }
}

void Kmean::Reset()
{
    curDraw = 0;
}

void Kmean::initializeGL()
{
    curDraw = 0;
}

void Kmean::resizeGL ( int, int )
{
    curDraw = 0;
}

void Kmean::mousePressEvent ( QMouseEvent * event )
{
    float selpx = (float)event->pos().x() / (float)size().width() * 2.0f - 1.0f;
    float selpy = (float)event->pos().y() / (float)size().height() * 2.0f - 1.0f;

    int closest = 0;
    int closest_left = 0;
    int closest_right = 0;

    itemStart = 0;
    itemEnd = 0;

    for(int i = 1; i < (int)dimLoc.size(); i++)
    {
        dis = maxPoint[i];
        if( fabsf( dimLoc[i].first - dis - selpx ) < fabsf( dimLoc[closest].first - dis - selpx ) )
        {
            closest = i;
        }
    }

    // if dimension is selected
    if( fabsf( dimLoc[closest].first - maxPoint[closest] -selpx ) < 0.01f )
    {
        selected = closest;
        curDraw = 0;
    }    

    if(locItem == true || locItemRange == true)
    {
        for(int i = 1; i < (int)dimLoc.size(); i++)
        {
            dis = maxPoint[i];
            if( dimLoc[i].first - dis < selpx && fabsf( dimLoc[i].first - dis - selpx ) < fabsf( dimLoc[closest_left].first - dis - selpx ) )
            {
                closest_left = i;
            }
            if( dimLoc[i].first - dis > selpx && fabsf( dimLoc[i].first - dis - selpx ) < fabsf( dimLoc[closest_right].first - dis - selpx ) )
            {
                closest_right = i;
            }
        }

        xItem = selpx;
        yItem = selpy;
        selectedDim1 = closest_left;
        selectedDim2 = closest_right;

        selectedLine();
        itemStart = selectedItem;
    }   
    // if the locate item function is turned on
    if(locItem == true)
    {
        selLine = true;
        curDraw = 0;
    }
    if(locItemRange == true)
    {
        selLineRange = true;
        curDraw = 0;
        stopRange = false;
    }

}

void Kmean::mouseReleaseEvent ( QMouseEvent * event )
{

    float selpx = (float)event->pos().x() / (float)size().width() * 2.0f - 1.0f;
    float selpy = (float)event->pos().y() / (float)size().height() * 2.0f - 1.0f;

    int closest_left = 0;
    int closest_right = 0;

    if(selected != -1)
    {        
        dimLoc[ selected ].first = selpx;
        std::sort( dimLoc.begin(), dimLoc.end() );
        for(int i = 0; i < dim; i++)
        {
            dimLoc[i].first = SCI::lerp( -1.0f, 1.0f, ((float)i+0.5f) / ((float)dim) );
        }
        selected = -1;
        curDraw = 0;
        d_scale = 0.1f;
    }

    // if the locate item function is turned on -> lines range
    if(locItemRange == true)
    {
        for(int i = 1; i < (int)dimLoc.size(); i++)
        {
            dis = maxPoint[i];
            if( dimLoc[i].first - dis < selpx && fabsf( dimLoc[i].first - dis - selpx ) < fabsf( dimLoc[closest_left].first - dis - selpx ) )
            {
                closest_left = i;
            }
            if( dimLoc[i].first - dis > selpx && fabsf( dimLoc[i].first - dis - selpx ) < fabsf( dimLoc[closest_right].first - dis - selpx ) )
            {
                closest_right = i;
            }
        }

        xItem = selpx;
        yItem = selpy;
        selectedDim1 = closest_left;
        selectedDim2 = closest_right;

        selectedLine();
        itemEnd = selectedItem;
        stopRange = true;
    }

}

void Kmean::mouseMoveEvent ( QMouseEvent * event )
{

    float selpx = (float)event->pos().x() / (float)size().width() * 2.0f - 1.0f;
    float selpy = (float)event->pos().y() / (float)size().height() * 2.0f - 1.0f;

    std::pair<int,int> scatter_pair = std::make_pair(-1,-1);

    if(selected != -1)
    {
        dimLoc[ selected ].first = selpx;

        bool repeat;
        do {
            repeat = false;
            if( selected > 0 && dimLoc[ selected ].first < dimLoc[ selected-1 ].first )
            {
                std::swap( dimLoc[ selected ], dimLoc[ selected-1 ]);

                // swap two dimensions in visual data and update new data
                data->SwapDims(selected, selected-1);
                SetData(data);
                // reset main widget for scatter matrix plot
                mw->ProgressiveReset();

                selected--;
                repeat = true;
            }
            if( selected < (dim-1) && dimLoc[ selected ].first > dimLoc[ selected+1 ].first )
            {
                std::swap( dimLoc[ selected ], dimLoc[ selected+1 ]);

                // swap two dimensions in visual data and update new data
                data->SwapDims(selected, selected + 1);
                SetData(data);
                // reset main widget for scatter matrix plot
                mw->ProgressiveReset();

                selected++;
                repeat = true;
            }

        } while(repeat);

        for(int i = 0; i < dim; i++)
        {
            if( i != selected )
                dimLoc[i].first = SCI::lerp( -1.0f, 1.0f, ((float)i+0.5f) / ((float)dim) );
        }

        curDraw = 0;

        if( selected == ((int)dimLoc.size()-1) )
        {
            scatter_pair = std::make_pair(dimLoc[selected].second, dimLoc[selected-1].second);
        }
        else
        {
            scatter_pair = std::make_pair(dimLoc[selected+1].second, dimLoc[selected].second);
        }
    }
    else{
        int closest_left  = 0;
        int closest_right = (int)dimLoc.size()-1;
        for(int i = 1; i < (int)dimLoc.size()-1; i++)
        {
            if( dimLoc[i].first < selpx && fabsf( dimLoc[i].first-selpx ) < fabsf( dimLoc[closest_left].first-selpx ) )
            {
                closest_left = i;
            }
            if( dimLoc[i].first >= selpx && fabsf( dimLoc[i].first-selpx ) < fabsf( dimLoc[closest_right].first-selpx ) )
            {
                closest_right = i;
            }
        }

        scatter_pair = std::make_pair(dimLoc[closest_right].second, dimLoc[closest_left].second);
    }

    float closest_left, closest_right;
    // track the locate item range function
    if(locItemRange == true)
    {
        // if the locate item function is turned on -> lines range
        if(itemStart != 0 && stopRange == false)
        {
            for(int i = 1; i < (int)dimLoc.size(); i++)
            {
                dis = maxPoint[i];
                if( dimLoc[i].first - dis < selpx && fabsf( dimLoc[i].first - dis - selpx ) < fabsf( dimLoc[closest_left].first - dis - selpx ) )
                {
                    closest_left = i;
                }
                if( dimLoc[i].first - dis > selpx && fabsf( dimLoc[i].first - dis - selpx ) < fabsf( dimLoc[closest_right].first - dis - selpx ) )
                {
                    closest_right = i;
                }
            }

            xItem = selpx;
            yItem = selpy;
            selectedDim1 = closest_left;
            selectedDim2 = closest_right;

            selectedLine();
            itemEnd = selectedItem;
        }
    }
    emit UpdatedSelection( scatter_pair );

}

void Kmean::Start()
{
    started = true;
}

void Kmean::paintGL()
{
    glViewport( 0, 0, size().width(), size().height() );
    if( curDraw == 0 )
    {
        glClearColor( 1,1,1,1 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    if( !started )
    {
        update();
        return;
    }

    if(data == 0 )
    {
        update();
        return;
    }

    // draw PCP lines and labels
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -1, 1, -1, 1, -1, 1 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    if( curDraw == 0 )
    {
        // process clustering Kmean in data
        kMeanCluster();

        // Draw curve axis and find the max point of each curve
        if (curveDegree == 3)
        {
            // draw cubic axis
            cubicAxis();
        }
        if (curveDegree == 2)
        {
            // draw quadratic axis
            quadAxis();
        }
        // end of draw curve axis

        // draw grey PCP axis line from dim1 to dimN
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        for(int i = 0; i < dim; i++)
        {
            glColor3f(0.6f,0.6f,0.6f);
            if(i == selected) glColor3f(1,0,0);
            glVertex3f( dimLoc[i].first, -1, -0.8f );
            glVertex3f( dimLoc[i].first, 1, -0.8f );
        }
        glEnd();
        // end of draw PCP axis lines

        // draw PCP labels
        float aspect = (float)size().width()/(float)size().height();
        for(int d = 0; d < dim; d++)
        {
            glPushMatrix();
                glTranslatef( dimLoc[d].first+0.0075, 0.95f, 0 );
                glColor3f(0.6f,0.6f,0.6f);
                glScalef(0.02f,0.02f*aspect,0.1f);
                glRotatef(90,0,0,-1);
                int _d = dimLoc[d].second;
                font->Print( data->GetLabelParsed(_d).c_str() );
            glPopMatrix();
        }        
        // end of draw PCP labels
    }

    //glLineWidth(1.0f);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    glDepthFunc( GL_LEQUAL );

    float * space = new float[dim];

    // draw elements of PCP
    #if 0
        for( ; curDraw < data->GetElementCount()/5; curDraw+=5 )
        {
            data->GetElement( curDraw, space );
            DrawElement(space);
        }
        curDraw = 0;
    #else
        int step = SCI::Max( 1, data->GetElementCount() / 500 );

        if( curDraw == 0 )
        {
            for( ; curDraw < data->GetElementCount(); curDraw += step )
            {
                 data->GetElement( curDraw, space );
                 DrawElement(space);
            }
            curDraw = 1;
         }
         else
         {
             int fin = curDraw+500;
             for( ; curDraw < fin && curDraw < data->GetElementCount(); curDraw++ )
             {
                 if( (curDraw%step) == 0 ) continue;
                 data->GetElement( curDraw, space );
                 DrawElement(space);
             }
         }
     #endif

     delete [] space;
     glDisable( GL_BLEND );
     update();
     // end of drawing elements of PCP
 }

// draw elements of dimensions
void Kmean::DrawElement( float * elem )
{    
    glBegin(GL_LINES);
    glLineWidth(3.0f);
    glColor4f( 0, 0, 0, 0.025f );

    preCor = data->GetCorrelation(0,1);
    numCor = 0;

    flip = 1;
    flip2 = 1;
    for(int j = 0; j < (dim-1); j++)
    {
        int d0, d1;
        float x0, x1;
        // distance between curve axis and grey line axis is maxPoint of each curve
        dis = maxPoint[j+1];
        d0 = dimLoc[j].second;
        d1 = dimLoc[j+1].second;
        x0 = dimLoc[j].first;
        x1 = dimLoc[j+1].first;

        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float correlation = data->GetCorrelation( d0, d1 );
        float xt, yt;
        float err;

        if (preCor < 0 && correlation >= 0)
        {           
            flip = flip;
            flip2 = -flip2;
        }
        if (preCor < 0 && correlation < 0)
        {
            if(numCor%2 == 0)
            {
                flip = -1;
                flip2 = 1;
            }
            else
            {
                flip = 1;
                flip2 = -1;
            }
        }
        if (preCor >= 0 && correlation < 0)
        {
            flip = -flip;
            flip2 = flip2;
        }
        if (preCor >= 0 && correlation >= 0)
        {
            flip = flip;
            flip2 = flip2;
        }

        float xt1, yt1, yt0, xt0;
        if (curvePos[j] == 0)
        {
            xt1 = x1;
            yt1 = flip*y1 ;
            yt0 = y0;
            xt0 = x0;

            // draw elements follows fitting cubic curve
            if(correlation < 0 )
            {
                yt = flip2*yt0;
                numCor +=1;
            }
            if(correlation >= 0 )
            {
                yt = flip2*yt0;
                numCor = 0;
            }
        }
        if (curvePos[j] == 1)
        {
            xt1 = x0;
            yt1 = flip*y0;
            yt0 = y1;
            xt0 = x1;

            // draw elements follows fitting cubic curve
            if(correlation < 0 )
            {
                yt = flip2*yt0;
                numCor +=1;
            }
            if(correlation >= 0 )
            {
                yt = flip2*yt0;
                numCor = 0;
            }
        }

        if(curveDegree == 2)
            xt = qua_a[j] + qua_b[j]*yt + qua_c[j]*yt*yt + maxPoint[j];
        if(curveDegree == 3)
            xt = cub_a[j] + cub_b[j]*yt + cub_c[j]*yt*yt + cub_d[j]*yt*yt*yt + maxPoint[j];


        if ( correlation < 0 )
        {          
            err = xt - xt0 - (yt0+yt1)*eleRange;
            if(err < 0.000000000000000000000001f)
            {
                glVertex3f( xt, yt, -0.5f );
                glVertex3f( xt1 , yt1, -0.5f );
            }
            else
            {
                glVertex3f( xt, yt, -0.5f );
                glVertex3f( xt1, yt1, -0.5f );
            }
        }
        if ( correlation >= 0 )
        {
            err = xt - xt0 - (yt0-yt1)*eleRange;
            if(err < 0.000000000000000000000001f)
            {
                glVertex3f( xt, yt, -0.5f );
                glVertex3f( xt1, yt1, -0.5f );
            }
            else
            {
                glVertex3f( xt, yt, -0.5f );
                glVertex3f( xt1, yt1, -0.5f );
            }
        }

        // draw grey horizontal lines to fill the gap between curve axis and vertical line axis
        glColor4f( 0.6f, 0.6f, 0.6f, 0.025f );
        glVertex3f( xt0, yt, -0.9f );
        glVertex3f( xt, yt, -0.9f );

        preCor = correlation;
        // end of draw grapping lines
    }
    glEnd();
}

// draw cubic curve axis PCP
void Kmean::cubicAxis()
{    
    cub_a.clear();
    cub_b.clear();
    cub_c.clear();
    cub_d.clear();

    fitErr.clear();
    maxPoint.clear();
    curvePos.clear();

    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    float * elem = new float[dim];

    for(int j = 0; j < (dim-1); j++)
    {
        int   d0 = dimLoc[j].second;
        int   d1 = dimLoc[j+1].second;
        float x0 = dimLoc[j].first;

        maxPoint.push_back(0.0f);
        float erFit = cubicFitCurveKM(step, elem, d0, d1, x0);

        for(int i = 0; i < data->GetElementCount(); i += step )
        {
            data->GetElement( i, elem );
            float yt = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
            float xt = a + b*yt + c*yt*yt + d*yt*yt*yt;
            if ((x0 - xt) > maxPoint[j])
            {
                maxPoint[j] = x0 - xt;
            }
        }

        maxPoint[j] = 0.0f;
        curvePos.push_back(0);
        fitErr.push_back(erFit);

        cub_a.push_back(a);
        cub_b.push_back(b);
        cub_c.push_back(c);
        cub_d.push_back(d);
        // std::cout << "errFit = " << erFit << std::endl;

        // draw PCP curve
        cubicCurve(a, b, c, d, maxPoint[j]);

    }
    maxPoint.push_back(0.0f);
}

// draw quadratic curve axis PCP
void Kmean::quadAxis()
{
    qua_a.clear();
    qua_b.clear();
    qua_c.clear();

    fitErr.clear();
    maxPoint.push_back(0.0f);
    curvePos.clear();

    int step = SCI::Max( 1, data->GetElementCount() / 500 );    
    float * elem = new float[dim];

    for(int j = 0; j < (dim-1); j++)
    {
        int   d0 = dimLoc[j].second;
        int   d1 = dimLoc[j+1].second;
        float x0 = dimLoc[j].first;

        maxPoint.push_back(0.0f);
        float erFit = quadFitCurveKM(step, elem, d0, d1, x0);

        for(int i = 0; i < data->GetElementCount(); i += step )
        {
            data->GetElement( i, elem );
            float yt = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
            float xt = e + f*yt + g*yt*yt;
            if ((x0 - xt) > maxPoint[j])
            {
                maxPoint[j] = x0 - xt;
            }
        }

        maxPoint[j] = 0.0f;
        curvePos.push_back(0);
        fitErr.push_back(erFit);

        qua_a.push_back(e);
        qua_b.push_back(f);
        qua_c.push_back(g);
        // std::cout << "errFit = " << erFit << std::endl;

        // draw PCP curve
        quadCurve(e, f, g, maxPoint[j]);
    }
    maxPoint.push_back(0.0f);
}

// draw quadratic curve for quadAxis() function
void Kmean::quadCurve(float a, float b, float c, float maxpoint)
{
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(0,0,0);

    for (float t=-rangeV; t <= rangeV; t += 0.01f)
    {
        float yt = t;
        float xt = a + b*yt + c*yt*yt + maxpoint;
        glVertex3f(xt,yt,-0.5f);
    }
    glEnd();
}


// draw cubic curve
void Kmean::cubicCurve(float a, float b, float c, float d, float maxpoint)
{
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(0,0,0);

    for (float t=-rangeV; t <= rangeV; t += 0.01f)
    {
        float yt = t;
        float xt = a + b*yt + c*yt*yt + d*yt*yt*yt + maxpoint;
        glVertex3f(xt,yt,-0.5f);
    }
    glEnd();
}

// compute the distances between the point P(xp, yp)and a line y = m*x + n
float Kmean::disPLine(float xp, float yp, float m, float n)
{
    float dist = fabsf(yp - m*xp - n)/sqrtf(m*m+1);
    return dist;
}

// find the line that is closest with current point between selectedDim1 and selectedDim2
void Kmean::selectedLine()
{
    float * elem = new float[dim];
    int step = SCI::Max( 1, data->GetElementCount() / 500 );

    int j = selectedDim1;

    int   d0 = dimLoc[j].second;
    int   d1 = dimLoc[j+1].second;

    float x0 = dimLoc[j].first;
    float x1 = dimLoc[j+1].first;
    // distance between curve axis and grey line axis is maxPoint of each curve
    dis = maxPoint[j+1];

    float minDisPLine = 100.0f;
    float minDis1x, minDis1y;
    float minDis2x, minDis2y;

    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float correlation = data->GetCorrelation( d0, d1 );
        float xt, yt;

        // draw elements follows fitting cubic curve
        //yt = y0;
        if(correlation < 0 )
        {
            yt = flip2*y0;

        }
        if(correlation >= 0 )
        {
            yt = y0;
        }

        if(curveDegree == 2)
            xt = qua_a[j] + qua_b[j]*yt + qua_c[j]*yt*yt;
        if(curveDegree == 3)
            xt = cub_a[j] + cub_b[j]*yt + cub_c[j]*yt*yt + cub_d[j]*yt*yt*yt;

        float xa, ya, xb, yb;
        xa = xt;
        ya = yt;
        xb = x1 - dis;
        float err;

        if ( correlation < 0 )
        {
            err = xt - x0 - (y0+y1)*eleRange;
            if(err < 0.000000000000000000000001f)
                yb = flip*y1;
            else
                yb = flip*y1 + err;
        }
        if ( correlation >= 0 )
        {
            err = xt - x0 - (y0-y1)*eleRange;
            if(err < 0.000000000000000000000001f)
                yb = y1;
            else
                yb = y1 + err;
        }

        float xp = xItem;
        float yp = -yItem;
        float m = (ya - yb)/(xa - xb);
        float n = ya - m*xa;

        // compute the distance between chosen point and lines
        float dist = fabsf(yp - m*xp - n)/(sqrtf(m*m+1));

        // compute the shortest distance between point and the line
        if (dist < minDisPLine)
        {
            minDisPLine = dist;
            minDis1x = xa;
            minDis1y = ya;
            minDis2x = xb;
            minDis2y = yb;
            selectedItem = i;
        }
    }

    selectedy2 = minDis2y;
    selectedx1 = minDis1x;
    selectedy1 = minDis1y;
    selectedx2 = minDis2x;
}

// draw the selected lines
void Kmean::DrawSelectedLine()
{
    glBegin(GL_LINES);
    glLineWidth(3.0f);
    glColor3f(0,0,0);

    float * elem = new float[dim];

    preCor = data->GetCorrelation(0,1);
    numCor = 0;
    flip = 1;
    flip2 = 1;

    for(int j = 0; j < (dim-1); j++)
    {
        int   d0 = dimLoc[j].second;
        int   d1 = dimLoc[j+1].second;
        float correlation = data->GetCorrelation( d0, d1 );

        float x0 = dimLoc[j].first;
        float x1 = dimLoc[j+1].first;
        float xt, yt, err;
        dis = maxPoint[j+1];

        if (preCor < 0 && correlation >= 0)
        {
            flip = flip;
            flip2 = -flip2;
        }
        if (preCor < 0 && correlation < 0)
        {
            if(numCor%2 == 0)
            {
                flip = -1;
                flip2 = 1;
            }
            else
            {
                flip = 1;
                flip2 = -1;
            }
        }
        if (preCor >= 0 && correlation < 0)
        {
            flip = -flip;
            flip2 = flip2;
        }
        if (preCor >= 0 && correlation >= 0)
        {
            flip = flip;
            flip2 = flip2;
        }

        data->GetElement( selectedItem, elem );
        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

        //yt = y0;

        if(correlation < 0 )
        {
            yt = flip2*y0;
            numCor += 1;
        }
        if(correlation >= 0 )
        {
            yt = flip2*y0;
            numCor = 0;
        }

        if(curveDegree == 2)
            xt = qua_a[j] + qua_b[j]*yt + qua_c[j]*yt*yt;
        if(curveDegree == 3)
            xt = cub_a[j] + cub_b[j]*yt + cub_c[j]*yt*yt + cub_d[j]*yt*yt*yt;

        if ( correlation < 0 )
        {
            err = xt - x0 - (y0+y1)*eleRange;
            // change the color
            if(err < 0.000000000000000000000001f)
            {
                glVertex3f( xt, yt, -0.5f );
                glVertex3f( x1-dis, flip*y1, -0.5f );
            }
            else
            {
                glVertex3f( xt, yt, -0.5f );
                glVertex3f( x1-dis, flip*y1, -0.5f );
            }
        }
        if ( correlation >= 0 )
        {
            err = xt - x0  - (y0-y1)*eleRange;
            // change the color
            if(err < 0.000000000000000000000001f)
            {
                glVertex3f( xt, yt, -0.5f );
                glVertex3f( x1-dis, flip*y1, -0.5f );
            }
            else
            {
                glVertex3f( xt, yt, -0.5f );
                glVertex3f( x1-dis, flip*y1, -0.5f );
            }
        }

        preCor = correlation;
    }
    glEnd();

}

// compute cubic fitting curve for cubicAxis() function
float Kmean::cubicFitCurve(int step, float * elem, int d0, int d1, float x0)
{
    float C1 = 499.0f;
    float D1 = 0.0f;
    float E1 = 0.0f;
    float F1 = 0.0f;
    float G1 = 0.0f;
    float C2 = 0.0f;
    float D2 = 0.0f;
    float E2 = 0.0f;
    float F2 = 0.0f;
    float G2 = 0.0f;
    float C3 = 0.0f;
    float D3 = 0.0f;
    float E3 = 0.0f;
    float F3 = 0.0f;
    float G3 = 0.0f;
    float C4 = 0.0f;
    float D4 = 0.0f;
    float E4 = 0.0f;
    float F4 = 0.0f;
    float G4 = 0.0f;

    float correlation = data->GetCorrelation( d0, d1 );

    // finding the best fitting curve
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );

        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float y, x;

        // set the initial (x, y) of data points
        if (correlation < 0)
        {
            x = flip2*y0;
            y = x0 + (y0+y1)*eleRange;
        }
        else
        {
            x = y0;
            y = x0 + (y0-y1)*eleRange;
        }

        // cubic fitting curve
        D1 += x;
        E1 += x*x;
        F1 += x*x*x;
        G1 += y;
        C2 = D1;
        C3 = D2 = E1;
        C4 = D3 = E2 = F1;
        F2 += x*x*x*x;
        D4 = E3 = F2;
        F3 += x*x*x*x*x;
        E4 = F3;
        F4 += x*x*x*x*x*x;
        G2 += x*y;
        G3 += x*x*y;
        G4 += x*x*x*y;
    }

    float K2 = D1 -(C1/C2)*D2;
    float M2 = E1 -(C1/C2)*E2;
    float N2 = F1 -(C1/C2)*F2;
    float H2 = G1 -(C1/C2)*G2;

    float K3 = D1 -(C1/C3)*D3;
    float M3 = E1 -(C1/C3)*E3;
    float N3 = F1 -(C1/C3)*F3;
    float H3 = G1 -(C1/C3)*G3;

    float K4 = D1 -(C1/C4)*D4;
    float M4 = E1 -(C1/C4)*E4;
    float N4 = F1 -(C1/C4)*F4;
    float H4 = G1 -(C1/C4)*G4;

    float I3 = M2 -(K2/K3)*M3;
    float J3 = N2 -(K2/K3)*N3;
    float L3 = H2 -(K2/K3)*H3;

    float I4 = M2 -(K2/K4)*M4;
    float J4 = N2 -(K2/K4)*N4;
    float L4 = H2 -(K2/K4)*H4;

    d = (L3 - (I3/I4)*L4)/(J3 - (I3/I4)*J4);
    c = (L3 - J3*d)/I3;
    b = (H2 - N2*d - M2*c)/K2;
    a = (G1 - F1*d - E1*c - D1*b)/C1;

    // compute fitting error
    float errFit = 0.0f;
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );

        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float yt = y0;
        float xt = a + b*yt + c*yt*yt + d*yt*yt*yt;
        float err;

        // compute fitting error
        if ( correlation < 0 )
        {
            err = xt - x0 - (y0+y1)*eleRange;
        }
        if ( correlation >= 0 )
        {
            err = xt - x0 - (y0-y1)*eleRange;
        }
        errFit += err*err;
    }
    return errFit;
}

// compute quadratic fitting curve for quadAxis() function
float Kmean::quadFitCurve(int step, float * elem, int d0, int d1, float x0)
{
    float D1 = 499.0f;
    float E1 = 0.0f;
    float F1 = 0.0f;
    float G1 = 0.0f;
    float D2 = 0.0f;
    float E2 = 0.0f;
    float F2 = 0.0f;
    float G2 = 0.0f;
    float D3 = 0.0f;
    float E3 = 0.0f;
    float F3 = 0.0f;
    float G3 = 0.0f;

    float correlation = data->GetCorrelation( d0, d1 );

    // finding the best fitting curve
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float y, x;

        // set the initial (x, y) of data points
        if (correlation < 0)
        {
            x = flip2*y0;
            y = x0 + (y0+y1)*eleRange;
        }
        else
        {
            x = y0;
            y = x0 + (y0-y1)*eleRange;
        }

        // quadratic fitting curve
        E1 += x;
        D2 = E1;
        F1 += x*x;
        D3 = E2 = F1;
        F2 += x*x*x;
        E3 = F2;
        F3 += x*x*x*x;
        G1 += y;
        G2 += x*y;
        G3 += x*x*y;
    }

    float K2 = E1 - (D1/D2)*E2;
    float M2 = F1 - (D1/D2)*F2;
    float N2 = G1 - (D1/D2)*G2;

    float K3 = E1 - (D1/D3)*E3;
    float M3 = F1 - (D1/D3)*F3;
    float N3 = G1 - (D1/D3)*G3;

    g = (N2-(K2/K3)*N3)/(M2-(K2/K3)*M3);
    f = (N2 - M2*g)/K2;
    e = (G1 - F1*g - E1*f)/D1;

    float errFit = 0.0f;
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float yt = y0;
        float xt = e + f*yt + g*yt*yt;
        float err;

        // compute fitting error
        if ( correlation < 0 )
        {
            err = xt - x0 - (y0+y1)*eleRange;
        }
        if ( correlation >= 0 )
        {
            err = xt - x0 - (y0-y1)*eleRange;
        }
        errFit += err*err;
    }
    return errFit;
}


//find the minimum and maximum of 1 dimension in data.
void Kmean::minmaxItem(int j)
{
    int   d0 = dimLoc[j].second;
    int   d1 = dimLoc[j+1].second;
    float x0 = dimLoc[j].first;

    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    float * elem = new float[dim];

    xyElement(elem, 0, d0, d1, x0);
    float minx = xe;
    float miny = ye;
    float maxx = xe;
    float maxy = ye;
    int indMin = 0;
    int indMax = 0;

    for(int i = step; i < data->GetElementCount(); i += step )
    {
        xyElement(elem, i, d0, d1, x0);

        if( xe < minx)
        {
            minx = xe;
            miny = ye;
            indMin = i;
        }

        if (xe > maxx)
        {
            maxx = xe;
            maxy = ye;
            indMax = i;
        }

        indMinElem = indMin;
        minElemX = minx;
        minElemY = miny;

        indMaxElem = indMax;
        maxElemX = maxx;
        maxElemY = maxy;
    }
}


// compute distance between (x1, y1) and (x2, y2)
float Kmean::dist(float x1, float y1, float x2, float y2)
{
    float di;
    di = sqrtf((y1 - y2)*(y1 - y2) + (x1 - x2)*(x1 - x2));
    return di;
}


// compute x, y based on element
void Kmean::xyElement(float * elem, int i, int d0, int d1, float x0)
{
    data->GetElement( i, elem );
    float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
    float y, x;
    float correlation = data->GetCorrelation(d0, d1);
    // set the initial (x, y) of data points
    if (correlation < 0)
    {
        x = flip2*y0;
        y = x0 + (y0+y1)*eleRange;
    }
    else
    {
        x = y0;
        y = x0 + (y0-y1)*eleRange;
    }
    xe = x;
    ye = y;
}


// find clustering
void Kmean::kMeanCluster()
{
    // mininum value to beat (default value is bigger than data range)
    float bnum = 10000000.0f;
    float minum = bnum;
    // the current mininum value
    float distance = 0.0f;
    int cluster = 0;
    bool isStillMoving = true;

    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    float * elem = new float[dim];

    int j = 0;
    int   d0 = dimLoc[j].second;
    int   d1 = dimLoc[j+1].second;        
    float x0 = dimLoc[j].first;

    // start to find clustering for dim[j]
    // initilize dataset and centroid
    clusters.clear();
    centroid.clear();

    // find the minimum and maximum element of dimension dim[j]
    minmaxItem(j);
    if (numClusters == 0 || numClusters == 1)
        centroid.push_back(std::make_pair((maxElemX+minElemX)/2, (maxElemY+minElemY)/2));

    if(numClusters == 2)
    {
        centroid.push_back(std::make_pair(minElemX, minElemY));
        centroid.push_back(std::make_pair(maxElemX, maxElemY));
    }

    if (numClusters > 2)
    {
        centroid.push_back(std::make_pair(minElemX, minElemY));

        for (int m = 2; m < numClusters; m++)
        {
            float xcen = (m-1)*(maxElemX+minElemX)/(numClusters-1);
            float ycen = (m-1)*(maxElemY+minElemY)/(numClusters-1);
            centroid.push_back(std::make_pair(xcen, ycen));
        }
        centroid.push_back(std::make_pair(maxElemX, maxElemY));
    }


    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        xyElement(elem, i, d0, d1, x0);

        minum = bnum;            

        for(int k = 0; k < numClusters; k++)
        {
            distance = dist(xe, ye, centroid[k].first, centroid[k].second);
            if(distance < minum)
            {
                minum = distance;
                cluster = k;
            }
        }
        clusters.push_back(cluster);

        // calculate new centroids
        for(int k = 0; k < numClusters; k++)
        {
            float totalX = 0;
            float totalY = 0;
            int totalInCluster = 0;
            for(int m = 0; m < i; m += step )
            {
                xyElement(elem, m, d0, d1, x0);
                if(clusters[m] == k)
                {
                    totalX += xe;
                    totalY += ye;
                    totalInCluster += 1;
                }
            }
            if(totalInCluster > 0)
            {
                centroid[k].first = totalX/totalInCluster;
                centroid[k].second = totalY/totalInCluster;
            }
        }
    }

    // keep shifting centroidss until equilibrium occurs
    while(isStillMoving)
    {
        // calculate new centroids
        for (int k = 0; k < numClusters; k++)
        {
            float totalX = 0;
            float totalY = 0;
            int totalInCluster = 0;

            for(int i = 0; i < data->GetElementCount(); i+=step)
            {
                xyElement(elem, i, d0, d1, x0);
                if(clusters[i] == k)
                {
                    totalX += xe;
                    totalY += ye;
                    totalInCluster += 1;
                }
            }
            if(totalInCluster > 0)
            {
                centroid[k].first = totalX/totalInCluster;
                centroid[k].second = totalY/totalInCluster;
            }
        }

        // assign all data to the new centroids
        isStillMoving = false;

        for(int i = 0; i < data->GetElementCount(); i+=step)
        {
            xyElement(elem, i, d0, d1, x0);
            minum = bnum;
            for(int k = 0; k < numClusters; k++)
            {
                distance = dist(xe, ye, centroid[k].first, centroid[k].second);
                if(distance < minum)
                {
                    minum = distance;
                    cluster = k;
                }
            }
            clusters[i] = cluster;
            if(clusters[i] != cluster)
            {
                clusters[i] = cluster;
                isStillMoving = true;
            }
        }
    }

    // draw the clustering result
    for (int k = 0; k < numClusters; k++)
    {
        // draw cluster points
        for(int i = 0; i < data->GetElementCount(); i+=step)
        {
            xyElement(elem, i, d0, d1, x0);
            glPointSize( 5.0 );
            glBegin(GL_POINTS);

            if (clusters[i] == 0)
            {
                glPointSize( 5.0 );
                glBegin(GL_POINTS);
                glColor4f( 1, 0.8, 0, 0.025f );
                glVertex3f(ye, xe, -0.5f);
                glEnd();
            }
            if (clusters[i] == 1)
            {
                glPointSize( 5.0 );
                glBegin(GL_POINTS);
                glColor4f(0.0f, 1.0f, 0.0f, 0.025f);
                glVertex3f(ye, xe, -0.5f);
                glEnd();
            }
            if (clusters[i] == 2)
            {
                glPointSize( 5.0 );
                glBegin(GL_POINTS);
                glColor4f(0.0f, 1.0f, 1.0f, 0.025f);
                glVertex3f(ye, xe, -0.5f);
                glEnd();
            }
            if (clusters[i] == 3)
            {
                glPointSize( 5.0 );
                glBegin(GL_POINTS);
                glColor4f(0.0f, 0.0f, 1.0f, 0.025f);
                glVertex3f(ye, xe, -0.5f);
                glEnd();
            }
            if (clusters[i] == 4)
            {
                glPointSize( 5.0 );
                glBegin(GL_POINTS);
                glColor4f(0.7f, 0.5f, 0.0f, 0.025f);
                glVertex3f(ye, xe, -0.5f);
                glEnd();
            }

            if(clusters[i] > 4)
            {
                SCI::Vex3 cor_color;
                cor_color = (clusters[i])*SCI::Vex3(0.1f,0.1f,0.01f);
                glColor3f( cor_color.x, cor_color.y, cor_color.z );

                glVertex3f(ye, xe, -0.5f);
                glEnd();
            }
        }
        // draw centroids
        glPointSize( 8.0 );
        glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(centroid[k].second, centroid[k].first, -0.2f);
        glEnd();
    }
}


// compute cubic fitting curve based on Kmean centroid points for cubicAxis() function
float Kmean::cubicFitCurveKM(int step, float * elem, int d0, int d1, float x0)
{
    float C1 = 499.0f;
    float D1 = 0.0f;
    float E1 = 0.0f;
    float F1 = 0.0f;
    float G1 = 0.0f;
    float C2 = 0.0f;
    float D2 = 0.0f;
    float E2 = 0.0f;
    float F2 = 0.0f;
    float G2 = 0.0f;
    float C3 = 0.0f;
    float D3 = 0.0f;
    float E3 = 0.0f;
    float F3 = 0.0f;
    float G3 = 0.0f;
    float C4 = 0.0f;
    float D4 = 0.0f;
    float E4 = 0.0f;
    float F4 = 0.0f;
    float G4 = 0.0f;
    float x, y;
    float correlation = data->GetCorrelation(d0, d1);

    // finding the best fitting curve
    for(int i = 0; i < numClusters; i += 1 )
    {
        y = centroid[i].second;
        x = centroid[i].first;

        // cubic fitting curve
        D1 += x;
        E1 += x*x;
        F1 += x*x*x;
        G1 += y;
        C2 = D1;
        C3 = D2 = E1;
        C4 = D3 = E2 = F1;
        F2 += x*x*x*x;
        D4 = E3 = F2;
        F3 += x*x*x*x*x;
        E4 = F3;
        F4 += x*x*x*x*x*x;
        G2 += x*y;
        G3 += x*x*y;
        G4 += x*x*x*y;
    }

    float K2 = D1 -(C1/C2)*D2;
    float M2 = E1 -(C1/C2)*E2;
    float N2 = F1 -(C1/C2)*F2;
    float H2 = G1 -(C1/C2)*G2;

    float K3 = D1 -(C1/C3)*D3;
    float M3 = E1 -(C1/C3)*E3;
    float N3 = F1 -(C1/C3)*F3;
    float H3 = G1 -(C1/C3)*G3;

    float K4 = D1 -(C1/C4)*D4;
    float M4 = E1 -(C1/C4)*E4;
    float N4 = F1 -(C1/C4)*F4;
    float H4 = G1 -(C1/C4)*G4;

    float I3 = M2 -(K2/K3)*M3;
    float J3 = N2 -(K2/K3)*N3;
    float L3 = H2 -(K2/K3)*H3;

    float I4 = M2 -(K2/K4)*M4;
    float J4 = N2 -(K2/K4)*N4;
    float L4 = H2 -(K2/K4)*H4;

    d = (L3 - (I3/I4)*L4)/(J3 - (I3/I4)*J4);
    c = (L3 - J3*d)/I3;
    b = (H2 - N2*d - M2*c)/K2;
    a = (G1 - F1*d - E1*c - D1*b)/C1;

    // compute fitting error
    float errFit = 0.0f;
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );

        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float yt = y0;
        float xt = a + b*yt + c*yt*yt + d*yt*yt*yt;
        float err;

        // compute fitting error
        if ( correlation < 0 )
        {
            err = xt - x0 - (y0+y1)*eleRange;
        }
        if ( correlation >= 0 )
        {
            err = xt - x0 - (y0-y1)*eleRange;
        }
        errFit += err*err;
    }
    return errFit;
}


// compute quadratic fitting curve for quadAxis() function
float Kmean::quadFitCurveKM(int step, float * elem, int d0, int d1, float x0)
{
    float D1 = 499.0f;
    float E1 = 0.0f;
    float F1 = 0.0f;
    float G1 = 0.0f;
    float D2 = 0.0f;
    float E2 = 0.0f;
    float F2 = 0.0f;
    float G2 = 0.0f;
    float D3 = 0.0f;
    float E3 = 0.0f;
    float F3 = 0.0f;
    float G3 = 0.0f;
    float correlation = data->GetCorrelation( d0, d1 );
    float x, y;

    // finding the best fitting curve
    for(int i = 0; i < numClusters; i += 1 )
    {
        y = centroid[i].second;
        x = centroid[i].first;

        // quadratic fitting curve
        E1 += x;
        D2 = E1;
        F1 += x*x;
        D3 = E2 = F1;
        F2 += x*x*x;
        E3 = F2;
        F3 += x*x*x*x;
        G1 += y;
        G2 += x*y;
        G3 += x*x*y;
    }

    float K2 = E1 - (D1/D2)*E2;
    float M2 = F1 - (D1/D2)*F2;
    float N2 = G1 - (D1/D2)*G2;

    float K3 = E1 - (D1/D3)*E3;
    float M3 = F1 - (D1/D3)*F3;
    float N3 = G1 - (D1/D3)*G3;

    g = (N2-(K2/K3)*N3)/(M2-(K2/K3)*M3);
    f = (N2 - M2*g)/K2;
    e = (G1 - F1*g - E1*f)/D1;

    float errFit = 0.0f;
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float yt = y0;
        float xt = e + f*yt + g*yt*yt;
        float err;

        // compute fitting error
        if ( correlation < 0 )
        {
            err = xt - x0 - (y0+y1)*eleRange;
        }
        if ( correlation >= 0 )
        {
            err = xt - x0 - (y0-y1)*eleRange;
        }
        errFit += err*err;
    }
    return errFit;
}
