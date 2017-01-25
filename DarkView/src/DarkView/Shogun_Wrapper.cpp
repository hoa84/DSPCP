#include <DarkView/Shogun_Wrapper.h>

#ifndef WIN32

#include <shogun/base/init.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/converter/KernelLocallyLinearEmbedding.h>
#include <shogun/preprocessor/PCA.h>
#include <shogun/preprocessor/KernelPCA.h>
#include <shogun/kernel/LinearKernel.h>
#include <shogun/kernel/GaussianKernel.h>

using namespace shogun;

bool ___shogun_inited = false;


int Shogun_Wrapper::GetMethodCount(){
    return 2;
}

const char * Shogun_Wrapper::GetMethodName(int id){
    switch(id){
    case 0: return "PCA";
    case 1: return "Kernel PCA";
    case 2: return "Kernel LLE";
    }
    return "Unknown Switch";
}

void print_fun( FILE * file, const char * msg ){
    printf("SHOGUN : %s\n",msg); fflush(stdout);
}

void cancel_fun( bool & b0, bool & b1 ){

}

void print_message(FILE* target, const char* str)
{
    fprintf(target, "%s", str); fflush(target);
}

void print_warning(FILE* target, const char* str)
{
    fprintf(target, "%s", str); fflush(target);
}

void print_error(FILE* target, const char* str)
{
    fprintf(target, "%s", str); fflush(target);
}



Shogun_Wrapper::Shogun_Wrapper(){

    if(!___shogun_inited){
        //init_shogun( print_fun, print_fun, print_fun, cancel_fun );
        //init_shogun_with_defaults();
        init_shogun(&print_message, &print_warning, &print_error);
        ___shogun_inited = true;
    }

    input_matrix = (double*)malloc(0);
    output_matrix = (double*)malloc(0);
    high_dim = 0;
    low_dim = 0;
    elemN = 0;
    _preprocessor = 0;
    _kernel = 0;
    method = 0;
}

Shogun_Wrapper::~Shogun_Wrapper(){

    if(input_matrix)  free(input_matrix);
    if(output_matrix) free( output_matrix );

}




void Shogun_Wrapper::SetMethod( int _method ){
    method = _method;
}

void Shogun_Wrapper::SetSize( int _elemN, int _low_dim, int _high_dim ){

    high_dim = _high_dim;
    low_dim = _low_dim;
    elemN = _elemN;

    input_matrix  = (double*)realloc( input_matrix,sizeof(double)*elemN*high_dim);
    output_matrix = (double*)realloc(output_matrix,sizeof(double)*elemN*low_dim);

}

void Shogun_Wrapper::Run( ){

    CDenseFeatures<double>* features = new CDenseFeatures<double>(input_matrix,high_dim,elemN);

    switch(method){
    case 0:
        {
            _preprocessor = new CPCA();

        } break;
    case 1:
        {
            float kernel_width = 0.01f;
            CGaussianKernel * kernel = new CGaussianKernel(10,kernel_width);
            kernel->init(features, features);
            _preprocessor = new CKernelPCA(kernel);
        } break;
    }

    CDimensionReductionPreprocessor * preprocessor = (CDimensionReductionPreprocessor*)_preprocessor;

    preprocessor->set_target_dim(low_dim);
    preprocessor->parallel->set_num_threads(4);
    preprocessor->init(features);

    for(int i = 0; i < elemN; i++){
        GetMappedPoint( input_matrix + (i*high_dim), output_matrix + (i*low_dim) );
    }


}

void Shogun_Wrapper::SetInputData( int elem, float  * vals ){
    for(int i = 0; i < high_dim; i++){
        input_matrix[ elem*high_dim + i ] = vals[i];
    }
}

void Shogun_Wrapper::SetInputData( int elem, double * vals ){
    for(int i = 0; i < high_dim; i++){
        input_matrix[ elem*high_dim + i ] = vals[i];
    }
}


void Shogun_Wrapper::GetOutputData( int elem, double * vals ) const {
    for(int i = 0; i < low_dim; i++){
        vals[i] = output_matrix[ elem*low_dim + i ];
    }
}

void Shogun_Wrapper::GetOutputData( int elem, float  * vals ) const {
    for(int i = 0; i < low_dim; i++){
        vals[i] = output_matrix[ elem*low_dim + i ];
    }
}

void Shogun_Wrapper::GetMatrixPCA( float * mat ){
    CPCA * pca = dynamic_cast<CPCA*>((CDimensionReductionPreprocessor *)_preprocessor);
    if( pca != 0 ){
        SGMatrix<double> m = pca->get_transformation_matrix();
        for(int i = 0; i < m.num_cols*m.num_rows; i++){
            mat[i] = m.matrix[i];
        }
    }
}

void Shogun_Wrapper::GetMeanPCA( float * vec ){
    CPCA * pca = dynamic_cast<CPCA*>((CDimensionReductionPreprocessor *)_preprocessor);
    if( pca != 0 ){
        SGVector<double> v = pca->get_mean();
        for(int i = 0; i < v.size(); i++){
            vec[i] = v.vector[i];
        }
    }
}

void Shogun_Wrapper::GetMappedPoint( double * highd_in, double * lowd_out ) const {
    SGVector<double> * hd = new SGVector<double>( high_dim, false );
    //SGVector<double> hd( highd_in, high_dim, false );
    memcpy(hd->vector,highd_in,sizeof(double)*high_dim);
    //preprocessor->apply_to_feature_vector( hd );

    //SGVector<double> hd = SGVector<double>( highd_in, high_dim, false );
    SGVector<double> ld = ((CDimensionReductionPreprocessor*)_preprocessor)->apply_to_feature_vector( *hd );

    for(int j = 0; j < low_dim; j++){
        lowd_out[j] = ld.vector[j];
    }
    //printf("%1.2f %1.2f\n",lowd_out[0],lowd_out[1]);

    delete hd;
    /*
    for(int j = 0; j < low_dim; j++){
        lowd_out[j] = 0;
    }
    */

}

#endif



