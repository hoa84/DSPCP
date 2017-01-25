#include <DarkView/DRL_Wrapper.h>

#ifdef WIN32

#include <string.h>

#include <iostream>
#include <limits>
#include "dimred/ya_dimred.h"

using namespace std;
using namespace yala;

int DRL_Wrapper::GetMethodCount(){
    return 5;
}

const char * DRL_Wrapper::GetMethodName(int id){
    switch(id){
    case 0: return "PCA - Principal component analysis";
    case 1: return "MDS - Multidimensional scaling";
    case 2: return "LLE - Locally linear embedding";
    case 3: return "IsoMap";
    case 4: return "AE - Autoencoder Neural Network";
    }
    return "Error";
}


DRL_Wrapper::DRL_Wrapper(){
    input_matrix = (double*)malloc(0);
    output_matrix = (double*)malloc(0);
    high_dim = 0;
    low_dim = 0;
    elemN = 0;
    redmet        = NULL;
}

DRL_Wrapper::~DRL_Wrapper(){
    if(input_matrix)  free(input_matrix);
    if(output_matrix) free( output_matrix );
    if(redmet) delete redmet;
}

void DRL_Wrapper::SetMethod( int method ){
    if(redmet) delete redmet;
    switch( method ){
        case 0:    redmet=new YAPCAReduce<double>; break;
        case 1:    redmet=new YAMDSReduce<double>; break;
        case 2:    redmet=new YALLEReduce<double>; break;
        case 3: redmet=new YAIsoReduce<double>; break;
        case 4:     redmet=new YAATEReduce<double>; break;
        default: return;
    }

}

void DRL_Wrapper::SetSize( int _elemN, int _low_dim, int _high_dim ){

    high_dim = _high_dim;
    low_dim = _low_dim;
    elemN = _elemN;

    input_matrix  = (double*)realloc( input_matrix,sizeof(double)*elemN*high_dim);
    output_matrix = (double*)realloc(output_matrix,sizeof(double)*elemN*low_dim);

}

void DRL_Wrapper::SetInputData( int elem, double * vals ){
    for(int i = 0; i < high_dim; i++){
        input_matrix[ elem*high_dim + i ] = vals[i];
    }
}

void DRL_Wrapper::SetInputData( int elem, float * vals ){
    for(int i = 0; i < high_dim; i++){
        input_matrix[ elem*high_dim + i ] = vals[i];
    }
}

void DRL_Wrapper::GetOutputData( int elem, double * vals ) const {
    for(int i = 0; i < low_dim; i++){
        vals[i] = output_matrix[ elem*low_dim + i ];
    }
}

void DRL_Wrapper::GetOutputData( int elem, float * vals ) const {
    for(int i = 0; i < low_dim; i++){
        vals[i] = output_matrix[ elem*low_dim + i ];
    }
}

void DRL_Wrapper::Run( ){

    YA_WRAP_RM(double) winput (  input_matrix, elemN, high_dim );
    YA_WRAP_RM(double) woutput( output_matrix, elemN,  low_dim );

    // Use progress meter and timing output
    ((YADimReduce<double>*)redmet)->verbose(2);

    // Eigen decomposition options
    EigenOptions eigopts;

    // Use k-nearest neighbors with k=10
    ((YADimReduce<double>*)redmet)->neighbor_mode(0);
    ((YADimReduce<double>*)redmet)->neighbors(5);

    // Reduce the dimensionality
    ((YADimReduce<double>*)redmet)->find_t(winput,woutput,low_dim,eigopts);

    //((YAPCAReduce<double>*)redmet)->save_map(cout);

}

void DRL_Wrapper::GetMappedPoint( double * highd_in, double * lowd_out ) const {
    YA_WRAP_RM(double) winput(highd_in,1,high_dim);
    YA_WRAP_RM(double) woutput(lowd_out,1,low_dim);
    ((YADimReduce<double>*)redmet)->forward_t( winput, woutput );
}

#endif
