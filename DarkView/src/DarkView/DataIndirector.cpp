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

#include <DarkView/DataIndirector.h>

DataIndirector::DataIndirector(Data::PhysicsData * _data ) : data(_data) {
    Recompute();
    SortData();
}

void DataIndirector::IgnoreDimension( int dim ) {
    data->Disable(dim);
    Recompute();
    SortData();
}

void DataIndirector::Recompute() {
    indr.clear();
    for(int i = 0; i < data->GetDim(); i++){
        if( data->isEnabled(i) ){
            indr.push_back(i);
        }
    }
}

// This function is used to sort dimensions of visual data
// indr is vector that contain list of visual dimension
void DataIndirector::SortData() {
    int dim = indr.size();
    float tp = 0.0f;
    float cor[100];

    for(int e = 0; e < dim -1; e++) 
    {
        cor[e] = data->GetCorrelation(e, e+1);
    }

    // sort dimensions from smallest to biggest
    for (int m = 0;  m < dim - 2;  m++)
    {
        for (int j = 0;  j < dim -2 - m;  j++)
        {
            if (cor[j] > cor[j+1])
            {
                tp = cor[j];
                cor[j] = cor[j+1];
                cor[j+1]= tp;
                std::swap(indr[j], indr[j+1]);
            }
        }
    }
}

// hoa sua
// This function is used to swap two dimension of data indirector.
void DataIndirector::SwapDims(int dim_x, int dim_y)
{
    std::swap(indr[dim_x], indr[dim_y]);
}

int DataIndirector::GetRealDimension( int dim ){
    if(dim>=(int)indr.size()) return -1;
    return indr[dim];
}

bool DataIndirector::isIgnored( int dim ){
    return !data->isEnabled(dim);
}

int DataIndirector::GetDim(){
    return (int)indr.size();
}

void DataIndirector::GetElement( int elem_id, float * space ){
    std::vector<float> tmp = data->GetElement( elem_id );
    for(int i = 0; i < (int)indr.size(); i++){
        space[i] = tmp[ indr[i] ];
    }
}

float DataIndirector::GetElement( int elem_id, int dim ){
    return data->GetElement( elem_id, indr[dim] );
}

std::string DataIndirector::GetLabel( int dim ){
    return data->GetLabel( indr[dim] );
}

std::string DataIndirector::GetLabelParsed( int dim ){
    return data->GetLabelParsed( indr[dim] );
}

float DataIndirector::GetCorrelation( int dim_x, int dim_y ){
    return data->GetCorrelation( indr[dim_x], indr[dim_y] );
}

float DataIndirector::GetMinimumValue( int dim ){
    if( dim == -1 )
        return data->GetMinimumValue( -1 );
    return data->GetMinimumValue( indr[dim] );
}

float DataIndirector::GetMaximumValue( int dim ){
    if( dim == -1 )
        return data->GetMaximumValue( -1 );
    return data->GetMaximumValue( indr[dim] );
}

int DataIndirector::GetElementCount( ){
    return data->GetElementCount();
}
