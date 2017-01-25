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

#ifndef SMALLMULTIPLES_H
#define SMALLMULTIPLES_H

#include <Data/PhysicsData.h>
#include <DarkView/ScatterPlot.h>
#include <DarkView/DataIndirector.h>
#include <GL/oglFont.h>

class SmallMultiples
{
protected:

    oglWidgets::oglFont * font;

    std::vector<ScatterPlot> sp;

    float mouse_x, mouse_y;
    int selX, selY;
    int mouse_selX, mouse_selY;


public:
    SmallMultiples( oglWidgets::oglFont &font );

    void Reset( );
    std::pair<int,int> GetSelected( );
    void Select( );
    void SetMouse( float mx, float my );

    void ProgressiveReset( );
    bool ProgressiveDraw( DataIndirector & data );
    void ProgressiveBorder(DataIndirector & data );
    void ProgressiveLabels(float aspect, DataIndirector & data );
    //void Draw( float aspect, DataIndirector & data );
};

#endif // SMALLMULTIPLES_H
