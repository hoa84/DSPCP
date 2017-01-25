/*
**  DarkView: Parameter Space Visualization Tool
**  Copyright (C) 2013  Paul Rosen
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

#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include <GL/oglFont.h>
#include <Data/PhysicsData.h>
#include <DarkView/DataIndirector.h>
#include <SCI/Graphics/RenderElements.h>
#include <SCI/Vex4.h>

class ScatterPlot : public SCI::Graphics::RectangularRenderElement {

public:
    ScatterPlot( oglWidgets::oglFont & font );

    void Reset( );

    void SetAspect( float a );
    void Set(DataIndirector &data, int dimX, int dimY, bool show_labels );
    void SetBorderWidth( float w );
    void SetBorderColor( float r, float g, float b, float a = 1.0f );

    bool ProgressiveDraw();
    void ProgressiveReset( );
    void ProgressiveBorder( );

    virtual void Draw( );

protected:
    bool progressive_render;


    void UpdateLayout( );
    void DrawPoints( SCI::Vex4 col, int start, int stop, int step );

    oglWidgets::oglFont * font;

    DataIndirector *_data;
    int curdraw;
    int real_dimX, real_dimY;
    int _dimX;
    int _dimY;
    bool _show_labels;
    float aspect;
    float border_size;
    SCI::Vex4 border_color;

    float x_min;
    float x_max;
    float dscaleX;
    float dcenX;

    float y_min;
    float y_max;
    float dscaleY;
    float dcenY;

    void cubicCurve(float a, float b, float c, float d, SCI::Vex4 col);
    void quadCurve(float a, float b, float c, SCI::Vex4 col);
    void fitCubicCurve(SCI::Vex4 col, int start, int stop, int step);
    void fitQuadCurve(SCI::Vex4 col, int start, int stop, int step);
};

#endif // SCATTERPLOT_H
