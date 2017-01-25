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

#include <Data/DenseMultiDimensionalData.h>

#include <SCI/Utility.h>
#include <float.h>

using namespace Data;

DenseMultiDimensionalData::DenseMultiDimensionalData( int _elemN, int _dimN ) : MultiDimensionalData(_elemN,_dimN){
    data.resize( elemN * dimN, FLT_MAX );
}

void DenseMultiDimensionalData::Resize( int _elemN, int _dimN ){
    MultiDimensionalData::Resize(_elemN,_dimN);
    data.resize( elemN * dimN, FLT_MAX );
}

// Get a rough estimate of the size of the data contained in the class
int DenseMultiDimensionalData::GetDataSize() const {
    return (int)(data.size() * sizeof(float));
}

// Various functions for setting values
void DenseMultiDimensionalData::SetElement( int elem_id, const std::vector<float> & val ){
    if( elem_id < 0 || elem_id >= GetElementCount() ) return;
    for(int cur_dim = 0; cur_dim < GetDimension() && cur_dim < (int)val.size(); cur_dim++){
        data[ cur_dim * GetElementCount() + elem_id] = val[cur_dim];
    }
    min_val = max_val = FLT_MAX;
}

void DenseMultiDimensionalData::SetElement( int elem_id, int dim, float val ){
    if( elem_id < 0 || elem_id >= GetElementCount() ) return;
    data[ dim * GetElementCount() + elem_id ] = val;
    min_val = max_val = FLT_MAX;
}

void DenseMultiDimensionalData::SetElement( int elem_id, const float  * val ){
    if( elem_id < 0 || elem_id >= GetElementCount() ) return;
    for(int cur_dim = 0; cur_dim < GetDimension(); cur_dim++){
        data[ cur_dim * GetElementCount() + elem_id] = val[cur_dim];
    }
    min_val = max_val = FLT_MAX;
}

void DenseMultiDimensionalData::SetElement( int elem_id, const double * val ){
    if( elem_id < 0 || elem_id >= GetElementCount() ) return;
    for(int cur_dim = 0; cur_dim < GetDimension(); cur_dim++){
        data[ cur_dim * GetElementCount() + elem_id] = (float)val[cur_dim];
    }
    min_val = max_val = FLT_MAX;
}


// Various functions for getting values
SCI::VexN DenseMultiDimensionalData::GetElement( int elem_id ) const {
    SCI::VexN ret( GetDimension() );
    if( elem_id < 0 || elem_id >= GetElementCount() ) return ret;
    for(int cur_dim = 0; cur_dim < GetDimension(); cur_dim++){
        ret[cur_dim] = data[ cur_dim*GetElementCount() + elem_id ] ;
    }
    return ret;
}

float DenseMultiDimensionalData::GetElement( int elem_id, int dim ) const {
    if( elem_id < 0 || elem_id >= GetElementCount() ) return FLT_MAX;
    return data[ dim*GetElementCount() + elem_id ];
}

void DenseMultiDimensionalData::GetElement( int elem_id, float  * space ) const {
    if( elem_id < 0 || elem_id >= GetElementCount() ) return;
    for(int cur_dim = 0; cur_dim < GetDimension(); cur_dim++){
        space[cur_dim] = data[ cur_dim*GetElementCount() + elem_id];
    }
}

void DenseMultiDimensionalData::GetElement( int elem_id, double * space ) const {
    if( elem_id < 0 || elem_id >= GetElementCount() ) return;
    for(int cur_dim = 0; cur_dim < GetDimension(); cur_dim++){
        space[cur_dim] = data[ cur_dim*GetElementCount() + elem_id];
    }
}


void DenseMultiDimensionalData::RecalculateMinumumAndMaximum(){
    if( fabsf(min_val) == FLT_MAX || fabsf(max_val) == FLT_MAX ){
        min_val =  FLT_MAX;
        max_val = -FLT_MAX;
        for(int cur_dim = 0; cur_dim < GetDimension(); cur_dim++){
            min_dval[cur_dim] =  FLT_MAX;
            max_dval[cur_dim] = -FLT_MAX;
            for(int elem_id = 0; elem_id < GetElementCount(); elem_id++ ){
                min_dval[cur_dim] = SCI::Min( min_dval[cur_dim], data[ cur_dim*GetElementCount() + elem_id ] );
                max_dval[cur_dim] = SCI::Max( max_dval[cur_dim], data[ cur_dim*GetElementCount() + elem_id ] );
            }
            min_val = SCI::Min( min_val, min_dval[cur_dim] );
            max_val = SCI::Max( max_val, max_dval[cur_dim] );
        }
    }
}

float DenseMultiDimensionalData::GetMaximumValue( int dim ) {
    RecalculateMinumumAndMaximum();
    return MultiDimensionalData::GetMaximumValue(dim);
}

float DenseMultiDimensionalData::GetMinimumValue( int dim ) {
    RecalculateMinumumAndMaximum();
    return MultiDimensionalData::GetMinimumValue(dim);
}

