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

#include <DarkView/Scatter.h>
#include <GL/oglCommon.h>

#include <QMouseEvent>
#include <iostream>

#include <SCI/Vex3.h>
#include <SCI/Vex4.h>

#include <dlib/optimization.h>
#include <iostream>
#include <vector>

std::vector< std::pair<float,float> > xydim;

float am1, bm1, cm1, dm1;
float am2, bm2, cm2, dm2;
float am3, bm3, cm3, dm3;

float para0, para1, para2, para3;

using namespace std;
using namespace dlib;

typedef matrix<double,2,1> input_vector;
typedef matrix<double,4,1> parameter_vector;

Scatter::Scatter(MainWidget *_mw, oglWidgets::oglFont & _font, QWidget * parent ) : QGLWidget( QGLFormat(QGL::SingleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::AlphaChannel | QGL::DirectRendering | QGL::SampleBuffers), parent )
{
    fitMeth = 0;

    curveDegree = 3;

    // = 0 for fitting curve drawing
    // = 1 for user draw with default curve
    // = 2 for user free draw curve
    userDefaultDraw = 2;
    // = 1 for user free draw curve
    userFreeDraw = 0;

    // value of k in knn method
    knum =  30;

    // number of segments
    nseg = 100;

    rangeV = 0.8f;

    data = 0;
    curDraw = 0;
    selected = -1;
    d_scale = 0.1f;
    font = &_font;
    started = false;
    setMouseTracking(true);
    mw = _mw;
    a = b = c = d = 0.0f;
    e = f = g = 0.0f;
    // no scale
    eleRange = 1.0f;

    sDim1 = -1;
    sDim2 = -1;
    selectedPair = -1;
    drawDV = false;

    // control points
    xp0 = xp1 = xp2 = xp3 = 0.0f;
    yp0 = yp1 = yp2 = yp3 = 0.0f;

    ctrlSelected = -1;
}


// ----------------------------------------------------------------------------------------

// We will use this function to generate data.  It represents a function of 2 variables
// and 3 parameters.   The least squares procedure will be used to infer the values of
// the 3 parameters based on a set of input/output pairs.
double model(const input_vector& input,const parameter_vector& params)
{
    const double p0 = params(0);
    const double p1 = params(1);
    const double p2 = params(2);
    const double p3 = params(3);

    const double i0 = input(0);
    const double i1 = input(1);

    //const double temp = p0*i0 + p1*i1 + p2;

    const double temp = -i0 + p0 + p1*i1 + p2*i1*i1 + p3*i1*i1*i1;

    //return temp*temp;
    return temp;
}
// ----------------------------------------------------------------------------------------
// This function is the "residual" for a least squares problem.   It takes an input/output
// pair and compares it to the output of our model and returns the amount of error.  The idea
// is to find the set of parameters which makes the residual small on all the data pairs.
double residual (const std::pair<input_vector, double>& data, const parameter_vector& params)
{
    return model(data.first, params) - data.second;
}
// ----------------------------------------------------------------------------------------
// This function is the derivative of the residual() function with respect to the parameters.
parameter_vector residual_derivative (const std::pair<input_vector, double>& data, const parameter_vector& params)
{
    parameter_vector der;

    const double p0 = params(0);
    const double p1 = params(1);
    const double p2 = params(2);
    const double p3 = params(3);

    const double i0 = data.first(0);
    const double i1 = data.first(1);

    //const double temp = p0*i0 + p1*i1 + p2;
    const double temp = -i0 + p0 + p1*i1 + p2*i1*i1 + p3*i1*i1*i1;

    der(0) = i0*2*temp;
    der(1) = i1*2*temp;
    der(2) = 2*temp;

    return der;
}
// ----------------------------------------------------------------------------------------
void LSmain()
{
    try
    {
        parameter_vector params;

        // dim7 and dim25
        /*
        params(0) = 0.335163;
        params(1) = 0.528832;
        params(2) = -0.564344;
        params(3) = -0.102714;
        */

        /*
        // dim3 and dim19
        params(0) = -0.439854;
        params(1) = 0.612193;
        params(2) = 1.09845;
        params(3) = -2.13234;
        */

        params(0) = para0;
        params(1) = para1;
        params(2) = para2;
        params(3) = para3;

        cout << "params: " << trans(params) << endl;

        // Now lets generate a bunch of input/output pairs according to our model.
        std::vector<std::pair<input_vector, double> > data_samples;
        input_vector input;

        for (int i = 0; i < 499; ++i)
        {
            //input = 10*randm(2,1);

            //xydim.push_back(make_pair(input(0), input(1)));
            input(0) = xydim[i].first;
            input(1) = xydim[i].second;

            const double output = model(input, params);

            // save the pair
            data_samples.push_back(make_pair(input, output));

        }

        // Before we do anything, lets make sure that our derivative function defined above matches
        // the approximate derivative computed using central differences (via derivative()).
        // If this value is big then it means we probably typed the derivative function incorrectly.
        cout << "derivative error: " << length(residual_derivative(data_samples[0], params) -
                                               derivative(&residual)(data_samples[0], params) ) << endl;

        // Now lets use the solve_least_squares_lm() routine to figure out what the
        // parameters are based on just the data_samples.
        parameter_vector x;
        x = 1;

        cout << "Use Levenberg-Marquardt" << endl;
        // Use the Levenberg-Marquardt method to determine the parameters which
        // minimize the sum of all squared residuals.
        solve_least_squares_lm(objective_delta_stop_strategy(1e-7).be_verbose(),
                               &residual,
                               &residual_derivative,
                               data_samples,
                               x);

        am1 = x(0);
        bm1 = x(1);
        cm1 = x(2);
        dm1 = x(3);

        // Now x contains the solution.  If everything worked it will be equal to params.
        cout << "inferred parameters: "<< trans(x) << endl;
        cout << "solution error:      "<< length(x - params) << endl;
        cout << endl;

        x = 1;
        cout << "Use Levenberg-Marquardt, approximate derivatives" << endl;
        // If we didn't create the residual_derivative function then we could
        // have used this method which numerically approximates the derivatives for you.
        solve_least_squares_lm(objective_delta_stop_strategy(1e-7).be_verbose(),
                               &residual,
                               derivative(&residual),
                               data_samples,
                               x);

        am2 = x(0);
        bm2 = x(1);
        cm2 = x(2);
        dm2 = x(3);

        // Now x contains the solution.  If everything worked it will be equal to params.
        cout << "inferred parameters: "<< trans(x) << endl;
        cout << "solution error:      "<< length(x - params) << endl;
        cout << endl;

        x = 1;
        cout << "Use Levenberg-Marquardt/quasi-newton hybrid" << endl;
        // This version of the solver uses a method which is appropriate for problems
        // where the residuals don't go to zero at the solution.  So in these cases
        // it may provide a better answer.
        solve_least_squares(objective_delta_stop_strategy(1e-7).be_verbose(),
                            &residual,
                            &residual_derivative,
                            data_samples,
                            x);

        am3 = x(0);
        bm3 = x(1);
        cm3 = x(2);
        dm3 = x(3);

        // Now x contains the solution.  If everything worked it will be equal to params.
        cout << "inferred parameters: "<< trans(x) << endl;
        cout << "solution error:      "<< length(x - params) << endl;

        //cout << "tes = " << tes << endl;

    }
    catch (std::exception& e)
    {
        cout << e.what() << endl;
    }
}

// This function to setup or update visual data (indr)
void Scatter::SetData( DataIndirector * _data )
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

        float xx0 = dimLoc[0].first;
        float xx1 = dimLoc[1].first;

        yp0 = (xx0 + xx1)/2;
        xp0 = xx0;

        yp1 = xx0+(xx1 - xx0)/3;
        xp1 = xx0+(xx1 - xx0)/3;

        yp2 = xx0 + 2*(xx1 - xx0)/3;
        xp2 = xx0 + 2*(xx1 - xx0)/3;

        yp3 = (xx0 + xx1)/2;
        xp3 = xx1;

    }
}

void Scatter::Reset()
{
    curDraw = 0;
}

void Scatter::initializeGL()
{
    curDraw = 0;
}

void Scatter::resizeGL ( int, int )
{
    curDraw = 0;
}

void Scatter::mousePressEvent ( QMouseEvent * event )
{
    float selpx = (float)event->pos().x() / (float)size().width() * 2.0f - 1.0f;
    float selpy = -((float)event->pos().y() / (float)size().height() * 2.0f - 1.0f);

    /*
    int closest = 0;

    for(int i = 1; i < (int)dimLoc.size(); i++)
    {
        if( fabsf( dimLoc[i].first - selpx ) < fabsf( dimLoc[closest].first - selpx ) )
        {
            closest = i;
        }
    }

    // if dimension is selected
    if( fabsf( dimLoc[closest].first - selpx ) < 0.01f )
    {
        selected = closest;
        curDraw = 0;
    }

    // find selected Dim1 and Dim2
    int closest_leftx  = 0;
    int closest_rightx = (int)dimLoc.size()-1;
    for(int i = 1; i < (int)dimLoc.size()-1; i++)
    {
        if( dimLoc[i].first < selpx && fabsf( dimLoc[i].first-selpx ) < fabsf( dimLoc[closest_leftx].first-selpx ) )
        {
            closest_leftx = i;
        }
        if( dimLoc[i].first >= selpx && fabsf( dimLoc[i].first-selpx ) < fabsf( dimLoc[closest_rightx].first-selpx ) )
        {
            closest_rightx = i;
        }
    }

    int closest_lefty  = 0;
    int closest_righty = (int)dimLoc.size()-1;
    for(int i = 1; i < (int)dimLoc.size()-1; i++)
    {
        if( dimLoc[i].first < selpy && fabsf( dimLoc[i].first-selpy ) < fabsf( dimLoc[closest_lefty].first-selpy ) )
        {
            closest_lefty = i;
        }
        if( dimLoc[i].first >= selpy && fabsf( dimLoc[i].first-selpy ) < fabsf( dimLoc[closest_righty].first-selpy ) )
        {
            closest_righty = i;
        }
    }

    sDim1 = (int)dimLoc.size() - 2 - closest_lefty;
    sDim2 = closest_rightx;

    if(sDim1 != -1 && sDim2 != -1)
    {
        selectedPair = 1;
    }
    */


    // selected control point is PC0
    if (fabsf(selpx - xp0) < 0.1f && fabsf(selpy - yp0) < 0.1f)
    {
        ctrlSelected = 0;
        curDraw = 0;
    }
    if (fabsf(selpx - xp1) < 0.1f && fabsf(selpy - yp1) < 0.1f)
    {
        ctrlSelected = 1;
        curDraw = 0;
    }
    if (fabsf(selpx - xp2) < 0.1f && fabsf(selpy - yp2) < 0.1f)
    {
        ctrlSelected = 2;
        curDraw = 0;
    }
    if (fabsf(selpx - xp3) < 0.1f && fabsf(selpy - yp3) < 0.1f)
    {
        ctrlSelected = 3;
        curDraw = 0;
    }

    // userFreeDraw = 1
    if (userDefaultDraw == 2)
    {
        userPoints.clear();
        userFreeDraw = 1;
        //curPointX = selpx;
        //curPointY = -selpy;
        curDraw = 0;
    }

}

void Scatter::mouseReleaseEvent ( QMouseEvent * event )
{    
    float selpx = (float)event->pos().x() / (float)size().width() * 2.0f - 1.0f;
    float selpy = -((float)event->pos().y() / (float)size().height() * 2.0f - 1.0f);

    /*
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

    if (selectedPair != -1)
    {
        drawDV = true;
        selectedPair = -1;
        curDraw = 0;
    }
    */
    if(ctrlSelected == 0)
    {
        xp0 = selpx;
        yp0 = selpy;
        ctrlSelected = -1;
        curDraw = 0;
    }
    if(ctrlSelected == 1)
    {
        xp1 = selpx;
        yp1 = selpy;
        ctrlSelected = -1;
        curDraw = 0;
    }
    if(ctrlSelected == 2)
    {
        xp2 = selpx;
        yp2 = selpy;
        ctrlSelected = -1;
        curDraw = 0;
    }
    if(ctrlSelected == 3)
    {
        xp3 = selpx;
        yp3 = selpy;
        ctrlSelected = -1;
        curDraw = 0;
    }

    if(userFreeDraw == 1)
    {
        curPointX = selpx;
        curPointY = selpy;
        userFreeDraw = 0;
    }
}

void Scatter::mouseMoveEvent ( QMouseEvent * event )
{
    float selpx = (float)event->pos().x() / (float)size().width() * 2.0f - 1.0f;
    float selpy = (float)event->pos().y() / (float)size().height() * 2.0f - 1.0f;

    if(userFreeDraw == 1)
    {
        curPointX = selpx;
        curPointY = -selpy;
        curDraw = 0;
        userPoints.push_back(std::make_pair(curPointX, curPointY));
    }

    /*
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

    emit UpdatedSelection( scatter_pair );
    */

}

void Scatter::Start()
{
    started = true;
}

void Scatter::paintGL()
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
        knn_pca();
        // tam
        /*
        // draw fitting curve using the library dlib
        float distan = fabsf(dimLoc[0].first - dimLoc[1].first);
        int d0, d1;
        float x0, x1;
        d0 = dimLoc[0].second;
        d1 = dimLoc[1].second;
        x0 = dimLoc[0].first;
        x1 = dimLoc[1].first;

        int step = SCI::Max( 1, data->GetElementCount() / 500 );
        float * elem = new float[dim];
        xydim.clear();

        // update input (x,y) for LSmain()
        for(int i = 0; i < data->GetElementCount(); i += step )
        {
            data->GetElement( i, elem );
            float y0 = SCI::lerp(x0, x0+distan, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
            float y1 = SCI::lerp(x1-distan, x1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
            float x, y;
            x = y1;
            y = y0;
            xydim.push_back(make_pair(x, y));
        }

        // update params (para0, para1, para2, para3) for LSmain()
        float erFit = cubicFitCurve(step, elem, d0, d1, x0, x0+distan, x1-distan, x1);

        float aa1 = a;
        float bb1 = b;
        float cc1 = c;
        float dd1 = d;

        float erFit1 = cubicFitCurve(step, elem, d1, d0, x0, x0+distan, x1-distan, x1);

        if (erFit < erFit1)
        {
            a = aa1;
            b = bb1;
            c = cc1;
            d = dd1;
        }

        para0 = a;
        para1 = b;
        para2 = c;
        para3 = d;

        LSmain();
        */

        if(userFreeDraw == 1)
        {
            glPointSize( 5.0 );
            glBegin(GL_POINTS);
            glColor3f(1,0,0);

            for(int i = 0; i < ((int)userPoints.size()-1); i++)
                glVertex3f( userPoints[i].first, userPoints[i].second, 0.0f);

            glEnd();
        }

        // tam

        // Draw curve axis and find the max point of each curve
        if(userDefaultDraw == 0)
        {
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
        }
        // end of draw curve axis

        // draw grey PCP axis line from dim1 to dimN

        for(int i = 0; i < (dim-1); i++)
        {
            glColor3f(0.6f,0.6f,0.6f);
            if(i == selected) glColor3f(1,0,0);
            float xx0 = dimLoc[i].first;
            float xx1 = dimLoc[i+1].first;
            float correlation = data->GetCorrelation(dimLoc[i].second, dimLoc[i+1].second);

            glLineWidth(3.0f);
            glBegin(GL_LINES);

            glVertex3f( xx0, xx0, -0.8f );
            glVertex3f( xx0, xx1, -0.8f );

            glVertex3f( xx0, xx1, -0.8f );
            glVertex3f( xx1, xx1, -0.8f );

            glEnd();

            // temp - draw curves for users
            // draw control line with curves
            if(userDefaultDraw == 1)
            {
                glPointSize( 5.0 );
                glBegin(GL_POINTS);
                glColor3f(0,0,0);
                glVertex3f( xp0, yp0,0.0f);
                glVertex3f( xp1, yp1,0.0f);
                glVertex3f( xp2, yp2,0.0f);
                glVertex3f( xp3, yp3,0.0f);
                glEnd();
                // end of draw control line with curves

                // draw default curve for user
                glBegin(GL_LINE_STRIP);
                SCI::Vex3 cor_color;
                if( correlation > 0 ) cor_color = SCI::Vex3(1,0,0) * ( correlation );
                if( correlation < 0 ) cor_color = SCI::Vex3(0,0,1) * (-correlation );
                glColor3f( cor_color.x, cor_color.y, cor_color.z );

                // draw curve from (xp1, yp1) to (xp2, yp2)
                for (float t=0;t<=1;t += 0.01f)
                {
                    float xt = CubicComp(t, xp0, xp1, xp2, xp3);
                    float yt = CubicComp(t, yp0, yp1, yp2, yp3);

                    glVertex3f(xt,yt,-0.5f);
                }
                glEnd();

                glBegin(GL_LINE_STRIP);
                glColor3f( cor_color.x, cor_color.y, cor_color.z );

                // draw curve from (xp0, yp0) to (xp1, yp1)
                for (float t=0;t<=1;t += 0.01f)
                {
                    float xt = CubicComp(t, xp2, xp0, xp1, xp3);
                    float yt = CubicComp(t, yp2, yp0, yp1, yp3);

                    glVertex3f(xt,yt,-0.5f);
                }
                glEnd();

                glBegin(GL_LINE_STRIP);
                glColor3f( cor_color.x, cor_color.y, cor_color.z );

                // draw curve from (xp2, yp2) to (xp3, yp3)
                for (float t=0;t<=1;t += 0.01f)
                {
                    float xt = CubicComp(t, xp0, xp2, xp3, xp1);
                    float yt = CubicComp(t, yp0, yp2, yp3, yp1);

                    glVertex3f(xt,yt,-0.5f);
                }
                glEnd();
            }
            // end of drawing curves for users

            for (int j = i; j < dim; j++)
            {
                float dd0 = dimLoc[i].first;
                float dd1 = dimLoc[j].first;

                glLineWidth(3.0f);
                glBegin(GL_LINES);
                glColor3f(0.6f,0.6f,0.6f);
                if(i == selected) glColor3f(1,0,0);

                glVertex3f( dd1, dd1, -0.8f );
                glVertex3f( dd1, dd0, -0.8f );

                glVertex3f( dd1, dd0, -0.8f );
                glVertex3f( dd0, dd0, -0.8f );

                glEnd();
            }
        }

        // end of draw PCP axis lines

        // draw PCP labels
        float aspect = (float)size().width()/(float)size().height();
        for(int d = 0; d < (dim - 1); d++)
        {
            glPushMatrix();
                glTranslatef( (dimLoc[d].first+dimLoc[d + 1].first)/2 - 0.01f, -0.95f, 0 );
                glColor3f(0.6f,0.6f,0.6f);
                glScalef(0.02f,0.02f*aspect,0.1f);
                glRotatef(0,0,0,-1);
                int _d1 = dimLoc[d+1].second;
                font->Print( data->GetLabelParsed(_d1).c_str() );
            glPopMatrix();

            glPushMatrix();
                glTranslatef( 0.9f, (dimLoc[d].first+dimLoc[d + 1].first)/2 - 0.01f, 0 );
                glColor3f(0.6f,0.6f,0.6f);
                glScalef(0.02f,0.02f*aspect,0.1f);
                glRotatef(0,0,0,-1);
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
        // hoatam
        for( ; curDraw < data->GetElementCount()/5; curDraw+=5 )
        //for( ; curDraw < data->GetElementCount()/1; curDraw+=1 )
        {
            data->GetElement( curDraw, space );
            DrawElement(space);
        }
        curDraw = 0;
    #else
        //hoatam
        int step = SCI::Max( 1, data->GetElementCount() / 500 );
        //int step = 1;

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
             //hoatam
             int fin = curDraw+500;
             //int fin = curDraw+1;
             for( ; curDraw < fin && curDraw < data->GetElementCount(); curDraw++ )
             {
                 if( (curDraw%step) == 0 ) continue;
                 data->GetElement( curDraw, space );
                 DrawElement(space);
             }
         }
     #endif

    // draw detail view
    if(drawDV == true)
    {
        //std::cout << "Detail View" << std::endl;
        detailView(sDim1, sDim2);
    }

     delete [] space;
     glDisable( GL_BLEND );
     update();
     // end of drawing elements of PCP
 }

// draw elements of dimensions
void Scatter::DrawElement( float * elem )
{
    /*
    glEnable( GL_POINT_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    */
    glPointSize( 3.0 );
    glBegin(GL_POINTS);

    for(int j = 0; j < (dim); j++)
    {
        float distan = fabsf(dimLoc[0].first - dimLoc[1].first);
        for(int m = j+1 ; m < (dim); m++)
        {
            int d0, d1;
            float x0, x1;
            d0 = dimLoc[j].second;
            d1 = dimLoc[m].second;
            x0 = dimLoc[j].first;
            x1 = dimLoc[m].first;

            float y0 = SCI::lerp(x0, x0+distan, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
            float y1 = SCI::lerp(x1-distan, x1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

            float x, y;
            x = y1;
            y = y0;

            glColor3f( 0.0f, 0.8f, 0.0f);
            glVertex3f( x, y, -0.8f );
        }
    }
    glEnd();
}

// compute cubic fitting curve for cubicAxis() function
float Scatter::cubicFitCurve(int step, float * elem, int d0, int d1, float x0, float x0_1, float x1, float x1_1)
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

    float distan = fabsf(dimLoc[0].first - dimLoc[1].first);
    // finding the best fitting curve
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float y0 = SCI::lerp(x0, x0_1, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(x1, x1_1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float x, y;
        x = y1;
        y = y0;

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

        float y0 = SCI::lerp(x0, x0_1, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(x1, x1_1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

        float x, y;
        x = y1;
        y = y0;

        float xt = x;
        float yt = a + b*xt + c*xt*xt + d*xt*xt*xt;

        float err = yt - y;

        errFit += err*err;
    }
    return errFit;
}

// compute quadratic fitting curve for quadAxis() function
float Scatter::quadFitCurve(int step, float * elem, int d0, int d1, float x0, float x1)
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

    float distan = fabsf(dimLoc[0].first - dimLoc[1].first);
    // finding the best fitting curve
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float y0 = SCI::lerp(x0, x0+distan, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(x1-distan, x1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float x, y;
        x = y1;
        y = y0;

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

    // compute fitting error
    float errFit = 0.0f;
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );

        float y0 = SCI::lerp(x0, x0+distan, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(x1-distan, x1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

        float x, y;
        x = y1;
        y = y0;

        float xt = x;
        float yt = e + f*xt + g*xt*xt;

        float err = yt - y;

        errFit += err*err;
    }
    return errFit;
}

// draw cubic curve
void Scatter::cubicAxis()
{    
    cub_a.clear();
    cub_b.clear();
    cub_c.clear();
    cub_d.clear();
    cub_ind.clear();

    fitErr.clear();

    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    float * elem = new float[dim];

    int incub = 0;

    for(int j = 0; j < (dim); j++)
    {
        float distan = fabsf(dimLoc[0].first - dimLoc[1].first);
        for(int m = j+1 ; m < (dim); m++)
        {
            incub += 1;

            int d0, d1;
            float x0, x1;
            d0 = dimLoc[j].second;
            d1 = dimLoc[m].second;
            x0 = dimLoc[j].first;
            x1 = dimLoc[m].first;

            int tx;

            fitErr.push_back(0.0f);
            float erFit = cubicFitCurve(step, elem, d0, d1, x0, x0+distan, x1-distan, x1);
            //std::cout << "errFit [" << j << "]= " << erFit << std::endl;

            float aa1 = a;
            float bb1 = b;
            float cc1 = c;
            float dd1 = d;

            tx = 0;

            float erFit1 = cubicFitCurve(step, elem, d1, d0, x0, x0+distan, x1-distan, x1);
            //std::cout << "errFit1 [" << j << "]= " << erFit1 << std::endl;

            tx = 1;

            fitErr[j] = erFit1;

            if (erFit < erFit1)
            {
                a = aa1;
                b = bb1;
                c = cc1;
                d = dd1;
                fitErr[j] = erFit;
                tx = 0;
            }

            /*
            std::cout << "scatter plots" << std::endl;
            std::cout << "a = " << a << std::endl;
            std::cout << "b = " << b << std::endl;
            std::cout << "c = " << c << std::endl;
            std::cout << "d = " << d << std::endl;
            */

            cub_a.push_back(a);
            cub_b.push_back(b);
            cub_c.push_back(c);
            cub_d.push_back(d);
            cub_ind.push_back(incub-1);


            //std::cout << "at j = " << j << ", m = " << m << " : a = " << a << " b = "  << b << " c = " << c << " d = " << d << std::endl;
            if(fitMeth == 1)
            {
                //tx = 0;
                // Use Levenberg-Marquardt                
                a = am1;
                b = bm1;
                c = cm1;
                d = dm1;
            }
            if(fitMeth == 2)
            {
                //tx = 0;
                // Use Levenberg-Marquardt, approximate derivatives                
                a = am2;
                b = bm2;
                c = cm2;
                d = dm2;
            }
            if(fitMeth == 3)
            {
                //tx = 0;
                //  Use Levenberg-Marquardt/quasi-newton hybrid
                a = am3;
                b = bm3;
                c = cm3;
                d = dm3;
            }

            cout << "a = " << a << endl;
            cout << "b = " << b << endl;
            cout << "c = " << c << endl;
            cout << "d = " << d << endl;

            int thu = incub - 1;

            // draw PCP curve
            glLineWidth(3.0f);
            glBegin(GL_LINE_STRIP);
            //glColor3f(0,0,0);
            float correlation = data->GetCorrelation( d0, d1 );
            SCI::Vex3 cor_color;
            if( correlation > 0 ) cor_color = SCI::Vex3(1,0,0) * ( correlation );
            if( correlation < 0 ) cor_color = SCI::Vex3(0,0,1) * (-correlation );
            glColor3f( cor_color.x, cor_color.y, cor_color.z );

            float tmin = x1-distan;
            float tmax = x1;

            for (float t= tmin; t <= tmax; t += 0.01f)
            {
                float y = cub_a[thu] + cub_b[thu]*t + cub_c[thu]*t*t + cub_d[thu]*t*t*t;
                if (tx == 0)
                    glVertex3f(t,y,-0.5f);
                if (tx == 1)
                    glVertex3f(y + (m-j-1)* distan , t - (m-j-1)* distan ,-0.5f);
            }
            glEnd();


            // find tangent line of the curve.
            // tangent at point
            float xx0, yy0, xx1, yy1, xx2, yy2;
            //float xx_min = x1-distan;
            float xx_min = x1-distan;
            float xx_max = x1;
            float yf0;

            for(int i = 0; i <= nseg; i++)
            {
                // first tangent
                // d17, d21
                /*
                xx0 = xx_min + i*(xx_max - xx_min)/nseg;
                yy0 = a + b*xx0 + c*xx0*xx0 + d*xx0*xx0*xx0;
                yf0 = b + 2*c*xx0 + 3*d*xx0*xx0;
                */

                // d8, d17
                yy0 = xx_min + i*(xx_max - xx_min)/nseg;
                xx0 = a + b*yy0 + c*yy0*yy0 + d*yy0*yy0*yy0;
                yf0 = b + 2*c*xx0 + 3*d*xx0*xx0;

                //d17, d21
                xx1 = xx_max;
                yy1 = yy0 + yf0*(xx1 - xx0);

                xx2 = xx_min;
                yy2 = yy0 + yf0*(xx2 - xx0);

                //d17, d19
                /*
                yy1 = xx_max;
                xx1 = (yy1 - yy0)*yf0 + xx0;

                yy2 = xx_min;
                xx2 = (yy2 - yy0)*yf0 + xx0;
                */

                glPointSize(5.0 );
                glBegin(GL_POINTS);
                glColor3f(1,0,0);
                glVertex3f(xx0, yy0, 0.7f);
                glEnd();

                glLineWidth(3.0f);
                glBegin(GL_LINES);
                glColor3f(0,1,1);
                glVertex3f(xx1, yy1, 0.2f);
                glVertex3f(xx2, yy2, 0.2f);
                glEnd();

                /*
                // d8, d17
                glPointSize(5.0 );
                glBegin(GL_POINTS);
                glColor3f(1,0,0);
                glVertex3f(yy0, xx0, 0.5f);
                glEnd();

                glLineWidth(3.0f);
                glBegin(GL_LINES);
                glColor3f(0,1,1);
                glVertex3f(yy1, xx1, 0.5f);
                glVertex3f(yy2, xx2, 0.5f);
                glEnd();
                */

            }

            /*
            // second tangent
            xx0 = xx_min + (xx_max - xx_min)/2;
            yy0 = a + b*xx0 + c*xx0*xx0 + d*xx0*xx0*xx0;

            yf0 = b + 2*c*xx0 + 3*d*xx0*xx0;

            xx1 = xx_max;
            yy1 = yy0 + yf0*(xx1 - xx0);

            xx2 = xx_min;
            yy2 = yy0 + yf0*(xx2 - xx0);

            glPointSize(5.0 );
            glBegin(GL_POINTS);
            glColor3f(1,0,0);
            glVertex3f(xx0, yy0, 0.5f);
            glEnd();

            glLineWidth(3.0f);
            glBegin(GL_LINES);
            glColor3f(0,1,1);
            glVertex3f(xx1, yy1, 0.5f);
            glVertex3f(xx2, yy2, 0.5f);
            glEnd();

            // third tangent
            xx0 = xx_max - (xx_max - xx_min)/6;
            yy0 = a + b*xx0 + c*xx0*xx0 + d*xx0*xx0*xx0;

            yf0 = b + 2*c*xx0 + 3*d*xx0*xx0;

            xx1 = xx_max;
            yy1 = yy0 + yf0*(xx1 - xx0);

            xx2 = xx_min;
            yy2 = yy0 + yf0*(xx2 - xx0);

            glPointSize(5.0 );
            glBegin(GL_POINTS);
            glColor3f(1,0,0);
            glVertex3f(xx0, yy0, 0.5f);
            glEnd();

            glLineWidth(3.0f);
            glBegin(GL_LINES);
            glColor3f(1,1,0);
            glVertex3f(xx1, yy1, 0.5f);
            glVertex3f(xx2, yy2, 0.5f);
            glEnd();
            */

            // end of tangent lines
        }
    }    
}

// draw quadratic curve axis PCP
void Scatter::quadAxis()
{
    qua_a.clear();
    qua_b.clear();
    qua_c.clear();

    fitErr.clear();

    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    float * elem = new float[dim];

    for(int j = 0; j < (dim); j++)
    {
        float distan = fabsf(dimLoc[0].first - dimLoc[1].first);
        for(int m = j+1 ; m < (dim); m++)
        {
            int d0, d1;
            float x0, x1;
            d0 = dimLoc[j].second;
            d1 = dimLoc[m].second;
            x0 = dimLoc[j].first;
            x1 = dimLoc[m].first;

            int tx;

            fitErr.push_back(0.0f);
            float erFit = quadFitCurve(step, elem, d0, d1, x0, x1);
            //std::cout << "errFit [" << j << "]= " << erFit << std::endl;

            float aa1 = e;
            float bb1 = f;
            float cc1 = g;

            tx = 0;

            float erFit1 = quadFitCurve(step, elem, d1, d0, x0, x1);
            //std::cout << "errFit1 [" << j << "]= " << erFit1 << std::endl;

            tx = 1;

            fitErr[j] = erFit1;

            if (erFit < erFit1)
            {
                e = aa1;
                f = bb1;
                g = cc1;
                fitErr[j] = erFit;
                tx = 0;
            }

            qua_a.push_back(e);
            qua_b.push_back(f);
            qua_c.push_back(g);

            // draw PCP curve
            glLineWidth(3.0f);
            glBegin(GL_LINE_STRIP);
            //glColor3f(0,0,0);

            float correlation = data->GetCorrelation( d0, d1 );
            SCI::Vex3 cor_color;
            if( correlation > 0 ) cor_color = SCI::Vex3(1,0,0) * ( correlation );
            if( correlation < 0 ) cor_color = SCI::Vex3(0,0,1) * (-correlation );
            //glColor4f( cor_color.x, cor_color.y, cor_color.z, 0.025f );
            glColor3f( cor_color.x, cor_color.y, cor_color.z );

            if (tx == 0)
            {
                for (float t= x1 - distan; t <= x1; t += 0.01f)
                {
                    float y = e + f*t + g*t*t;
                    glVertex3f(t,y,-0.5f);
                }
            }

            if (tx == 1)
            {
                for (float t= x1-distan; t <= x1; t += 0.01f)
                {
                    float y = e + f*t + g*t*t;
                    glVertex3f(y + (m-j-1)* distan , t - (m-j-1)* distan ,-0.5f);
                }
            }

            glEnd();
        }
    }
}

// darw detail view
void Scatter::detailView(int sd1, int sd2)
{    
    // draw frame
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(0.6f,0.6f,0.6f);

    float xx0 = 0.9f;
    float xx1 = 0.3f;

    glVertex3f( -xx0, xx0, -0.8f );
    glVertex3f( -xx0, xx1, -0.8f );

    glVertex3f( -xx0, xx1, -0.8f );
    glVertex3f( -xx1, xx1, -0.8f );

    glVertex3f( -xx1, xx1, -0.8f );
    glVertex3f( -xx1, xx0, -0.8f );

    glVertex3f( -xx1, xx0, -0.8f );
    glVertex3f( -xx0, xx0, -0.8f );

    glEnd();

    // draw real points
    glPointSize( 3.0 );
    glBegin(GL_POINTS);

    int d0, d1;
    d0 = dimLoc[sd2].second;
    d1 = dimLoc[sd1].second;

    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    float * elem = new float[dim];
    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float x = SCI::lerp(-xx0, -xx1, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y = SCI::lerp(xx1, xx0, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

        glColor3f( 0.0f, 0.8f, 0.0f);
        glVertex3f( x, y, -0.8f );
    }
    glEnd();

    // draw fitting curve
    if(curveDegree == 3)
    {
        int tx;

        float erFit = cubicFitCurve(step, elem, d0, d1, -xx0, -xx1, xx1, xx0);
        //std::cout << "errFit [" << j << "]= " << erFit << std::endl;

        float aa1 = a;
        float bb1 = b;
        float cc1 = c;
        float dd1 = d;

        tx = 0;

        float erFit1 = cubicFitCurve(step, elem, d1, d0, -xx0, -xx1, xx1, xx0);
        //std::cout << "errFit1 [" << j << "]= " << erFit1 << std::endl;

        tx = 1;

        if (erFit < erFit1)
        {
            a = aa1;
            b = bb1;
            c = cc1;
            d = dd1;
            tx = 0;
        }

        // draw PCP curve
        glLineWidth(3.0f);
        glBegin(GL_LINE_STRIP);

        float correlation = data->GetCorrelation( d0, d1 );
        SCI::Vex3 cor_color;
        if( correlation > 0 ) cor_color = SCI::Vex3(1,0,0) * ( correlation );
        if( correlation < 0 ) cor_color = SCI::Vex3(0,0,1) * (-correlation );
        glColor3f( cor_color.x, cor_color.y, cor_color.z );

        if (tx == 0)
        {
            for (float t= xx1; t <= xx0; t += 0.01f)
            {
                float y = a + b*t + c*t*t + d*t*t*t;
                glVertex3f(y,t,-0.5f);
            }
        }

        if (tx == 1)
        {            
            for (float t= -xx0; t <= -xx1; t += 0.01f)
            {
                float y = a + b*t + c*t*t + d*t*t*t;
                //glVertex3f(y + (m-j-1)* distan , t - (m-j-1)* distan ,-0.5f);
                glVertex3f(-t,-y,-0.5f);
            }
        }

        glEnd();

    }

}

float Scatter::QuadraticComp(float t, float p0, float p1, float p2)
{
    // berizer curve
    float curve = (1-t)*(1-t)*p0 + 2*(1-t)*t*p1 + t*t*p2;
    return curve;
}

float Scatter::CubicComp(float t, float p0, float p1, float p2, float p3)
{    
    // berizer curve
    //float curve = (1-t)*(1-t)*(1-t)*p0 + 3*(1-t)*(1-t)*t*p1 + 3*(1-t)*t*t*p2 + t*t*t*p3;
    // Catmull-Rom spline
    float curve = (-(0.5f)*t*t*t + t*t - (0.5f)*t)*p0 + ((1.5f)*t*t*t - (2.5f)*t*t + 1)*p1 + (-(1.5f)*t*t*t + (2.0f)*t*t + (0.5f)*t)*p2 + ((0.5f)*t*t*t - (0.5f)*t*t)*p3;
    return curve;
}

void Scatter::knn_pca()
{
    //hoatam
    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    //int step = 1;
    int nstep = SCI::Max( 1, data->GetElementCount())/step;
    float * elem = new float[dim];
    float * elem2 = new float[dim];

    //std::cout << "nstep = " << nstep << std::endl;

    //glPointSize( 3.0 );
    //glBegin(GL_POINTS);


    for(int j = 0; j < (dim); j++)
    {
        float distan = fabsf(dimLoc[0].first - dimLoc[1].first);
        for(int m = j+1 ; m < (dim); m++)
        {
            int d0, d1;
            float x0, x1;
            d0 = dimLoc[j].second;
            d1 = dimLoc[m].second;
            x0 = dimLoc[j].first;
            x1 = dimLoc[m].first;
            disKnn.clear();
            indKnn.clear();
            float ind = 0;

            for(int i = 0; i < data->GetElementCount(); i += step )
            {
                data->GetElement( i, elem );
                float y = SCI::lerp(x0, x0+distan, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
                float x = SCI::lerp(x1-distan, x1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

                //for(int h = i + step; h < data->GetElementCount(); h += step )
                for(int h = 0; h < data->GetElementCount(); h += step )
                {
                    data->GetElement( h, elem2 );
                    float yf = SCI::lerp(x0, x0+distan, (elem2[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
                    float xf = SCI::lerp(x1-distan, x1, (elem2[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

                    float di = sqrtf((y-yf)*(y-yf) + (x-xf)*(x-xf));

                    disKnn.push_back( std::make_pair(i, di) );
                    indKnn.push_back( std::make_pair(i, h) );

                    ind += 1;
                }
            }

            // sorting
            float nin = ind;
            //std::cout << "nin = " << ind << std::endl;

            float ns = nstep;

            int st = 0;
            int en = 0;

            //for(int i = 0; i < nstep - 1; i++)
            for(int i = 0; i < nstep; i++)
            {
                //en += ns - 1;
                en += ns;

                for (int m = st;  m < en;  m++)
                {
                    for (int j = m+1;  j < en;  j++)
                    {
                        if (disKnn[m].second > disKnn[j].second)
                        {
                            std::swap(disKnn[m], disKnn[j]);
                            std::swap(indKnn[m], indKnn[j]);
                        }
                    }
                }

                st = en;
                //ns -= 1;
            }

            /*
            for(int i = 0; i < nin; i++)
            {
                std::cout << "ind = " << i << "at i = " << disKnn[i].first << " and h = " << indKnn[i].second << " has di = " << disKnn[i].second << std::endl;
            }
            */


            glPointSize( 3.0 );
            glBegin(GL_POINTS);

            int segNum = 0;
            int group = 0;
            bool rep1, rep2, rep3, rep4 = true;
            //int i = 0;
            for(int i = 0; i < nstep; i++)
            {
                // draw red point at i
                data->GetElement( disKnn[i*nstep].first, elem );


                float y = SCI::lerp(x0, x0+distan, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
                float x = SCI::lerp(x1-distan, x1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

                //glColor3f( 1.0f, 0.0f, 0.0f);
                //glVertex3f( x, y, 0.8f );

                //float input[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
                float input[knum*2];

                input[0] = x;
                input[1] = y;

                // draw red point at knum-1 point at h
                for(int t = 0; t < (knum-1); t++)
                {
                    data->GetElement( indKnn[i*nstep + t].second, elem );

                    float y = SCI::lerp(x0, x0+distan, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
                    float x = SCI::lerp(x1-distan, x1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

                    input[2*(t+1)] = x;
                    input[2*(t+1)+1] = y;

                    //glColor3f( 1.0f, 0.0f, 0.0f);
                    //glVertex3f( x, y, 0.8f );
                }

                glEnd();

                int dimN = 2;
                int elemN = knum;
                int componentN = 2;

                float mean[2];
                float pc0[2];
                float pc1[2];

                drPCA.Calculate( 0, input, dimN, elemN, componentN );
                drPCA.GetMeanPCA( mean );
                drPCA.GetVectorPCA( 0, pc0 );
                drPCA.GetVectorPCA( 1, pc1 );

                /*
                std::cout << "mean0 = " << mean[0] << "mean1 = " << mean[1] << std::endl;
                std::cout << "pc0[0] = " << pc0[0] << "pc0[1] = " << pc0[1] << std::endl;
                std::cout << "pc1[0] = " << pc1[0] << "pc1[1] = " << pc1[1] << std::endl;
                */

                glLineWidth(3.0f);
                glBegin(GL_LINES);

                float tval = 0.03f;
                float b0, xpc0, ypc0;

                if(pc0[0] == 0 && pc0[1] != 0)
                {
                    xpc0 = mean[0];
                    //ypc0 = x0+distan;
                    ypc0 = mean[1] + tval;
                }
                if(pc0[1] == 0 && pc0[0] != 0)
                {
                    //xpc0 = x0+distan;
                    xpc0 = mean[0] + tval;
                    ypc0 = mean[1];
                }
                if (pc0[0] != 0 && pc0[1] != 0)
                {
                    b0 = pc0[0]*mean[1] - pc0[1]*mean[0];
                    //xpc0 = x0+distan;
                    xpc0 = mean[0] + tval;
                    ypc0 = (pc0[1]*xpc0 + b0)/pc0[0];
                }

                /*
                if(xpc0 < x0) xpc0 = x0;
                if(xpc0 > (x0+distan)) xpc0 = x0 + distan;
                if(ypc0 < x0) ypc0 = x0;
                if(ypc0 > (x0+distan)) ypc0 = x0 + distan;
                */

                mean[0] = input[0];
                mean[1] = input[1];

                // hoasua
                /*
                glColor3f( 0.0f, 0.0f, 1.0f);
                glVertex3f( mean[0], mean[1], 0.9f );
                glVertex3f( xpc0, ypc0, 0.9f );
                */

                /*
                float angl = AngleCompute(mean[0] - xpc0, mean[1] - ypc0);

                if (angl <= 90 && angl >= 0)
                {
                    segNum = 1;
                    glColor3f( 0.0f, 0.0f, 1.0f);
                    glVertex3f( mean[0], mean[1], 0.9f );
                    glVertex3f( xpc0, ypc0, 0.9f );
                }
                if (angl <= 180 && angl > 90)
                {
                    segNum = 2;
                    glColor3f( 0.0f, 1.0f, 0.0f);
                    glVertex3f( mean[0], mean[1], 0.9f );
                    glVertex3f( xpc0, ypc0, 0.9f );
                }
                if (angl <= 270 && angl > 180)
                {
                    segNum = 3;
                    glColor3f( 1.0f, 0.0f, 0.0f);
                    glVertex3f( mean[0], mean[1], 0.9f );
                    glVertex3f( xpc0, ypc0, 0.9f );
                }
                if (angl <= 360 && angl > 270)
                {
                    segNum = 4;
                    glColor3f( 1.0f, 0.0f, 1.0f);
                    glVertex3f( mean[0], mean[1], 0.9f );
                    glVertex3f( xpc0, ypc0, 0.9f );
                }
                */

                float angl = AngleCompute(mean[0] - xpc0, mean[1] - ypc0);

                if (angl <= 90 && angl >= 0)
                {
                    segNum = 1;
                    glColor3f( 1.0f, 0.0f, 0.0f);
                    glVertex3f( mean[0], mean[1], 0.9f );
                    glVertex3f( xpc0, ypc0, 0.9f );
                }
                if (angl <= 180 && angl > 90)
                {
                    segNum = 2;
                    glColor3f( 0.0f, 1.0f, 1.0f);
                    glVertex3f( mean[0], mean[1], 0.9f );
                    glVertex3f( xpc0, ypc0, 0.9f );
                }
                if (angl <= 270 && angl > 180)
                {
                    segNum = 3;
                    glColor3f( 0.0f, 0.0f, 1.0f);
                    glVertex3f( mean[0], mean[1], 0.9f );
                    glVertex3f( xpc0, ypc0, 0.9f );
                }
                if (angl <= 360 && angl > 270)
                {
                    segNum = 4;
                    glColor3f( 0.0f, 1.0f, 0.0f);
                    glVertex3f( mean[0], mean[1], 0.9f );
                    glVertex3f( xpc0, ypc0, 0.9f );
                }

                /*
                for(int t = 0; t < nstep; t++)
                {
                    data->GetElement( indKnn[i*nstep + t].second, elem );

                    float y = SCI::lerp(x0, x0+distan, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
                    float x = SCI::lerp(x1-distan, x1, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

                    glColor3f( 1.0f, 0.0f, 0.0f);
                    glVertex3f( x, y, 0.8f );
                }
                */

                float b1, xpc1, ypc1;

                if(pc1[0] == 0 && pc1[1] != 0)
                {
                    xpc1 = mean[0];
                    //ypc1 = x0+distan;
                    ypc1 = mean[1]+tval;
                }
                if(pc1[1] == 0 && pc1[0] != 0)
                {
                    //xpc1 = x0+distan;
                    xpc1 = mean[0]+tval;
                    ypc1 = mean[1];
                }
                if (pc1[0] != 0 && pc1[1] != 0)
                {
                    b1 = pc1[0]*mean[1] - pc1[1]*mean[0];
                    //xpc1 = x0+distan;
                    xpc1 = mean[0]+tval;
                    ypc1 = (pc1[1]*xpc1 + b1)/pc1[0];
                }

                /*
                if(xpc1 < x0) xpc1 = x0;
                if(xpc1 > (x0+distan)) xpc1 = x0 + distan;
                if(ypc1 < x0) ypc1 = x0;
                if(ypc1 > (x0+distan)) ypc1 = x0 + distan;
                */

                glColor3f( 8.0f, 8.0f, 0.0f);
                glVertex3f( mean[0], mean[1], 0.9f );
                glVertex3f( xpc1, ypc1, 0.9f );

                glEnd();
            }
        }
    }

}

// compute angle based on x and y coordinates
float Scatter::AngleCompute(float xs, float ys)
{
    float pi = 3.14159265f;
    float sA = 0.0f;

    if (xs <0 && ys <0)
    {
        sA = atan(fabsf(ys/xs))*180/pi;
    }
    else if (xs > 0 && ys <0)
    {
        sA = 180 - (atan(fabsf(ys/xs))*180/pi);
    }
    else if(xs >0 && ys >0)
    {
        sA = (atan(fabsf(ys/xs))*180/pi) + 180;
    }
    else if (xs <0 && ys >0)
    {
        sA = 360 - (atan(fabsf(ys/xs))*180/pi);
    }
    return sA;
}

