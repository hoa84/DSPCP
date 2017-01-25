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

#include <DarkView/ParallelCoordinates.h>
#include <GL/oglCommon.h>
#include <QMouseEvent>
#include <iostream>
#include <SCI/Vex3.h>
#include <SCI/Vex4.h>
#include <iostream>
#include <stack>
#include <stdlib.h>

using namespace std;

ParallelCoordinates::ParallelCoordinates(MainWidget *_mw, oglWidgets::oglFont & _font, QWidget * parent) : QGLWidget( QGLFormat(QGL::SingleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::AlphaChannel | QGL::DirectRendering | QGL::SampleBuffers), parent )
{
    // number of bin
    numbin = 100;
    binrange = 0.05f;
    hasNeg = 0;
    hasPos = 0;
    // k value in knn algorithm
    knum = 20;
    // kcase: 1 - single K ; 2 - variation K
    kcase = 1;
    frame = 0;
    numtest = 10;
    numClusters = 2;
    numPosClusters = 2;
    nseg = 50;
    verseg = 50;
    rad = 0.03f;
    th1 = 1.0f;
    th2 = 1.0f;
    th3 = 0.9f;
    th4 = 0.9f;

    ng1 = 300.0f;
    ng2 = 400.0f;
    ng3 = 500.0f;
    ng4 = 600.0f;
    ng5 = 700.0f;
    ng6 = 800.0f;
    ng7 = 900.0f;
    ng8 = 1000.0f;
    ng9 = 1100.0f;

    // threshold of triangle mesh
    // This value can be difined again for each group by th1, th2, th3, th4, ... values
    thresholdTri = ng1 ;
    ngVector[0] = ng1;
    ngVector[1] = ng2;
    ngVector[2] = ng3;
    ngVector[3] = ng4;
    ngVector[4] = ng5;
    ngVector[5] = ng6;
    ngVector[6] = ng7;
    ngVector[7] = ng8;
    ngVector[8] = ng9;

    ngVectorH[0] = 400.0f;
    ngVectorH[1] = 450.0f;
    ngVectorH[2] = 500.0f;
    ngVectorH[3] = 550.0f;
    ngVectorH[4] = 600.0f;
    ngVectorH[5] = 650.0f;
    ngVectorH[6] = 750.0f;
    ngVectorH[7] = 850.0f;
    ngVectorH[8] = 950.0f;

    // threshold for color map of positive
    ngpos1 = 1.0f;
    ngpos2 = 2.0f;
    ngpos3 = 3.0f;
    ngpos4 = 4.0f;
    ngpos5 = 5.0f;
    ngpos6 = 6.0f;
    ngpos7 = 7.0f;
    ngpos8 = 8.0f;
    ngpos9 = 9.0f;

    ngposVector[0] = ngpos1;
    ngposVector[1] = ngpos2;
    ngposVector[2] = ngpos3;
    ngposVector[3] = ngpos4;
    ngposVector[4] = ngpos5;
    ngposVector[5] = ngpos6;
    ngposVector[6] = ngpos7;
    ngposVector[7] = ngpos8;
    ngposVector[8] = ngpos9;

    // ntri is number of triangles for a half group (left or right)
    // ntri will be defined by nseg value.
    ntri = 25;

    // to use for images saving
    in = 0;

    // threshold for correlation to define number of cluster
    thresCorr = 0.80f;

    // default of group selected is 0
    selectedGroup = -1;

    colorgr1 = 0.8f;
    colorgr2 = 0.2f;
    colorgr3 = 0.2f;
    colorgrPos1 = 0.2f;
    colorgrPos2 = 0.2f;

    maxPPX = 0;
    maxPPY = 0;
    minPPX = 100000;
    minPPY = 100000;
    hasPos = 0;
    hasNeg = 0;
    stgrx = stgry = engrx = engry = 0.0f;
    stgrx1 = stgry1 = engrx1 = engry1 = 0.0f;
    startSelVal = false;
    maxPPX1 = minPPX1 = maxPPY1 = minPPY1 = 0.0f;
    maxPPX2 = minPPX2 = maxPPY2 = minPPY2 = 0.0f;

    // selection interaction
    // locate 1 item
    locItem = false;
    // locate a range of items
    locItemRange = false;
    xItem = 5.0f;
    yItem = 5.0f;
    selectedDim1 = 0;
    selectedDim2 = 0;
    selectedItem = -1;
    itemStart = 0;
    itemEnd = 0;
    selLine = false;
    selLineRange = false;
    selectedx1 = selectedy1 = selectedx2 = selectedy2 = 0.0f;
    stopRange = false;

    // maximum value of data points
    rangeV = 0.6f;
    data = 0;
    curDraw = 0;
    selected = -1;
    d_scale = 0.1f;
    font = &_font;
    started = false;
    setMouseTracking(true);
    mw = _mw;
}

// This function to setup or update visual data (indr)
void ParallelCoordinates::SetData( DataIndirector * _data )
{
    data = _data;

    // clear dimLoc before setup or update data
    dimLoc.clear();

    if(data != 0)
    {
        Reset();
        dim_min.clear();
        dim_max.clear();
        dim  = data->GetDim();

        for(int d = 0; d < dim; d++)
        {
            float minv = data->GetMinimumValue(d);
            float maxv = data->GetMaximumValue(d);
            float range  = (maxv-minv)/2.0f;
            float center = (minv+maxv)/2.0f;

            if( minv == maxv )
                range = 0.5f;

            minv = center - range*1.01f;
            maxv = center + range*1.01f;

            dim_min.push_back(minv);
            dim_max.push_back(maxv);

            float loc = SCI::lerp( -1.0f, 1.0f, ((float)d+0.5f) / ((float)dim) );
            dimLoc.push_back( std::make_pair(loc,d) );
        }
    }

}

void ParallelCoordinates::Reset()
{
    curDraw = 0;
}

void ParallelCoordinates::initializeGL()
{
    curDraw = 0;
}

void ParallelCoordinates::resizeGL ( int, int )
{
    curDraw = 0;
}

void ParallelCoordinates::mousePressEvent ( QMouseEvent * event )
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
        if( fabsf( dimLoc[i].first - selpx ) < fabsf( dimLoc[closest].first - selpx ) )
        {
            closest = i;
        }
    }

    // if dimension is selected
    if( fabsf( dimLoc[closest].first -selpx ) < 0.01f )
    {
        selected = closest;
        curDraw = 0;
    }

    // when menu is selected
    if(fabsf(selpx - 0.8f) < 0.1f && fabsf(-selpy - 0.8f) < 0.1f)
    {
        menu = 1;
        curDraw = 0;
    }

    if (menu == 1)
    {
        // 1st element in menu
        if(fabsf(selpx - 0.8f) < 0.05f && fabsf(-selpy - 0.9f) < 0.05f)
        {
            // 1K
            kcase = 1;
            curDraw = 0;
        }
        // 2nd element in menu
        if(fabsf(selpx - 0.9f) < 0.05f && fabsf(-selpy - 0.8f) < 0.05f)
        {
            // difK
            // kcase = 2;
            selectedGroup = 0;
            curDraw = 0;
        }
        // 3rd element in menu
        if(fabsf(selpx - 0.8f) < 0.05f && fabsf(-selpy - 0.7f) < 0.05f)
        {
            // locate 1 item
            locItem = true;
            // locate a range of items
            locItemRange = false;
            curDraw = 0;
        }
        // 4th element in menu
        if(fabsf(selpx - 0.7f) < 0.05f && fabsf(-selpy - 0.8f) < 0.05f)
        {
            // selected group
            selectedGroup = 0;
            // locate 1 item
            locItem = false;
            // locate a range of items
            locItemRange = true;
            curDraw = 0;
        }
    }

    if(locItem == true || locItemRange == true)
    {
        for(int i = 1; i < (int)dimLoc.size(); i++)
        {
            if( dimLoc[i].first < selpx && fabsf( dimLoc[i].first - selpx ) < fabsf( dimLoc[closest_left].first - selpx ) )
                closest_left = i;
            if( dimLoc[i].first > selpx && fabsf( dimLoc[i].first - selpx ) < fabsf( dimLoc[closest_right].first - selpx ) )
                closest_right = i;
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
        stopRange = false;
        curDraw = 0;
    }
}

void ParallelCoordinates::mouseReleaseEvent ( QMouseEvent * event )
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
            dimLoc[i].first = SCI::lerp( -1.0f, 1.0f, ((float)i+0.5f) / ((float)dim) );
        selected = -1;
        curDraw = 0;
        d_scale = 0.1f;
    }


    if (selectedGroup >= 0)
    {
        // select trend group
        float x0 = dimLoc[0].first;
        float x1 = dimLoc[1].first;
        selpy = -selpy;
        if (hasNeg > 0)
        {
            bool hasi = intsComp(x0, stgry, x1, stgrx, x0, engry, x1, engrx);
            float xm = xInts;
            float ym = yInts;

            if(point_in_tri(selpx, selpy,x0, stgry, x0, engry, xm, ym) || point_in_tri(selpx, selpy, x1, stgrx, x1, engrx, xm, ym))
                selectedGroup = 1;
            if(hasNeg > 1)
            {
                bool hasi1 = intsComp(x0, stgry1, x1, stgrx1, x0, engry1, x1, engrx1);
                float xm1 = xInts;
                float ym1 = yInts;

                if(point_in_tri(selpx, selpy,x0, stgry1, x0, engry1, xm1, ym1) || point_in_tri(selpx, selpy, x1, stgrx1, x1, engrx1, xm1, ym1))
                    selectedGroup = 2;
            }
        }
        if(hasPos > 0)
        {
            if(point_in_tri(selpx, selpy, x0, minPPY1, x1, minPPX1, 0, 0))
                selectedGroup = 3;
            if(point_in_tri(selpx, selpy, x0, maxPPY2, x1, maxPPX2, x1, minPPX2) || point_in_tri(selpx, selpy, x0, maxPPY2, x0, minPPY2, x1, minPPX2))
                selectedGroup = 4;
        }
         curDraw = 0;
    }

    // if the locate item function is turned on -> lines range
    if(locItemRange == true)
    {
        for(int i = 1; i < (int)dimLoc.size(); i++)
        {
            if( dimLoc[i].first < selpx && fabsf( dimLoc[i].first - selpx ) < fabsf( dimLoc[closest_left].first - selpx ) )
                closest_left = i;
            if( dimLoc[i].first > selpx && fabsf( dimLoc[i].first - selpx ) < fabsf( dimLoc[closest_right].first - selpx ) )
                closest_right = i;
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

void ParallelCoordinates::mouseMoveEvent ( QMouseEvent * event )
{
    float selpx = (float)event->pos().x() / (float)size().width() * 2.0f - 1.0f;
    float selpy = (float)event->pos().y() / (float)size().height() * 2.0f - 1.0f;

    std::pair<int,int> scatter_pair = std::make_pair(-1,-1);

    if(selected != -1)
    {
        dimLoc[ selected ].first = selpx;

        bool repeat;
        do
        {
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
            if( i != selected )
                dimLoc[i].first = SCI::lerp( -1.0f, 1.0f, ((float)i+0.5f) / ((float)dim) );

        curDraw = 0;

        if( selected == ((int)dimLoc.size()-1) )
            scatter_pair = std::make_pair(dimLoc[selected].second, dimLoc[selected-1].second);
        else
            scatter_pair = std::make_pair(dimLoc[selected+1].second, dimLoc[selected].second);
    }
    else
    {
        int closest_left  = 0;
        int closest_right = (int)dimLoc.size()-1;
        for(int i = 1; i < (int)dimLoc.size()-1; i++)
        {
            if( dimLoc[i].first < selpx && fabsf( dimLoc[i].first-selpx ) < fabsf( dimLoc[closest_left].first-selpx ) )
                closest_left = i;
            if( dimLoc[i].first >= selpx && fabsf( dimLoc[i].first-selpx ) < fabsf( dimLoc[closest_right].first-selpx ) )
                closest_right = i;
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
                if( dimLoc[i].first < selpx && fabsf( dimLoc[i].first - selpx ) < fabsf( dimLoc[closest_left].first - selpx ) )
                    closest_left = i;
                if( dimLoc[i].first > selpx && fabsf( dimLoc[i].first - selpx ) < fabsf( dimLoc[closest_right].first - selpx ) )
                    closest_right = i;
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

void ParallelCoordinates::Start()
{
    started = true;
}

void ParallelCoordinates::paintGL()
{
    glViewport( 0, 0, size().width(), size().height() );
    if( curDraw == 0 )
    {
        glClearColor( 1,1,1,1 );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        //CreateTexture();
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
        if (in <= frame)
        {
            // for savesreenshot
            knum += 1;
            // for savescreenshot and for noisevideo
            in += 1;
        }

        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glEnable( GL_BLEND );
        glDepthFunc( GL_LEQUAL );

        // create texture
        //CreateTexture();

        // draw circle menu
        DrawCircle(0.8f, 0.8f, 0.05f, 30, "Menu");

        if(menu == 1)
        {
            DrawCircle(0.8f, 0.9f, 0.05f, 30, "K");
            DrawCircle(0.9f, 0.8f, 0.05f, 30, "trendGr");
            DrawCircle(0.8f, 0.7f, 0.05f, 30, "1item");
            DrawCircle(0.7f, 0.8f, 0.05f, 30, "Items");
        }

        // histogram curve for all dimensions
        histCurve();

        if(selectedGroup == 1)
        {
            colorgr1 = 0.8f;
            colorgr2 = 0.2f;
            colorgrPos1 = 0.2f;
            colorgrPos2 = 0.2f;
        }
        if(selectedGroup == 2)
        {
            colorgr1 = 0.2f;
            colorgr2 = 0.8f;
            colorgrPos1 = 0.2f;
            colorgrPos2 = 0.2f;
        }
        if (selectedGroup == 3)
        {
            colorgr1 = 0.2f;
            colorgr2 = 0.2f;
            colorgrPos1 = 0.8f;
            colorgrPos2 = 0.2f;
        }
        if (selectedGroup == 4)
        {
            colorgr1 = 0.2f;
            colorgr2 = 0.2f;
            colorgrPos1 = 0.2f;
            colorgrPos2 = 0.8f;
        }

        // knnPCA
        if (kcase == 1)
        {
            // start single K
            for(int j = 0; j < (dim-1); j++)
            {
                if(j == 0)
                {
                    numClusters = 1;
                    numPosClusters = 2;
                    colorgr1 = 0.2f;
                    colorgr2 = 0.2f;
                    colorgrPos1 = 0.2f;
                    colorgrPos2 = 0.2f;
                }
                if(j == 1)
                {
                    numClusters = 1;
                    numPosClusters = 1;
                    colorgr1 = 0.2f;
                    colorgr2 = 0.2f;
                    colorgrPos1 = 0.2f;
                    colorgrPos2 = 0.2f;
                }
                if(j == 2)
                {
                    numClusters = 2;
                    numPosClusters = 2;
                    colorgr1 = 0.2f;
                    colorgr2 = 0.2f;
                    colorgrPos1 = 0.2f;
                    colorgrPos2 = 0.2f;
                }

                knnPCA(j, j+1);
                //clusterPos1(j, j+1, 0, 8);
                clusterPos1(j, j+1, 0, 8);
                clusterPos2(j, j+1, 0, 9);
                clusterPos3(j, j+1, 0, 9);

            }
        }
        else if (kcase == 2)
        {
            // variation K
            for (int l = 0; l < 8; l++)
            {
                // for negative
                thresholdTri = ngVector[l];
                knum = 2;
                for(int m = 0; m <=l; m++) 
                {
                    knum = knum*2;
                }
                for(int j = 0; j < (dim-1); j++)
                {
                    knnPCA(j, j+1);
                }

                glBegin(GL_TRIANGLES);

                // red hue for positive
                float maxV = ngVector[l] + 10;
                if(maxV < ng1)
                    glColor4f(1, 1, 0.92f, 0.7f);
                else if(maxV < ng2 && maxV >= ng1)
                    glColor4f(0.996f, 0.90f, 0.807f, 0.7f);
                else if(maxV < ng3 && maxV >= ng2)
                    glColor4f(0.992f, 0.815f, 0.635f, 0.7f);
                else if(maxV < ng4 && maxV >= ng3)
                    glColor4f(0.992f, 0.68f, 0.419f, 0.7f);
                else if(maxV < ng5 && maxV >= ng4)
                    glColor4f(0.992f, 0.55f, 0.235f, 0.7f);
                else if(maxV < ng6 && maxV >= ng5)
                    glColor4f(0.945f, 0.41f, 0.074f, 0.7f);
                else if(maxV < ng7 && maxV >= ng6)
                    glColor4f(0.85f, 0.28f, 0.004f, 0.7f);
                else if(maxV < ng8 && maxV >= ng7)
                    glColor4f(0.65f, 0.2117f, 0.0117f, 0.7f);
                else
                    glColor4f(0.498f, 0.153f, 0.0156f, 0.7f);

                glVertex3f(0.8f, -0.2f + (float)l/10, 0.8f);
                glVertex3f(0.9f, -0.2f + (float)l/10, 0.8f);
                glVertex3f(0.8f, -0.3f + (float)l/10, 0.8f);

                glVertex3f(0.9f, -0.3f + (float)l/10, 0.8f);
                glVertex3f(0.9f, -0.2f + (float)l/10, 0.8f);
                glVertex3f(0.8f, -0.3f + (float)l/10, 0.8f);

                glEnd();
            }
            // end of variations K
        }

        // draw grey PCP axis line from dim1 to dimN
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        for(int i = 0; i < dim; i++)
        {
            glColor3f(0.6f,0.6f,0.6f);
            if(i == selected) glColor3f(1,0,0);
            glVertex3f( dimLoc[i].first, -1, 0.95f );
            glVertex3f( dimLoc[i].first, 1, 0.95f );
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

        // save screen shoot for each value of knum
        if (frame>0)
            saveScreenshot();

        if(selLine == true)
           DrawSelectedLine();
        if(selLineRange == true)
            DrawSelectedHistogram();
    }

    if (in > frame)
        curDraw = 1;


    glDisable( GL_BLEND );
    update();
     // end of drawing elements of PCP
 }

void ParallelCoordinates::CreateTexture()
{
    //size of texture
    xSize = 1.59f*size().width();
    ySize = 1.59f*size().height();
    //our OpenGL texture handle
    //unsigned int texture;
    //new array
    colorBits = new char[ xSize * ySize * 3 ];

    //texture creation..
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE);   //Interpolate ALPHA with ALPHA


    glTexImage2D(GL_TEXTURE_2D, 0, 3, xSize, ySize, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBits);

    //you can set other texture parameters if you want
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // the texture wraps over at the edges (repeat)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    //clean up
    delete[] colorBits;
}

// knn and PCA
void ParallelCoordinates::knnPCA(int jd, int md)
{
    // hoa test data
    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    //int step = 1;
    // computer how many actual elements of each dimension
    int nstep = SCI::Max( 1, data->GetElementCount())/step;
    float * elem = new float[dim];
    float * elem2 = new float[dim];
    int d0, d1;
    float x0, x1;
    d0 = dimLoc[jd].second;
    d1 = dimLoc[md].second;
    x0 = dimLoc[jd].first;
    x1 = dimLoc[md].first;

    // knn
    float ind = 0;
    disKnn.clear();
    indKnn.clear();
    posPoints.clear();
    negPoints.clear();

    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float y = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float x = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        for(int h = 0; h < data->GetElementCount(); h += step )
        {
            data->GetElement( h, elem2 );
            float yf = SCI::lerp(-rangeV, rangeV, (elem2[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
            float xf = SCI::lerp(-rangeV, rangeV, (elem2[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
            float di = sqrtf((y-yf)*(y-yf) + (x-xf)*(x-xf));
            disKnn.push_back( std::make_pair(i, di) );
            indKnn.push_back( std::make_pair(i, h) );
            ind += 1;
        }
    }

    float ns = nstep;
    int st = 0;
    int en = 0;
    for(int i = 0; i < nstep; i++)
    {
        en += ns;
        for (int m = st;  m < en;  m++)
            for (int j = m+1;  j < en;  j++)
                if (disKnn[m].second > disKnn[j].second)
                {
                    std::swap(disKnn[m], disKnn[j]);
                    std::swap(indKnn[m], indKnn[j]);
                }
        st = en;
    }

    // PCA
    bool hasInts;
    hasInts = false;
    posints.clear();
    posints1.clear();
    posints2.clear();
    negints.clear();
    int segNum = 0;
    eigenNeg.clear();
    eigenNeg1.clear();
    eigenNeg2.clear();
    eigenNeg3.clear();
    eigenPos.clear();
    eigenPos1.clear();
    eigenPos2.clear();

    float cor = data->GetCorrelation(jd, md);
    
    // finding positive and negative groups (points and lines)
    for(int i = 0; i < nstep; i++)
    {
        // process each knn group
        data->GetElement( disKnn[i*nstep].first, elem );
        float y = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float x = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float input[knum*2];
        input[0] = x;
        input[1] = y;

        for(int t = 0; t < (knum-1); t++)
        {
            data->GetElement( indKnn[i*nstep + t].second, elem );
            float y = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
            float x = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
            input[2*(t+1)] = x;
            input[2*(t+1)+1] = y;
        }
        int dimN = 2;
        int componentN = 2;
        float mean[2];
        float pc0[2];
        float pc1[2];

        // find PCA of each knn group
        drPCA.Calculate( 0, input, dimN, knum, componentN );
        drPCA.GetMeanPCA( mean );
        drPCA.GetVectorPCA( 0, pc0 );
        drPCA.GetVectorPCA( 1, pc1 );

        // find Primary PCA vectors (xpc1, ypc1) and secondary PCA vector (xpc0, ypc0) and mean PCA vector (mean[0], mean[1])
        float tval = 0.03f;
        float b0, xpc0, ypc0;
        if(pc0[0] == 0 && pc0[1] != 0)
        {
            xpc0 = mean[0];
            ypc0 = mean[1] + tval;
        }
        if(pc0[1] == 0 && pc0[0] != 0)
        {
            xpc0 = mean[0] + tval;
            ypc0 = mean[1];
        }
        if (pc0[0] != 0 && pc0[1] != 0)
        {
            b0 = pc0[0]*mean[1] - pc0[1]*mean[0];
            xpc0 = mean[0] + tval;
            ypc0 = (pc0[1]*xpc0 + b0)/pc0[0];
        }
        float b1, xpc1, ypc1;
        if(pc1[0] == 0 && pc1[1] != 0)
        {
            xpc1 = mean[0];
            ypc1 = mean[1]+tval;
        }
        if(pc1[1] == 0 && pc1[0] != 0)
        {
            xpc1 = mean[0]+tval;
            ypc1 = mean[1];
        }
        if (pc1[0] != 0 && pc1[1] != 0)
        {
            b1 = pc1[0]*mean[1] - pc1[1]*mean[0];
            xpc1 = mean[0]+tval;
            ypc1 = (pc1[1]*xpc1 + b1)/pc1[0];
        }

        mean[0] = input[0];
        mean[1] = input[1];

        float eigenVal1 = dist(mean[0], mean[1], xpc1, ypc1);
        float eigenVal0 = dist(mean[0], mean[1], xpc0, ypc0);
        float eigenVal = 0.03f*eigenVal0/eigenVal1;
        
        // define groups by directions of PCA vectors
        // find angle bewteen mean vector and primary PCA vector
        float angl = AngleCompute(mean[0] - xpc0, mean[1] - ypc0);

        if (angl <= 90 && angl >= 0)
            segNum = 1;
        if (angl <= 180 && angl > 90)
            segNum = 2;
        if (angl <= 270 && angl > 180)
            segNum = 1;
        if (angl <= 360 && angl > 270)
            segNum = 2;

        // find positive intersection points
        if (segNum == 1)
        {
            hasPos = 1;
            posPoints.push_back(std::make_pair(x,y));
            // draw intersection points - primary PCA vectors
            // contain positive intersection points
            hasInts = false;
            // hoa thay doi
            //hasInts = intsComp(x0, mean[1], x1, mean[0], x0, ypc0, x1, xpc0);
            hasInts = intsComp(x0, mean[1], x1, xpc0, x1, mean[0], x0, ypc0);
            if (hasInts == true)
            {
                    posints.push_back( std::make_pair(xInts, yInts) );
                    eigenPos.push_back(eigenVal);
            }
        }
        // find negative intersection points
        else if (segNum == 2)
        {
            hasNeg = 1;
            negPoints.push_back(std::make_pair(x,y));
            // draw intersection points - primary PCA vectors
            // contain negative intersection points
            hasInts = false;
            hasInts = intsComp(x0, mean[1], x1, mean[0], x0, ypc0, x1, xpc0);
            if (hasInts == true)
                if (xInts < x1 && xInts > x0 && yInts < rangeV && yInts > -rangeV )
                {
                    negints.push_back( std::make_pair(xInts, yInts) );
                    eigenNeg.push_back(eigenVal);
                }
        }
    }

    if (cor > 0.94f)
    {
        hasNeg = 0;
        hasPos = 1;
    }
    if (cor < -0.94f)
    {
        hasNeg = 1;
        hasPos = 0;
    }
    // start Negative Process - grouping by location
    if(hasNeg > 0)
    {
        // sorting negative Points
        for (int i=0; i < negPoints.size(); i++ )
            for (int l=i+1; l < negPoints.size(); l++ )
                if (negPoints[l].first < negPoints[i].first)
                {
                    std::swap(negPoints[l], negPoints[i]);
                    std::swap(negints[l], negints[i]);
                }

        float preX = negPoints[0].first;
        float preY = negPoints[0].second;
        int gr = 0;
        stgrx = preX;
        stgry = preY;
        negints1.clear();
        negints2.clear();
        negints3.clear();

        // clustering
        // mininum value to beat (default value is bigger than data range)
        float bnum = 10000000.0f;
        float minum = bnum;
        // the current mininum value
        float distance = 0.0f;
        int cluster = 0;
        bool isStillMoving = true;

        // start to find clustering for dim[j]
        // initilize dataset and centroid
        clusters.clear();
        centroid.clear();

        // find the minimum and maximum element of dimension dim[j]
        // 1 cluster
        if (numClusters == 0 || numClusters == 1)
        {
            centroid.push_back(std::make_pair(x1, (negPoints[(int)negPoints.size()-1].first + negPoints[0].first)/2));
            hasNeg = 1;
        }

        if(numClusters == 2)
        {
            centroid.push_back(std::make_pair(x1, negPoints[(int)negPoints.size()-1].first));
            centroid.push_back(std::make_pair(x1, negPoints[0].first));
            hasNeg = 2;
        }
        if(numClusters == 3)
        {
            centroid.push_back(std::make_pair(x1, negPoints[(int)negPoints.size()-1].first));
            centroid.push_back(std::make_pair(x1, (negPoints[(int)negPoints.size()-1].first + negPoints[0].first)/2));
            centroid.push_back(std::make_pair(x1, negPoints[0].first));
            hasNeg = 3;
        }

        for(int i = 0; i < (int)negPoints.size(); i += 1 )
        {
            xe = x1;
            ye = negPoints[i].first;

            minum = bnum;

            for(int k = 0; k < numClusters; k++)
            {
                distance = dist(xe, ye, centroid[k].first, centroid[k].second);
                //std::cout << " distance at k = " << k << " is " << distance << std::endl;
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
                for(int m = 0; m < (int)negPoints.size(); m += 1 )
                {
                    xe = x1;
                    ye = negPoints[m].first;
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

                for(int i = 0; i < (int)negPoints.size(); i+=1)
                {
                    xe = x1;
                    ye = negPoints[i].first;

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

            for(int i = 0; i < (int)negPoints.size(); i+=1)
            {
                xe = x1;
                ye = negPoints[i].first;
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
        // end clustering

        // init start and end points
        stgrx2 = stgrx1 = stgrx = rangeV;
        engrx2 = engrx1 = engrx = -rangeV;
        stgry2 = stgry1 = stgry = -rangeV;
        engry2 = engry1 = engry = rangeV;

        // find Boundary or Trend of groups
        // find the start and end points of negative trends in PCP
        for (int k = 0; k < numClusters; k++)
        {
            // draw cluster points
            for(int i = 0; i < (int)negPoints.size(); i+=1)
            {
                if (clusters[i] == 0)
                {
                    negints2.push_back(std::make_pair(negints[i].first, negints[i].second));
                    eigenNeg2.push_back(eigenNeg[i]);

                    if(stgrx1 > negPoints[i].first)
                        stgrx1 = negPoints[i].first;
                    if(engrx1 < negPoints[i].first)
                        engrx1 = negPoints[i].first;

                    if(stgry1 < negPoints[i].second)
                        stgry1 = negPoints[i].second;
                    if(engry1 > negPoints[i].second)
                        engry1 = negPoints[i].second;
                }
                if (clusters[i] == 1)
                {
                    negints1.push_back(std::make_pair(negints[i].first, negints[i].second));
                    eigenNeg1.push_back(eigenNeg[i]);

                    if(stgrx > negPoints[i].first)
                        stgrx = negPoints[i].first;
                    if(engrx < negPoints[i].first)
                        engrx = negPoints[i].first;

                    if(stgry < negPoints[i].second)
                        stgry = negPoints[i].second;
                    if(engry > negPoints[i].second)
                        engry = negPoints[i].second;
                }
                if (clusters[i] == 2)
                {
                    negints3.push_back(std::make_pair(negints[i].first, negints[i].second));
                    eigenNeg3.push_back(eigenNeg[i]);

                    if(stgrx2 > negPoints[i].first)
                        stgrx2 = negPoints[i].first;
                    if(engrx2 < negPoints[i].first)
                        engrx2 = negPoints[i].first;

                    if(stgry2 < negPoints[i].second)
                        stgry2 = negPoints[i].second;
                    if(engry2 > negPoints[i].second)
                        engry2 = negPoints[i].second;
                }
            }
        }

        gr = numClusters-1;
        // end of boundary for negative groups
    }
    // End Negative Process

    // Start Positive Process
    if (hasPos == 1)
    {
        // positive boundary and histogram contours
        for (int k = 0; k < (int)posPoints.size(); k++)
        {
            float x = posPoints[k].first;
            float y = posPoints[k].second;
            if(minPPX > x)
            {
                minPPX = x;
                minPPY = y;
            }
            if(maxPPY < y)
            {
                maxPPY = y;
                maxPPX = x;
            }
        }
        // end of process positive boundary

        // draw histogram clustering
        // clustering
        // mininum value to beat (default value is bigger than data range)
        float bnum = 10000000.0f;
        float minum = bnum;
        // the current mininum value
        float distance = 0.0f;
        int cluster = 0;
        bool isStillMoving = true;

        // start to find clustering for dim[j]
        // initilize dataset and centroid
        clusters.clear();
        centroid.clear();

        // find the minimum and maximum element of dimension dim[j]
        // 1 cluster
        if (numPosClusters == 0 || numPosClusters == 1)
            centroid.push_back(std::make_pair(x1, (posPoints[(int)posPoints.size()-1].first + posPoints[0].first)/2));

        if(numPosClusters == 2)
        {
            centroid.push_back(std::make_pair(x1, posPoints[(int)posPoints.size()-1].first));
            centroid.push_back(std::make_pair(x1, posPoints[0].first));
        }
        if(numPosClusters == 3)
        {
            centroid.push_back(std::make_pair(x1, posPoints[(int)posPoints.size()-1].first));
            centroid.push_back(std::make_pair(x1, (posPoints[(int)posPoints.size()-1].first + posPoints[0].first)/2));
            centroid.push_back(std::make_pair(x1, posPoints[0].first));
        }

        for(int i = 0; i < (int)posPoints.size(); i += 1 )
        {
            xe = x1;
            ye = posPoints[i].first;

            minum = bnum;

            for(int k = 0; k < numPosClusters; k++)
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
            for(int k = 0; k < numPosClusters; k++)
            {
                float totalX = 0;
                float totalY = 0;
                int totalInCluster = 0;
                for(int m = 0; m < (int)posPoints.size(); m += 1 )
                {
                    xe = x1;
                    ye = posPoints[m].first;
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
            for (int k = 0; k < numPosClusters; k++)
            {
                float totalX = 0;
                float totalY = 0;
                int totalInCluster = 0;

                for(int i = 0; i < (int)posPoints.size(); i+=1)
                {
                    xe = x1;
                    ye = posPoints[i].first;

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

            for(int i = 0; i < (int)posPoints.size(); i+=1)
            {
                xe = x1;
                ye = posPoints[i].first;
                minum = bnum;
                for(int k = 0; k < numPosClusters; k++)
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
        // end clustering

        // init start and end points
        minPPX2 = minPPX1 = rangeV;
        maxPPX2 = maxPPX1 = -rangeV;
        maxPPY2 = maxPPY1 = -rangeV;
        minPPY2 = minPPY1 = rangeV;

        // find Boundary or Trend of groups
        // find the start and end points of negative trends in PCP
        for (int k = 0; k < numPosClusters; k++)
        {
            // draw cluster points
            for(int i = 0; i < (int)posPoints.size(); i+=1)
            {
                if (clusters[i] == 0)
                {
                    posints1.push_back(std::make_pair(posints[i].first, posints[i].second));
                    eigenPos1.push_back(eigenPos[i]);

                    if(minPPX1 > posPoints[i].first)
                        minPPX1 = posPoints[i].first;
                    if(maxPPX1 < posPoints[i].first)
                        maxPPX1 = posPoints[i].first;

                    if(maxPPY1 < posPoints[i].second)
                        maxPPY1 = posPoints[i].second;
                    if(minPPY1 > posPoints[i].second)
                        minPPY1 = posPoints[i].second;
                }
                if (clusters[i] == 1)
                {
                    posints2.push_back(std::make_pair(posints[i].first, posints[i].second));
                    eigenPos2.push_back(eigenPos[i]);

                    if(minPPX2 > posPoints[i].first)
                        minPPX2 = posPoints[i].first;
                    if(maxPPX2 < posPoints[i].first)
                        maxPPX2 = posPoints[i].first;

                    if(maxPPY2 < posPoints[i].second)
                        maxPPY2 = posPoints[i].second;
                    if(minPPY2 > posPoints[i].second)
                        minPPY2 = posPoints[i].second;

                }
            }
        }
    }
    // End Positive Process
}

//  Triangles algorithm for triangle ABC - alpha edges
void ParallelCoordinates::marchingTriangles(float ax, float ay, float avalue, float bx, float by, float bvalue, float cx, float cy, float cvalue, float colgr, int posneg)
{
    int num = 0;
    float x1, x2, x3, x4;
    float y1, y2, y3, y4;
    float r1, r2;
    float maxV;

    glBegin(GL_TRIANGLES);
    // find which case of this triangle in 8 possible cases
    // triangle
    if(avalue <= thresholdTri && bvalue > thresholdTri && cvalue <= thresholdTri)
    {
        num = 2;
        x1 = bx;
        y1 = by;
        r1 = (thresholdTri - avalue)/(bvalue - avalue);
        r2 = (thresholdTri - cvalue)/(bvalue - cvalue);
        x2 = (1-r1)*ax + r1*bx;
        y2 = (1-r1)*ay + r1*by;
        x3 = (1-r2)*cx + r2*bx;
        y3 = (1-r2)*cy + r2*by;
        maxV = bvalue;
    } 
    else if(avalue <= thresholdTri && bvalue <= thresholdTri && cvalue > thresholdTri)
    {
        num = 2;
        x1 = cx;
        y1 = cy;
        r1 = (thresholdTri - avalue)/(cvalue - avalue);
        r2 = (thresholdTri - bvalue)/(cvalue - bvalue);
        x2 = (1-r1)*ax + r1*cx;
        y2 = (1-r1)*ay + r1*cy;
        x3 = (1-r2)*bx + r2*cx;
        y3 = (1-r2)*by + r2*cy;
        maxV = cvalue;
    }
    else if(avalue > thresholdTri && bvalue <= thresholdTri && cvalue <= thresholdTri)
    {
        num = 2;
        x1 = ax;
        y1 = ay;
        r1 = (thresholdTri - bvalue)/(avalue - bvalue);
        r2 = (thresholdTri - cvalue)/(avalue - cvalue);
        x2 = (1-r1)*bx + r1*ax;
        y2 = (1-r1)*by + r1*ay;
        x3 = (1-r2)*cx + r2*ax;
        y3 = (1-r2)*cy + r2*ay;
        maxV = avalue;
    }
    // polygon
    else if(avalue > thresholdTri && bvalue <= thresholdTri && cvalue > thresholdTri)
    {
        num = 3;
        x1 = ax;
        y1 = ay;
        r1 = (thresholdTri - avalue)/(bvalue - avalue);
        r2 = (thresholdTri - cvalue)/(bvalue - cvalue);
        x2 = (1-r1)*ax + r1*bx;
        y2 = (1-r1)*ay + r1*by;
        x3 = (1-r2)*cx + r2*bx;
        y3 = (1-r2)*cy + r2*by;
        x4 = cx;
        y4 = cy;
        maxV = (avalue+cvalue)/2;
    }
    else if(avalue > thresholdTri && bvalue > thresholdTri && cvalue <= thresholdTri)
    {
        num = 3;
        x1 = ax;
        y1 = ay;
        r1 = (thresholdTri - avalue)/(cvalue - avalue);
        r2 = (thresholdTri - bvalue)/(cvalue - bvalue);
        x2 = (1-r1)*ax + r1*cx;
        y2 = (1-r1)*ay + r1*cy;
        x3 = (1-r2)*bx + r2*cx;
        y3 = (1-r2)*by + r2*cy;
        x4 = bx;
        y4 = by;
        maxV = (avalue+bvalue)/2;
    }
    else if(avalue <= thresholdTri && bvalue > thresholdTri && cvalue > thresholdTri)
    {
        num = 3;
        x1 = bx;
        y1 = by;
        r1 = (thresholdTri - bvalue)/(avalue - bvalue);
        r2 = (thresholdTri - cvalue)/(avalue - cvalue);
        x2 = (1-r1)*bx + r1*ax;
        y2 = (1-r1)*by + r1*ay;
        x3 = (1-r2)*cx + r2*ax;
        y3 = (1-r2)*cy + r2*ay;
        x4 = cx;
        y4 = cy;
        maxV = (bvalue+cvalue)/2;
    }
    else if(avalue > thresholdTri && bvalue > thresholdTri && cvalue > thresholdTri)
    {
        num = 2;
        x1 = ax;
        y1 = ay;
        x2 = bx;
        y2 = by;
        x3 = cx;
        y3 = cy;
        maxV = (avalue+bvalue+cvalue)/3;
    }

    else
        num = 1;

    float colpos;

    if(colgr == 0.9f || colgr == 0.8f)
    {
        colpos = 0.6f;

        if(posneg == 1)
            glColor4f(0.9f, 0.96f, 0.968f, colpos);
        else if(posneg == 2)
            glColor4f(1, 1, 0.92f, colpos);

        glVertex3f(ax, ay, colgr - 0.1f);
        glVertex3f(bx, by, colgr - 0.1f);
        glVertex3f(cx, cy, colgr - 0.1f);
    }
    else
    {
        colpos = 0.1f;

        if(posneg == 1)
            glColor4f(0.95f, 0.96f, 0.968f, colpos);
        else if(posneg == 2)
            glColor4f(1, 1, 0.96f, colpos);

        glVertex3f(ax, ay, colgr);
        glVertex3f(bx, by, colgr);
        glVertex3f(cx, cy, colgr);
    }


    maxV = thresholdTri;

    if(colgr == 0.9f || colgr == 0.8f)
        colpos = 0.8f;
    else
        colpos = 0.1f;

    if(colgr == 0.9f)
    {
        glColor4f(1, 1, 1, 0.7f);
        glVertex3f(ax, ay, 0.85f);
        glVertex3f(bx, by, 0.85f);
        glVertex3f(cx, cy, 0.85f);

        // colormap for brushing - green hue

        if(maxV < ng1)
            glColor4f(0.9686f, 0.988f, 0.96f, colpos);
        else if(maxV < ng2 && maxV >= ng1)
            glColor4f(0.898f, 0.96f, 0.878f, colpos);
        else if(maxV < ng3 && maxV >=ng2)
            glColor4f(0.78f, 0.913f, 0.756f, colpos);
        else if(maxV < ng4 && maxV >=ng3)
            glColor4f(0.63f, 0.85f, 0.607f, colpos);
        else if(maxV < ng5 && maxV >=ng4)
            glColor4f(0.45f, 0.768f, 0.46f, colpos);
        else if(maxV < ng6 && maxV >=ng5)
            glColor4f(0.255f, 0.67f, 0.364f, colpos);
        else if(maxV < ng7 && maxV >=ng6)
            glColor4f(0.137f, 0.545f, 0.27f, colpos);
        else if(maxV < ng8 && maxV >=ng7)
            glColor4f(0, 0.427f, 0.173f, colpos);
        else if(maxV >= ng8)
            glColor4f(0, 0.27f, 0.106f, colpos);
    }
    else
    {
        // negative case
        if(posneg == 1)
        {
            if(maxV < ng1)
                glColor4f(0.9f, 0.96f, 0.968f, colpos);
            else if(maxV < ng2 && maxV >= ng1)
                glColor4f(0.87f, 0.92f, 0.968f, colpos);
            else if(maxV < ng3 && maxV >=ng2)
                glColor4f(0.776f, 0.858f, 0.937f, colpos);
            else if(maxV < ng4 && maxV >=ng3)
                glColor4f(0.619f, 0.792f, 0.882f, colpos);
            else if(maxV < ng5 && maxV >=ng4)
                glColor4f(0.419f, 0.682f, 0.839f, colpos);
            else if(maxV < ng6 && maxV >=ng5)
                glColor4f(0.25f, 0.572f, 0.776f, colpos);
            else if(maxV < ng7 && maxV >=ng6)
                glColor4f(0.129f, 0.443f, 0.71f, colpos);
            else if(maxV < ng8 && maxV >=ng7)
                glColor4f(0.03f, 0.317f, 0.611f, colpos);
            else if(maxV >= ng8)
                glColor4f(0.03f, 0.188f, 0.42f, colpos);
        }
        // positive case
        if(posneg == 2)
        {
            if(maxV < ng1)
                glColor4f(1, 1, 0.92f, colpos);
            else if(maxV < ng2 && maxV >= ng1)
                glColor4f(0.996f, 0.90f, 0.807f, colpos);
            else if(maxV < ng3 && maxV >= ng2)
                glColor4f(0.992f, 0.815f, 0.635f, colpos);
            else if(maxV < ng4 && maxV >= ng3)
                glColor4f(0.992f, 0.68f, 0.419f, colpos);
            else if(maxV < ng5 && maxV >= ng4)
                glColor4f(0.992f, 0.55f, 0.235f, colpos);
            else if(maxV < ng6 && maxV >= ng5)
                glColor4f(0.945f, 0.41f, 0.074f, colpos);
            else if(maxV < ng7 && maxV >= ng6)
                glColor4f(0.85f, 0.28f, 0.004f, colpos);
            else if(maxV < ng8 && maxV >= ng7)
                glColor4f(0.65f, 0.2117f, 0.0117f, colpos);
            else
                glColor4f(0.498f, 0.153f, 0.0156f, colpos);
        }
    }

    // draw triangles
    if(num == 2)
    {
        glVertex3f(x1, y1, colgr);
        glVertex3f(x2, y2, colgr);
        glVertex3f(x3, y3, colgr);
    }
    // draw polygon
    else if(num == 3)
    {
        glVertex3f(x1, y1, colgr);
        glVertex3f(x2, y2, colgr);
        glVertex3f(x3, y3, colgr);

        glVertex3f(x1, y1, colgr);
        glVertex3f(x4, y4, colgr);
        glVertex3f(x3, y3, colgr);
    }

    glEnd();
}
//end of marching triangle - threshold edges

// draw boundary for negative group
void ParallelCoordinates::DrawBoundary(int d0, int d1, float x0, float x1, float startx, float engrx, float starty, float engry, int numgr, float colgr)
{
    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    //int step = 1;
    float * elem3 = new float[dim];
    float px1, py1, px2, py2, ey1, ey2;
    float pj1_value, pj2_value, pre_pj1_value, pre_pj2_value, ey1_value, ey2_value;
    float xmid = 0.0f;
    float ymid = 0.0f;
    ey1 = 0.0f;
    ey2 = 0.0f;

    // max point
    px1 = x0;
    py1 = starty;
    // min point
    ey1 = engry;

    bool hasint = false;
    hasint = intsComp(x0, starty, x1, startx, x0, engry, x1, engrx);
    if(hasint == true)
    {
        xmid = xInts;
        ymid = yInts;
    }

    for(int i = 1; i <= nseg; i++)
    {
        px2 = x0 + i*(x1 - x0)/nseg;

        py2 = -1.0f;
        ey2 = 1.0f;

        // start finding upper and lower intersection points for boundary
        for(int k = 0; k < data->GetElementCount(); k += step )
        {
            data->GetElement( k, elem3 );
            float y = SCI::lerp(-rangeV, rangeV, (elem3[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
            float x = SCI::lerp(-rangeV, rangeV, (elem3[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

            // find max point (py2) and min point (ey2)
            if ((x >= startx) && (x <= engrx) && (y <= starty) && (y >= engry))
            {
                hasint = false;
                // finding intersection points between two lines: ((x0,y)&(x1,x)) and ((px2,rangeV)&(px2,-rangeV))
                hasint = intsComp(x0, y, x1, x, px2, 1.0f, px2, -1.0f);

                if (hasint == true)
                {
                    if(py2 < yInts)
                        py2 = yInts;
                    if(ey2 > yInts)
                        ey2 = yInts;
                }
            }
        }

        if(i == nseg)
        {
            py2 = engrx;
            ey2 = startx;
        }
        // end of finding boundary polygon (px1, py1) & (px2, py2) & (px2, ey2) & (px1, ey1)
        // draw boundary lines: ((px1, py1) & (px2, py2)) and ((px2, ey2) & (px1, ey1))

        if(colgr == 0.9f || colgr == 0.8f)
            glLineWidth( 3.0f );
        else
            glLineWidth( 1.0f );

        glBegin(GL_LINES);

        if(colgr == 0.9f || colgr == 0.8f)
        {
            glColor4f(0.58f,0,0.85f, 0.7f);
            if(colgr == 0.9f)
                glColor4f(0,1,0, 0.7f);
        }
        else if(colgr == 0.2f)
        {
            glColor4f(0,0.265f,0.11f, 0.7f);
        }
        else if(colgr == 0.5f)
        {
            glColor4f(0.77f, 0.11f, 0.49f, 0.7f);
        }

        glVertex3f(px1, py1, colgr+0.05f);
        glVertex3f(px2, py2, colgr+0.05f);

        glVertex3f(px1, ey1, colgr+0.05f);
        glVertex3f(px2, ey2, colgr+0.05f);
        glEnd();
        glLineWidth( 1.0f );
        // end drawing boundary lines

        // marching triangles for polygon (px1, py1) & (px2, py2) & (px2, ey2) & (px1, ey1)
        float pyj1 = py1;
        float pyj2 = py2;
        int si;
        float x, y;

        if(px1 >= xmid)
            verseg += 1;
        else
            verseg -= 1;

        for (int j = 1; j <= verseg; j++)
        {
            float pre_pyj1 = pyj1;
            float pre_pyj2 = pyj2;
            pj1_value = 0.0f;
            pj2_value = 0.0f;
            pre_pj1_value = 0.0f;
            pre_pj2_value = 0.0f;
            ey1_value = 0.0f;
            ey2_value = 0.0f;

            pyj1 = py1 - j*(py1 - ey1)/verseg;
            if(px1 < xmid)
                pyj2 = py2 - j*(py2 - ey2)/(verseg-1);
            else
                pyj2 = py2 - j*(py2 - ey2)/(verseg+1);

            if(numgr == 0)
                si = (int)negints1.size();
            else if (numgr == 1)
                si = (int)negints2.size();
            else if (numgr == 2)
                si = (int)negints3.size();

            for (int k = 0; k < si; k++)
            {
                if(numgr == 0)
                {
                    x = negints1[k].first;
                    y = negints1[k].second;
                    rad = eigenNeg1[k];
                }
                else if (numgr == 1)
                {
                    x = negints2[k].first;
                    y = negints2[k].second;
                    rad = eigenNeg2[k];
                }
                else if (numgr == 2)
                {
                    x = negints3[k].first;
                    y = negints3[k].second;
                    rad = eigenNeg3[k];
                }
                // compute value of vertices
                float dis = (float)sqrt((px1 - x)*(px1 - x) + (pyj1 - y)*(pyj1 - y));
                if (dis < rad)
                    pj1_value += 1/dis;
                dis = (float)sqrt((px2 - x)*(px2 - x) + (pyj2 - y)*(pyj2 - y));
                if (dis < rad)
                    pj2_value += 1/dis;

                dis = (float)sqrt((px1 - x)*(px1 - x) + (pre_pyj1 - y)*(pre_pyj1 - y));
                if (dis < rad)
                    pre_pj1_value += 1/dis;
                dis = (float)sqrt((px2 - x)*(px2 - x) + (pre_pyj2 - y)*(pre_pyj2 - y));
                if (dis < rad)
                    pre_pj2_value += 1/dis;

                dis = (float)sqrt((px1 - x)*(px1 - x) + (ey1 - y)*(ey1 - y));
                if (dis < rad)
                    ey1_value += 1/dis;
                dis = (float)sqrt((px2 - x)*(px2 - x) + (ey2 - y)*(ey2 - y));
                if (dis < rad)
                    ey2_value += 1/dis;
                // end computing value of vertices
            }

            // start marching triangles
            if(px1 < xmid)
            {
                marchingTriangles(px1, pre_pyj1, pre_pj1_value, px2, pre_pyj2, pre_pj2_value, px2, pyj2, pj2_value, colgr, 1);
                marchingTriangles(px2, pyj2, pj2_value, px1, pre_pyj1, pre_pj1_value, px1, pyj1, pj1_value, colgr, 1);
            }
            else
            {
                marchingTriangles(px1, pre_pyj1, pre_pj1_value, px2, pre_pyj2, pre_pj2_value, px1, pyj1, pj1_value, colgr, 1);
                marchingTriangles(px2, pyj2, pj2_value,  px2, pre_pyj2, pre_pj2_value, px1, pyj1, pj1_value, colgr, 1);
            }
            // end marching triangles
            if (px1 < xmid && j == verseg - 1)
                j += 2;
        }

        if (px1 < xmid)
            marchingTriangles(px1, ey1, ey1_value, px1, pyj1, pj1_value, px2, pyj2, pj2_value, colgr, 1);
        else
            marchingTriangles(px2, ey2, ey2_value, px1, pyj1, pj1_value, px2, pyj2, pj2_value, colgr, 1);

        // save values of boundary points for next process
        px1 = px2;
        py1 = py2;
        ey1 = ey2;
    }
}
//end drawBoundary

// draw boundary for positive group
void ParallelCoordinates::DrawPosBoundary(int d0, int d1, float x0, float x1, float maxX, float minX, float maxY, float minY, float colgr, int grnum)
{
    float px1, py1, px2, py2, ey1, ey2;
    float pj1_value, pj2_value, pre_pj1_value, pre_pj2_value, ey1_value, ey2_value;
    ey1 = 0.0f;
    ey2 = 0.0f;

    // max point
    px1 = x0;
    py1 = maxY;
    // min point
    ey1 = minY;

    for(int i = 1; i <= nseg; i++)
    {
        px2 = x0 + i*(x1 - x0)/nseg;

        // finding intersection points between two lines
        bool hasint = intsComp(x0, maxY, x1, maxX, px2, 1.0f, px2, -1.0f);
        py2 = yInts;

        bool hasint1 = intsComp(x0, minY, x1, minX, px2, 1.0f, px2, -1.0f);
        ey2 = yInts;

        if(i == nseg)
        {
            py2 = maxX;
            ey2 = minX;
        }
        // end of finding boundary polygon (px1, py1) & (px2, py2) & (px2, ey2) & (px1, ey1)

        // marching triangles for polygon (px1, py1) & (px2, py2) & (px2, ey2) & (px1, ey1)
        float pyj1 = py1;
        float pyj2 = py2;
        int si;
        float x, y;

        for (int j = 1; j <= verseg; j++)
        {
            float pre_pyj1 = pyj1;
            float pre_pyj2 = pyj2;
            pj1_value = 0.0f;
            pj2_value = 0.0f;
            pre_pj1_value = 0.0f;
            pre_pj2_value = 0.0f;
            ey1_value = 0.0f;
            ey2_value = 0.0f;

            pyj1 = py1 - j*(py1 - ey1)/verseg;
            pyj2 = py2 - j*(py2 - ey2)/verseg;

            if (grnum == 0)
                si = (int)posints.size();
            if (grnum == 1)
                si = (int)posints1.size();
            if (grnum == 2)
                si = (int)posints2.size();

            for (int k = 0; k < si; k++)
            {
                if(grnum == 0)
                {
                    x = posints[k].first;
                    y = posints[k].second;
                    rad = eigenPos[k];
                }
                if(grnum == 1)
                {
                    x = posints1[k].first;
                    y = posints1[k].second;
                    rad = eigenPos1[k];
                }
                if(grnum == 2)
                {
                    x = posints2[k].first;
                    y = posints2[k].second;
                    rad = eigenPos2[k];
                }
                // compute value of vertices
                float dis = (float)sqrt((px1 - x)*(px1 - x) + (pyj1 - y)*(pyj1 - y));
                if (dis < rad)
                    pj1_value += 1/dis;
                dis = (float)sqrt((px2 - x)*(px2 - x) + (pyj2 - y)*(pyj2 - y));
                if (dis < rad)
                    pj2_value += 1/dis;

                dis = (float)sqrt((px1 - x)*(px1 - x) + (pre_pyj1 - y)*(pre_pyj1 - y));
                if (dis < rad)
                    pre_pj1_value += 1/dis;
                dis = (float)sqrt((px2 - x)*(px2 - x) + (pre_pyj2 - y)*(pre_pyj2 - y));
                if (dis < rad)
                    pre_pj2_value += 1/dis;

                dis = (float)sqrt((px1 - x)*(px1 - x) + (ey1 - y)*(ey1 - y));
                if (dis < rad)
                    ey1_value += 1/dis;
                dis = (float)sqrt((px2 - x)*(px2 - x) + (ey2 - y)*(ey2 - y));
                if (dis < rad)
                    ey2_value += 1/dis;
            }
            marchingTriangles(px1, pre_pyj1, pre_pj1_value, px2, pre_pyj2, pre_pj2_value, px2, pyj2, pj2_value, colgr, 2);
            marchingTriangles(px2, pyj2, pj2_value, px1, pre_pyj1, pre_pj1_value, px1, pyj1, pj1_value, colgr, 2);
        }
        
        // save values of boundary points for next process
        px1 = px2;
        py1 = py2;
        ey1 = ey2;
    }
}
//end draw pos Boundary

// compute intersection point between line (x1,y1),(x2,y2) and line (x3,y3), (x4,y4)
bool ParallelCoordinates::intsComp(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    float d = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3-x4);

    // if d = 0 then there is no intersection point
    if (d == 0) return false;
    else
    {
        float p = x1*y2 - y1*x2;
        float q = x3*y4 - y3*x4;

        // compute intersection point
        float x = (p*(x3 - x4) - (x1 - x2)*q)/d;
        float y = (p*(y3 - y4) - (y1 - y2)*q)/d;

        // check if x and y coordinates are in both lines
        if (x < std::min(x1, x2) || x > std::max(x1, x2) || x < std::min(x3, x4) || x > std::max(x3, x4))
        {
            // there is no intersection point
            return false;
        }
        if (y < std::min(y1, y2) || y > std::max(y1, y2) || y < std::min(y3, y4) || y > std::max(y3, y4))
        {
            // there is no intersection point
            return false;
        }

        // else then it has intersection point between two lines
        xInts = x;
        yInts = y;

        return true;
    }
}

// compute angle based on x and y coordinates
float ParallelCoordinates::AngleCompute(float xs, float ys)
{
    float sA = 0.0f;
    float pi = 3.14159265f;

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
// end of angleCompute

// determine point inside triangle or not
bool ParallelCoordinates::point_in_tri(float sx, float sy, float ax, float ay, float bx, float by, float cx, float cy)
{
    float as_x = sx-ax;
    float as_y = sy-ay;

    bool s_ab = (bx-ax)*as_y-(by-ay)*as_x > 0;

    if((cx-ax)*as_y-(cy-ay)*as_x > 0 == s_ab) return false;

    if((cx-bx)*(sy-by)-(cy-by)*(sx-bx) > 0 != s_ab) return false;

    return true;
}

// save screenshot
void ParallelCoordinates::saveScreenshot()
{
    // save screen
    QPixmap originalPixmap;
    QString format = "png";
    fileName = "/Users/hoa/Documents/DVproject/TPCP/Results/"+QString::number(knum)+".png";

    originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
    originalPixmap.save(fileName, format.toAscii());
}

// find the line that is closest with current point between selectedDim1 and selectedDim2
// select line
void ParallelCoordinates::selectedLine()
{
    float * elem = new float[dim];
    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    int j = selectedDim1;
    int d0, d1;
    float x0, x1;
    // distance between curve axis and grey line axis is maxPoint of each curve
    d0 = dimLoc[j].second;
    d1 = dimLoc[j+1].second;
    x0 = dimLoc[j].first;
    x1 = dimLoc[j+1].first;

    float minDisPLine = 100.0f;
    float minDis1x, minDis1y;
    float minDis2x, minDis2y;

    for(int i = 0; i < data->GetElementCount(); i += step )
    {
        data->GetElement( i, elem );
        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
        float xp = xItem;
        float yp = -yItem;
        float m = (y0 - y1)/(x0 - x1);
        float n = y0 - m*x0;

        // compute the distance between chosen point and lines
        float dist = fabsf(yp - m*xp - n)/(sqrtf(m*m+1));

        // compute the shortest distance between point and the line
        if (dist < minDisPLine)
        {
            minDisPLine = dist;
            minDis1x = x0;
            minDis1y = y0;
            minDis2x = x1;
            minDis2y = y1;
            selectedItem = i;
        }
    }

    selectedy2 = minDis2y;
    selectedx1 = minDis1x;
    selectedy1 = minDis1y;
    selectedx2 = minDis2x;

    if (startSelVal == false)
    {
        startSelx1 = selectedx1;
        startSely1 = selectedy1;
        startSelx1 = selectedx1;
        startSely1 = selectedy1;
        startSelVal = true;
    }
}

// draw the selected lines
void ParallelCoordinates::DrawSelectedLine()
{
    glBegin(GL_LINES);
    glLineWidth(3.0f);
    glColor4f(0.0f,1.0f,0.0f, 0.7f);
    float * elem = new float[dim];

    for(int j = 0; j < (dim-1); j++)
    {
        int d0, d1;
        float x0, x1;
        d0 = dimLoc[j].second;
        d1 = dimLoc[j+1].second;
        x0 = dimLoc[j].first;
        x1 = dimLoc[j+1].first;

        data->GetElement( selectedItem, elem );
        float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
        float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

        glVertex3f( x0, y0, 0.8f );
        glVertex3f( x1, y1, 0.8f );
    }
    glEnd();
}

// draw the selected lines
void ParallelCoordinates::DrawSelectedHistogram(int jd0, int jd1)
{
    float * elem = new float[dim];

    int d0, d1;
    float x0, x1;
    d0 = dimLoc[jd0].second;
    d1 = dimLoc[jd1].second;
    x0 = dimLoc[jd0].first;
    x1 = dimLoc[jd1].first;

    float minx, maxx, miny, maxy;
    data->GetElement( itemStart, elem );
    float sty = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float stx= SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

    data->GetElement( itemEnd, elem );
    float eny = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float enx = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

    if(sty < eny)
    {
        miny = sty;
        maxy = eny;
    }
    else
    {
        miny = eny;
        maxy = sty;
    }

    if(stx < enx)
    {
        minx = stx;
        maxx = enx;
    }
    else
    {
        minx = enx;
        maxx = stx;
    }

    for (int l = 0; l < 9; l++)
    {
        thresholdTri = ngVectorH[l];
        if(jd0 == 0)
            DrawBoundary(d0, d1, x0, x1, minx, maxx, maxy, miny, 1, 0.9f);
        else if(jd0 == 1)
        {
            glBegin(GL_LINES);
            // hoa tam
            // purple glColor4f(0.58f,0,0.85f, 0.7f);
            glColor4f(0,1,0, 0.7f);
            //glColor4f(0.58f,0,0.85f, 0.7f);
            glVertex3f(x0, maxy, 0.95f);
            glVertex3f(x1, maxx, 0.95f);

            glVertex3f(x0, miny, 0.95f);
            glVertex3f(x1, minx, 0.95f);
            glEnd();
            glLineWidth( 3.0 );

            DrawPosBoundary(d0, d1, x0, x1, maxx, minx, maxy, miny , 0.9f, 1);
        }
        else if(jd0 == 2)
            DrawBoundary(d0, d1, x0, x1, minx, maxx, maxy, miny, 1, 0.9f);
    }

    std::cout << "Draw Histogram of selectedGroup = " << selectedGroup << std::endl;

}

void ParallelCoordinates::clusterPos1(int jd0, int jd1, int itStart, int itEnd)
{
    float * elem = new float[dim];

    int d0, d1;
    float x0, x1;
    d0 = dimLoc[jd0].second;
    d1 = dimLoc[jd1].second;
    x0 = dimLoc[jd0].first;
    x1 = dimLoc[jd1].first;

    float minx, maxx, miny, maxy;
    data->GetElement( itStart, elem );
    float sty = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float stx= SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

    data->GetElement( itEnd, elem );
    float eny = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float enx = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

    if(sty < eny)
    {
        miny = sty;
        maxy = eny;
    }
    else
    {
        miny = eny;
        maxy = sty;
    }

    if(stx < enx)
    {
        minx = stx;
        maxx = enx;
    }
    else
    {
        minx = enx;
        maxx = stx;
    }

    for (int l = 0; l < 9; l++)
    {
        thresholdTri = ngVector[l];
        if(jd0 == 0)
        {
            DrawBoundary(d0, d1, x0, x1, stgrx1, engrx1, stgry1, engry1, 1, 0.8f);
            a0 = stgry1;
            a1 = engrx1;
            b0 = engry1;
            b1 = stgrx1;
        }
        else if(jd0 == 1)
        {
            maxx = 0.05f;
            glBegin(GL_LINES);
            // hoa tam
            // purple glColor4f(0.58f,0,0.85f, 0.7f);
            //glColor4f(0,1,0, 0.7f);
            glColor4f(0.58f,0,0.85f, 0.7f);
            glVertex3f(x0, a1, 0.95f);
            glVertex3f(x1, maxx, 0.95f);

            glVertex3f(x0, b1, 0.95f);
            glVertex3f(x1, minx, 0.95f);
            glEnd();
            glLineWidth( 3.0 );

            DrawPosBoundary(d0, d1, x0, x1, maxx, minx, a1, b1 , 0.8f, 1);
            a2 = maxx;
            b2 = minx;
        }
        else if(jd0 == 2)
        {
            maxx = rangeV;
            maxy = 0.05f;
            //DrawBoundary(d0, d1, x0, x1, minx, maxx, maxy, miny, 1, 0.8f);
            DrawBoundary(d0, d1, x0, x1, minx, maxx, maxy, miny, 1, 0.8f);

            b3 = minx;

            //std::cout << "diem1 = " << maxx << std::endl;
            //std::cout << "diem2 = " << minx << std::endl;
        }
    }
}

void ParallelCoordinates::clusterPos2(int jd0, int jd1, int itStart, int itEnd)
{
    float * elem = new float[dim];

    int d0, d1;
    float x0, x1;
    d0 = dimLoc[jd0].second;
    d1 = dimLoc[jd1].second;
    x0 = dimLoc[jd0].first;
    x1 = dimLoc[jd1].first;

    float minx, maxx, miny, maxy;
    data->GetElement( itStart, elem );
    float sty = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float stx= SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

    data->GetElement( itEnd, elem );
    float eny = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float enx = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

    if(sty < eny)
    {
        miny = sty;
        maxy = eny;
    }
    else
    {
        miny = eny;
        maxy = sty;
    }

    if(stx < enx)
    {
        minx = stx;
        maxx = enx;
    }
    else
    {
        minx = enx;
        maxx = stx;
    }

    for (int l = 0; l < 9; l++)
    {
        thresholdTri = ngVector[l];

        if(jd0 == 0)
        {
            minPPX1 = -rangeV;
            glBegin(GL_LINES);
            glColor4f(0,0.265f,0.11f, 0.1f);
            glVertex3f(x0, maxPPY1, 0.1f);
            glVertex3f(x1, b1, 0.2f);

            glVertex3f(x0, minPPY1, 0.1f);
            glVertex3f(x1, minPPX1, 0.1f);
            glEnd();
            glLineWidth( 1.0 );

            DrawPosBoundary(d0, d1, x0, x1, b1, minPPX1, maxPPY1, minPPY1, -0.7f, 1);

        }
        else if(jd0 == 1)
        {
            minPPY1 = -rangeV;
            glBegin(GL_LINES);
            glColor4f(0,0.265f,0.11f, 0.1f);
            glVertex3f(x0, b1, 0.1f);
            glVertex3f(x1, b2, 0.1f);

            glVertex3f(x0, minPPY1, 0.1f);
            glVertex3f(x1, minPPX1, 0.1f);
            glEnd();
            glLineWidth( 1.0 );

            DrawPosBoundary(d0, d1, x0, x1, b2, minPPX1, b1, minPPY1, -0.7f, 1);
        }
        else if(jd0 == 2)
        {
            a0 = minPPX1;
            glBegin(GL_LINES);
            glColor4f(0,0.265f,0.11f, 0.1f);
            glVertex3f(x0, b2, 0.1f);
            glVertex3f(x1, b3, 0.1f);

            glVertex3f(x0, -rangeV, 0.1f);
            glVertex3f(x1, minPPX1, 0.1f);
            glEnd();
            glLineWidth( 1.0 );

            DrawPosBoundary(d0, d1, x0, x1, b3, minPPX1, b2, -rangeV, -0.7f, 1);
        }
    }
}

void ParallelCoordinates::clusterPos3(int jd0, int jd1, int itStart, int itEnd)
{
    float * elem = new float[dim];

    int d0, d1;
    float x0, x1;
    d0 = dimLoc[jd0].second;
    d1 = dimLoc[jd1].second;
    x0 = dimLoc[jd0].first;
    x1 = dimLoc[jd1].first;

    float minx, maxx, miny, maxy;
    data->GetElement( itStart, elem );
    float sty = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float stx= SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

    data->GetElement( itEnd, elem );
    float eny = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float enx = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

    if(sty < eny)
    {
        miny = sty;
        maxy = eny;
    }
    else
    {
        miny = eny;
        maxy = sty;
    }

    if(stx < enx)
    {
        minx = stx;
        maxx = enx;
    }
    else
    {
        minx = enx;
        maxx = stx;
    }

    float ent1;

    for (int l = 0; l < 9; l++)
    {
        thresholdTri = ngVector[l];
        if(jd0 == 0)
        {
            minPPY2 = -rangeV;
            glBegin(GL_LINES);
            glColor4f(0.77f, 0.11f, 0.49f, 0.7f);
            glVertex3f(x0, maxPPY2, 0.2f);
            glVertex3f(x1, maxPPX2, 0.2f);

            glVertex3f(x0, minPPY2, 0.2f);
            glVertex3f(x1, minPPX2, 0.2f);
            glEnd();
            glLineWidth( 1.0 );

            DrawPosBoundary(d0, d1, x0, x1, maxPPX2, minPPX2, maxPPY2, minPPY2, 0.5f, 2);
            ent1 = minPPY2;

        }
        else if(jd0 == 1)
        {
            DrawBoundary(d0, d1, x0, x1, stgrx1, engrx1, stgry1, engry1, 1, 0.5f);
        }
        else if(jd0 == 2)
        {
            engrx = a0;
            DrawBoundary(d0, d1, x0, x1, minPPX2, engrx, maxPPY2, engry, 0, 0.5f);
        }
    }
}

// compute the distances between the point P(xp, yp)and a line y = m*x + n
float ParallelCoordinates::disPLine(float xp, float yp, float m, float n)
{
    float dist = fabsf(yp - m*xp - n)/sqrtf(m*m+1);
    return dist;
}

// draw circle for menu
void ParallelCoordinates::DrawCircle(float cx, float cy, float r, int num_segments, std::string t)
{
    glColor3f(0.6f,0.6f,0.6f);
    glBegin(GL_TRIANGLE_FAN);

    for(int ii = 0; ii < num_segments; ii++)
    {
        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component

        glVertex3f(x + cx, y + cy, -0.5f);//output vertex

    }
    glEnd();

    float aspect = (float)size().width()/(float)size().height();
    glPushMatrix();
        glTranslatef( cx-0.01f, cy, 0 );
        glColor3f(1,0,0);
        glScalef(0.02f,0.02f*aspect,0.1f);
        glRotatef(0,0,0,-1);
        std::string s = t;
        font->Print(s.c_str());
    glPopMatrix();

}

// clustering
void ParallelCoordinates::minmaxItem(int j)
{
    int   d0 = dimLoc[j].second;
    int   d1 = dimLoc[j+1].second;
    float x0 = dimLoc[j].first;
    float x1 = dimLoc[j+1].first;

    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    float * elem = new float[dim];

    xyElement(elem, 0, d0, d1, x1);
    float minx = xe;
    float miny = ye;
    float maxx = xe;
    float maxy = ye;
    int indMin = 0;
    int indMax = 0;

    for(int i = step; i < data->GetElementCount(); i += step )
    {
        xyElement(elem, i, d0, d1, x1);

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
// for kmean
float ParallelCoordinates::dist(float x1, float y1, float x2, float y2)
{
    float di;
    di = sqrtf((y1 - y2)*(y1 - y2) + (x1 - x2)*(x1 - x2));
    return di;
}

// compute x, y based on element
// for kmean
void ParallelCoordinates::xyElement(float * elem, int i, int d0, int d1, float x0)
{
    data->GetElement( i, elem );
    float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
    float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));
    float y, x;
    xe = y0;
    ye = x0;
}


// find clustering
void ParallelCoordinates::kMeanCluster(int j)
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

    int   d0 = dimLoc[j].second;
    int   d1 = dimLoc[j+1].second;
    float x0 = dimLoc[j].first;
    float x1 = dimLoc[j+1].first;

    // start to find clustering for dim[j]
    // initilize dataset and centroid
    clusters.clear();
    centroid.clear();

    // find the minimum and maximum element of dimension dim[j]
    minmaxItem(j);

    // 1 cluster
    if (numClusters == 0 || numClusters == 1)
        centroid.push_back(std::make_pair((maxElemX+minElemX)/2, (maxElemY+minElemY)/2));
    // 2 cluster
    if(numClusters == 2)
    {
        centroid.push_back(std::make_pair(minElemX, minElemY));
        centroid.push_back(std::make_pair(maxElemX, maxElemY));
    }
    //  > 2 clusters
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
        // find (xe, ye) point of data item i
        xyElement(elem, i, d0, d1, x1);

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
                xyElement(elem, m, d0, d1, x1);
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
                xyElement(elem, i, d0, d1, x1);
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
            xyElement(elem, i, d0, d1, x1);
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
    // end finding clustering

    // draw the clustering result
    for (int k = 0; k < numClusters; k++)
    {
        // draw cluster points
        for(int i = 0; i < data->GetElementCount(); i+=step)
        {
            xyElement(elem, i, d0, d1, x1);

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


void ParallelCoordinates::histCurve()
{
    int step = SCI::Max( 1, data->GetElementCount() / 500 );
    float * elem = new float[dim];


    for(int k = 0; k < (dim-1); k++)
    {
        int   d0 = dimLoc[k].second;
        int   d1 = dimLoc[k+1].second;
        float x0 = dimLoc[k].first;
        float x1 = dimLoc[k+1].first;
        float tol0 = 0;
        float tol1 = 0;
        float mean0 = 0;
        float mean1 = 0;
        float stddev0 = 0;
        float stddev1 = 0;

        for(int j = 0; j < numbin; j++)
        {
            bin0[j] = 0;
            bin1[j] = 0;
            bin0x[j] = 0;
            bin0y[j] = 0;
            bin1x[j] = 0;
            bin1y[j] = 0;
        }

        for(int i = 0; i < data->GetElementCount(); i += step )
        {
            data->GetElement( i, elem );
            float y0 = SCI::lerp(-rangeV, rangeV, (elem[d0]-dim_min[d0])/(dim_max[d0]-dim_min[d0]));
            float y1 = SCI::lerp(-rangeV, rangeV, (elem[d1]-dim_min[d1])/(dim_max[d1]-dim_min[d1]));

            for(int j = 0; j < numbin; j++)
            {
                float st = -rangeV + 2*rangeV*j/numbin;
                float en = -rangeV + 2*rangeV*(j+1)/numbin;
                if(y0 >= st && y0 < en)
                    bin0[j] += 1;
                if(y1 >= st && y1 < en)
                    bin1[j] += 1;
            }
        }

        for(int j = 0; j < numbin; j++)
        {

            tol0 += bin0[j];
            tol1 += bin1[j];
        }

        // compute standard deviation
        for(int j = 0; j < numbin; j++)
        {
            bin0x[j] = x0 + binrange*bin0[j]/tol0;
            bin0y[j] = -rangeV + 2*rangeV*j/numbin;
            bin1x[j] = x1 - binrange*bin1[j]/tol1;
            bin1y[j] = -rangeV + 2*rangeV*j/numbin;
            mean0 += bin0x[j];
            mean1 += bin1x[j];
        }

        mean0 = (x1+x0)/2;
        mean1 = (x1+x0)/2;

        for(int j = 0; j < numbin; j++)
        {

            stddev0 += (bin0x[j] - mean0)*(bin0x[j] - mean0);
            stddev1 += (bin1x[j] - mean1)*(bin1x[j] - mean1);
        }

        stddev0 = sqrtf(stddev0/numbin);
        stddev1 = sqrtf(stddev1/numbin);

        glLineWidth(4.0f);
        glBegin(GL_LINE_STRIP);
        glColor4f(0.9f,0,0.9f, 0.7f);
        glVertex3f(x0,-rangeV,0.95f);

        for (float t= -rangeV ; t <= rangeV; t += 0.01f)
        {
            float y0t, yy0t;
            for(int j = 0; j < numbin; j++)
            {
                float st = -rangeV + 2*rangeV*j/numbin;
                float en = -rangeV + 2*rangeV*(j+1)/numbin;
                if(t >= st && t < en)
                {
                    y0t = bin0x[j];
                    yy0t = bin0y[j];
                }
            }

            float x0t = x0 - 3.8f*(x1-x0)/2 - 0.005f + (1/(stddev0*(sqrtf(2*3.14f))))*(exp(-((y0t-mean0)*(y0t-mean0)/(2*stddev0*stddev0))));

            glVertex3f(x0t,t,0.95f);
        }
        glVertex3f(x0,rangeV,0.95f);
        glEnd();

        glLineWidth(4.0f);
        glBegin(GL_LINE_STRIP);
        glColor4f(0.9f,0,0.9f, 0.7f);
        glVertex3f(x1,-rangeV,0.95f);

        for (float t= -rangeV ; t <= rangeV; t += 0.01f)
        {
            float y1t, yy1t;
            for(int j = 0; j < numbin; j++)
            {
                float st = -rangeV + 2*rangeV*j/numbin;
                float en = -rangeV + 2*rangeV*(j+1)/numbin;
                if(t >= st && t < en)
                {
                    y1t = bin1x[j];
                    yy1t = bin1y[j];
                }
            }

            float x1t = x1 + 3.8f*(x1-x0)/2 + 0.005f -(1/(stddev1*(sqrtf(2*3.14f))))*(exp(-((y1t-mean1)*(y1t-mean1)/(2*stddev1*stddev1))));

            glVertex3f(x1t,t,0.95f);
        }
        glVertex3f(x1,rangeV,0.95f);
        glEnd();

    }
}
