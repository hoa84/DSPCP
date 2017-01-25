#include <DarkView/DimensionalityReduction.h>

#include <string.h>

#include <iostream>
#include <limits>
#include <float.h>

using namespace std;

int DimensionalityReduction::GetMethodCount(){
    return DR_WRAPPER::GetMethodCount();
}

const char * DimensionalityReduction::GetMethodName( int i ){
    return DR_WRAPPER::GetMethodName(i);
}


DimensionalityReduction::DimensionalityReduction(){
    high_dimN     = 0;
    low_dimN      = 0;
    cur_method    = 0;
    min_elem = 0;
    max_elem = 0;
    fcount = 0;
}

DimensionalityReduction::~DimensionalityReduction(){ }


void DimensionalityReduction::Calculate( int method, float * data_in, int elemDim, int elemCount, int reducedDim ){

    high_dimN     = elemDim;
    low_dimN      = reducedDim;
    fcount      = elemCount;

    dr.SetSize( elemCount, low_dimN, high_dimN );

    double *tmp;

    tmp = new double[high_dimN];
    for(int i = 0; i < (int)elemCount; i++){
        for(int j = 0; j < high_dimN; j++){
            tmp[j] = data_in[i*high_dimN+j];
        }
        dr.SetInputData( i, tmp );
    }
    delete [] tmp;

    if(min_elem) delete [] min_elem;
    if(max_elem) delete [] max_elem;
    min_elem = new float[low_dimN];
    max_elem = new float[low_dimN];

    cur_method = method;
    dr.SetMethod( method );
    dr.Run( );

    for(int d = 0; d < low_dimN; d++){
        min_elem[d] =  FLT_MAX;
        max_elem[d] = -FLT_MAX;
    }

    tmp = new double[low_dimN];
    for(int i = 0; i < elemCount; i++){
        dr.GetOutputData( i, tmp );
        for(int d = 0; d < low_dimN; d++){
            min_elem[d] = ( min_elem[d] < (float)tmp[d] ) ? min_elem[d] : (float)tmp[d];
            max_elem[d] = ( max_elem[d] > (float)tmp[d] ) ? max_elem[d] : (float)tmp[d];
        }
    }
    delete [] tmp;

}

void DimensionalityReduction::GetFeaturePoint( int elem, double * pout ) const {
    dr.GetOutputData( elem, pout );
}

void DimensionalityReduction::GetFeaturePoint( int elem, float * pout ) const {
    dr.GetOutputData( elem, pout );
}

void DimensionalityReduction::GetMappedPoint( float  * pin, float  * pout ) const {
    double * din  = new double[high_dimN];
    double * dout = new double[low_dimN];
    for(int i = 0; i < high_dimN; i++){
        din[i]=pin[i];
    }
    GetMappedPoint(din,dout);
    for(int i = 0; i < low_dimN; i++){
        pout[i]=dout[i];
    }
}

void DimensionalityReduction::GetMappedPoint( double * pin, double * pout ) const {
    dr.GetMappedPoint( pin, pout );
}

int DimensionalityReduction::GetFeatureCount() const {
    return fcount;
}

int DimensionalityReduction::GetHighDimension() const {
    return high_dimN;
}

int DimensionalityReduction::GetLowDimension() const {
    return low_dimN;
}

float DimensionalityReduction::GetMinValue( int dim ) const {
    if( dim < 0 || dim >= low_dimN ) return FLT_MAX;
    return min_elem[dim];
}

float DimensionalityReduction::GetMaxValue( int dim ) const {
    if( dim < 0 || dim >= low_dimN ) return FLT_MAX;
    return max_elem[dim];
}

void DimensionalityReduction::GetMeanPCA( float * pout ){
    dr.GetMeanPCA( pout );
}

void DimensionalityReduction::GetVectorPCA( int component, float * vout ){
    float * tmp = new float[high_dimN*low_dimN];
    dr.GetMatrixPCA( tmp );
    for(int i = 0; i < high_dimN; i++){
        vout[i] = tmp[(low_dimN-component-1)*high_dimN+i];
    }
    delete [] tmp;
}

void dimPCA()
{
    DimensionalityReduction dr;

    float input[8] = { 1.2, 0.75,
                       1.1, 0.75,
                       1.0, 0.75,
                       1.3, 0.75  };
    int dimN = 2;
    int elemN = 4;
    int componentN = 2;

    float mean[2];
    float pc0[2];
    float pc1[2];

    dr.Calculate( 0, input, dimN, elemN, componentN );
    dr.GetMeanPCA( mean );
    dr.GetVectorPCA( 0, pc0 );
    dr.GetVectorPCA( 1, pc1 );

    printf("%f %f\n",mean[0],mean[1]);
    printf("%f %f\n",pc0[0],pc0[1]);
    printf("%f %f\n",pc1[0],pc1[1]);

}


