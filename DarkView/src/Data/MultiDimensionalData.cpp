/*
**  Common Data Library
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


#include <Data/MultiDimensionalData.h>
#include <float.h>

using namespace Data;

MultiDimensionalData::MultiDimensionalData( int _elemN, int _dimN ) : dimN(_dimN), elemN(_elemN){
    min_dval.resize( dimN,  FLT_MAX );
    max_dval.resize( dimN, -FLT_MAX );
    min_val =  FLT_MAX;
    max_val = -FLT_MAX;
}

MultiDimensionalData::~MultiDimensionalData(){ }

void MultiDimensionalData::Resize( int _elemN, int _dimN ){
    elemN = _elemN;
    dimN = _dimN;
    min_dval.resize( dimN,  FLT_MAX );
    max_dval.resize( dimN, -FLT_MAX );
    min_val =  FLT_MAX;
    max_val = -FLT_MAX;
}

int MultiDimensionalData::GetDimension() const { return dimN; }

int MultiDimensionalData::GetElementCount() const { return elemN; }

float MultiDimensionalData::GetMaximumValue( int dim ) const {
    if( dim < 0 ){ return max_val; }
    if( dim >= (int)max_dval.size() ){ return FLT_MAX; }
    return max_dval[dim];
}

float MultiDimensionalData::GetMinimumValue( int dim ) const {
    if( dim < 0 ){ return min_val; }
    if( dim >= (int)min_dval.size() ){ return FLT_MAX; }
    return min_dval[dim];
}
