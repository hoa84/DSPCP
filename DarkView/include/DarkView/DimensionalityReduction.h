#ifndef DIMENSIONALITYREDUCTION_H
#define DIMENSIONALITYREDUCTION_H

#include <QObject>
#include <DarkView/DRL_Wrapper.h>
#include <DarkView/Shogun_Wrapper.h>

#ifdef WIN32
    #define DR_WRAPPER DRL_Wrapper
#else
    #define DR_WRAPPER Shogun_Wrapper
#endif

class DimensionalityReduction {

public:
    static int          GetMethodCount();
    static const char * GetMethodName( int i );

public:
    DimensionalityReduction();
    ~DimensionalityReduction();

    // Start a dimensionality reduction. Function will block until the feature points identify a map which can be used on the elements. New thread will be launched to reduce the elements.
    void Calculate( int method, float * _data_in, int elemDim, int elemCount, int reducedDim );

    // Return the total number of feature points
    int GetFeatureCount() const ;
    // Gets the reduced location of a feature point
    void GetFeaturePoint( int elem, float  * pout ) const ;
    void GetFeaturePoint( int elem, double * pout ) const ;

    // Maps a point (pin) from high dimensional space to low dimensional space
    void GetMappedPoint( float  * pin, float  * pout ) const ;
    void GetMappedPoint( double * pin, double * pout ) const ;

    void GetMeanPCA( float * pout );
    void GetVectorPCA( int component, float * vout );

    // Return the number of input (high) dimensions
    int GetHighDimension() const ;
    // Return the number of output (low) dimensions
    int GetLowDimension() const ;

    // Functions for getting the bounding box of the low dimensional data
    float GetMinValue( int dim ) const ;
    float GetMaxValue( int dim ) const ;

protected:

    int fcount;
    int high_dimN;
    int low_dimN;

    float * min_elem;
    float * max_elem;

    DR_WRAPPER dr;

    int cur_method;


public:
    void dimPCA();
};



#endif // DIMENSIONALITYREDUCTION_H
