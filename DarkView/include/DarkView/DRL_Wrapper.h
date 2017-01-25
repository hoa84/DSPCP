#ifndef DRL_Wrapper_H
#define DRL_Wrapper_H

#ifdef WIN32

class DRL_Wrapper {

public:
    /*
    const static int METHOD_PCA;
    const static int METHOD_MDS;
    const static int METHOD_LLE;
    const static int METHOD_ISOMAP;
    const static int METHOD_AE;
    */

    static int GetMethodCount();
    static const char * GetMethodName(int id);

    DRL_Wrapper();
    ~DRL_Wrapper();

    void SetMethod( int method );
    void SetSize( int _elemN, int _low_dim, int _high_dim );

    void Run( );

    void SetInputData( int elem, float  * vals );
    void SetInputData( int elem, double * vals );

    void GetOutputData( int elem, double * vals ) const ;
    void GetOutputData( int elem, float  * vals ) const ;

    void GetMappedPoint( double * highd_in, double * lowd_out ) const ;

protected:
    double *  input_matrix;
    double * output_matrix;

    int high_dim;
    int low_dim;
    int elemN;

    void * redmet;

};

#endif

#endif // DRL_Wrapper_H
