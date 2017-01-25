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

#ifndef DATAINDIRECTOR_H
#define DATAINDIRECTOR_H

#include <Data/PhysicsData.h>

class DataIndirector
{
public:
    DataIndirector( Data::PhysicsData * data );

    void IgnoreDimension( int dim );
    bool isIgnored( int dim );

    int GetRealDimension( int dim );
    int GetDim();

    int GetElementCount( );

    std::string GetLabel( int dim );
    std::string GetLabelParsed( int dim );

    void GetElement( int elem_id, float * space );
    float GetElement( int elem_id, int dim );

    float GetMinimumValue( int dim );
    float GetMaximumValue( int dim );

    float GetCorrelation( int dim_x, int dim_y );

    void Recompute();
    void SortData();
    void SwapDims(int dim_x, int dim_y);

// hoa sua
// protected:
    Data::PhysicsData * data;
    std::vector<int>  indr;
};

#endif // DATAINDIRECTOR_H
