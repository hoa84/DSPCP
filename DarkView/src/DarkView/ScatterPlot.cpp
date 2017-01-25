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

#include <DarkView/ScatterPlot.h>

#include <GL/oglCommon.h>
#include <SCI/Vex3.h>

ScatterPlot::ScatterPlot( oglWidgets::oglFont & _font )
{
    font = &_font;
    aspect = 1.0f;
    border_size = 0;
    real_dimX = -1;
    real_dimY = -1;
    curdraw = 0;
}

void ScatterPlot::Reset()
{
    real_dimX = -1;
    real_dimY = -1;
    curdraw = 0;
}

void ScatterPlot::Set(DataIndirector & data, int dimX, int dimY, bool show_labels )
{
    if( &data != _data || real_dimX != data.GetRealDimension( dimX ) || real_dimY != data.GetRealDimension( dimY ) )
    {
        curdraw = 0;
        real_dimX = data.GetRealDimension( dimX );
        real_dimY = data.GetRealDimension( dimY );
    }
    _data = &data;
    _dimX  = dimX;
    _dimY  = dimY;
    _show_labels = show_labels;
}

void ScatterPlot::SetAspect( float a )
{
    aspect = a;
}

void ScatterPlot::SetBorderWidth( float w )
{
    border_size = w;
}

void ScatterPlot::SetBorderColor( float r, float g, float b, float a )
{
    border_color.Set(r,g,b,a);
}

float frexp10(float x, int & exp)
{
    exp = ((int)log10(fabsf(x))) + ((fabsf(x)<1)?-1:0);
    return x / powf(10,exp);
}

void ScatterPlot::ProgressiveReset( )
{
    curdraw = 0;
}

void ScatterPlot::UpdateLayout( )
{
    x_min = _data->GetMinimumValue( _dimX );
    x_max = _data->GetMaximumValue( _dimX );
    dscaleX = (x_max-x_min)/2.0f;
    dcenX   = (x_max+x_min)/2.0f;

    y_min = _data->GetMinimumValue( _dimY );
    y_max = _data->GetMaximumValue( _dimY );
    dscaleY = (y_max-y_min)/2.0f;
    dcenY   = (y_max+y_min)/2.0f;

    if( dscaleX == 0 ){ dscaleX = 0.5f; }
    if( dscaleY == 0 ){ dscaleY = 0.5f; }

    x_min = dcenX - dscaleX * 1.15f;
    x_max = dcenX + dscaleX * 1.15f;

    y_min = dcenY - dscaleY * 1.15f;
    y_max = dcenY + dscaleY * 1.15f;
}

// draw SCPs in SPLOM
bool ScatterPlot::ProgressiveDraw( )
{
    if( curdraw >= _data->GetElementCount() )
    {
        return false;
    }

    UpdateLayout( );

    glPushMatrix();
    glTranslatef( center.x, center.y, 0.0f );

    float correlation = _data->GetCorrelation( _dimX, _dimY );
    SCI::Vex4 cor_color;
    if( correlation > 0 ) cor_color = SCI::Vex4(1,0,0,1) * ( correlation );
    if( correlation < 0 ) cor_color = SCI::Vex4(0,0,1,1) * (-correlation );

    glTranslatef( -size.x/2.0f, -size.y/2.0f, 0.0f );
    glScalef( size.x / (x_max-x_min), size.y / (y_max-y_min), 1.0f );
    glTranslatef( -x_min, -y_min, 0.0f );


    DrawPoints( cor_color, 0, _data->GetElementCount(), 1 );

    glPopMatrix();

    return true;
}

void ScatterPlot::ProgressiveBorder( )
{
    UpdateLayout( );

    glPushMatrix();
    glTranslatef( center.x, center.y, 0.0f );

    glEnable(GL_DEPTH_TEST);

    float correlation = _data->GetCorrelation( _dimX, _dimY );
    SCI::Vex4 cor_color;
    if( correlation > 0 ) cor_color = SCI::Vex4(1,0,0,1) * ( correlation );
    if( correlation < 0 ) cor_color = SCI::Vex4(0,0,1,1) * (-correlation );
    if(border_size > 0)
    {
        glLineWidth(border_size);
        //glColor4fv(border_color.data);
        glColor4fv(cor_color.data);
        glBegin(GL_LINE_LOOP);
            glVertex3f( -size.x/2.0f, -size.y/2.0f, 0.0f );
            glVertex3f(  size.x/2.0f, -size.y/2.0f, 0.0f );
            glVertex3f(  size.x/2.0f,  size.y/2.0f, 0.0f );
            glVertex3f( -size.x/2.0f,  size.y/2.0f, 0.0f );
        glEnd();
    }
    glPopMatrix();

}

// draw SCP in detail view
void ScatterPlot::Draw( )
{
    UpdateLayout( );

    glPushMatrix();
    glTranslatef( center.x, center.y, 0.0f );
    if( _show_labels )
    {
        int exp = 0;
        float txt_siz = SCI::Min(size.x,size.y) / 15.0f;

        SCI::Graphics::TextElement text(font);

        glPushMatrix();
        glScalef(1.0f,1.0f,1.0f);

        text.SetColor(0,0,0);
        text.SetSize( txt_siz );
        text.SetAspect( aspect );

        text.SetOrientation(0.0f);
        text.SetCenter( 0.0f);
        text.SetTop(   -size.y/2.0f);
        text.SetText( _data->GetLabelParsed(_dimX).c_str() );
        text.Draw();

        text.SetOrientation(90.0f);
        text.SetRight( -size.x/2.0f*1.02f);
        text.SetMiddle( 0.0f );
        text.SetText( _data->GetLabelParsed(_dimY).c_str() );
        text.Draw();

        text.SetColor(0,0,0);
        text.SetSize( txt_siz/1.5f );

        text.SetOrientation(0.0f);
        text.SetLeft(    size.x/2.0f*1.01f );
        text.SetBottom( -size.y/2.0f );
        text.SetTextf( "%1.2fe%+i", frexp10(y_max,exp), exp );
        text.Draw();

        text.SetOrientation(0.0f);
        text.SetLeft( size.x/2.0f*1.01f );
        text.SetTop(  size.y/2.0f );
        text.SetTextf( "%1.2fe%+i", frexp10(y_min,exp), exp );
        text.Draw();

        text.SetOrientation(-35.0f);
        text.SetLeft(  -size.x/2.0f );
        text.SetBottom( size.y/2.0f*1.01f );
        text.SetTextf( "%1.2fe%+i", frexp10(x_min,exp), exp );
        text.Draw();

        text.SetOrientation(-35.0f);
        text.SetRight(  size.x/2.0f );
        text.SetBottom( size.y/2.0f*1.01f );
        text.SetTextf( "%1.2fe%+i", frexp10(x_max,exp), exp );
        text.Draw();

        glPopMatrix();
    }

    float correlation = _data->GetCorrelation( _dimX, _dimY );
    SCI::Vex4 cor_color;
    if( correlation > 0 ) cor_color = SCI::Vex4(1,0,0,1) * ( correlation );
    if( correlation < 0 ) cor_color = SCI::Vex4(0,0,1,1) * (-correlation );
    if(border_size > 0)
    {
        glLineWidth(border_size);
        glColor4fv(cor_color.data);
        glBegin(GL_LINE_LOOP);
        glVertex3f( -size.x/2.0f, -size.y/2.0f, 0.0f );
        glVertex3f(  size.x/2.0f, -size.y/2.0f, 0.0f );
        glVertex3f(  size.x/2.0f,  size.y/2.0f, 0.0f );
        glVertex3f( -size.x/2.0f,  size.y/2.0f, 0.0f );
        glEnd();
    }

    glTranslatef( -size.x/2.0f, -size.y/2.0f, 0.0f );
    glScalef( size.x / (x_max-x_min), size.y / (y_max-y_min), 1.0f );
    glTranslatef( -x_min, -y_min, 0.0f );

    DrawPoints( cor_color, 0, _data->GetElementCount(), SCI::Max(1,_data->GetElementCount()/20000) );

    glPopMatrix();

}

void ScatterPlot::DrawPoints( SCI::Vex4 col, int start, int stop, int step )
{    
    glBegin(GL_POINTS);
        for(int k = start; k < stop; k+=step)
        {
            glColor4fv(col.data);
            float x = _data->GetElement( k, _dimX );
            float y = _data->GetElement( k, _dimY );

            glVertex3f(x,y,0.1f);
        }
    glEnd();
}

// draw fitting cubic curve
void ScatterPlot::fitCubicCurve(SCI::Vex4 col, int start, int stop, int step)
{
    float C1 = 4000.0f;
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
    float a = 0.0f;
    float b = 0.0f;
    float c = 0.0f;
    float d = 0.0f;

    for(int k = start; k < stop; k+=step)
    {
        float x = _data->GetElement( k, _dimX );
        float y = _data->GetElement( k, _dimY );

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

    glBegin(GL_POINTS);
    glColor3f(0,0,0);

    for(int k = start; k < stop; k+=step)
    {
        float xt = _data->GetElement( k, _dimX );
        float yt = a + b*xt + c*xt*xt + d*xt*xt*xt;
        glVertex3f(xt,yt,-0.5f);
    }
    glEnd();
}

// draw fitting quadratic curve
void ScatterPlot::fitQuadCurve(SCI::Vex4 col, int start, int stop, int step)
{
    float D1 = 4000.0f;
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
    float e, g, f;

    for(int k = start; k < stop; k+=step)
    {
        float x = _data->GetElement( k, _dimX );
        float y = _data->GetElement( k, _dimY );

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

    // draw curve
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(0,0,0);

    for(int k = start; k < stop; k+=step)
    {
        float xt = _data->GetElement( k, _dimX );
        float yt = e + f*xt + g*xt*xt;
        glVertex3f(xt,yt,-0.5f);
    }
    glEnd();
}

// draw quadratic curve
void ScatterPlot::quadCurve(float a, float b, float c, SCI::Vex4 col)
{
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glColor4fv(col.data);

    for (float t=-1; t <= 1; t += 0.01f)
    {
        float yt = t;
        float xt = a + b*yt + c*yt*yt;
        glVertex3f(xt,yt,-0.5f);
    }
    glEnd();
}

// draw cubic curve
void ScatterPlot::cubicCurve(float a, float b, float c, float d, SCI::Vex4 col)
{
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(0,0,0);

    for (float t=x_min; t <= x_max; t += 0.01f)
    {
        float xt = t;
        float yt = a + b*xt + c*xt*xt + d*xt*xt*xt;
        glVertex3f(xt,yt,-0.5f);
    }
    glEnd();
}

