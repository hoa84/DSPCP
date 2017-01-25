/*
**  Common Library
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

#ifndef SCI_VEXN_H
#define SCI_VEXN_H

#include <vector>
#include <iostream>
#include <SCI/Utility.h>

namespace SCI {

    class VexN : public std::vector<float> {
    public:
        VexN( )                              : std::vector<float>( )                   { }
        VexN( int siz )                      : std::vector<float>( siz )               { }
        VexN( int siz, float default_val )   : std::vector<float>( siz, default_val )  { }
        VexN( const std::vector<float> & v ) : std::vector<float>( v.begin(),v.end() ) { }

        double Mean( ) const {
            double val = 0;
            for(int d = 0; d < (int)size(); d++){
                val += at(d);
            }
            return val / (float)size();
        }

        double Stdev( ) const {
            double mean = Mean( );
            double sum_dif = 0;
            for(int d = 0; d < (int)size(); d++){
                sum_dif += (double)powf( (float)(at(d) - mean), 2.0f );
            }
            return (double)sqrtf((float)sum_dif/(float)size());
        }

        VexN & operator += (const std::vector<float> & right){
            int i = 0;
            while( i < (int)size() && i < (int)right.size() ){ at(i) += right[i];     i++; }
            while(                    i < (int)right.size() ){ push_back( right[i] ); i++; }
            return (*this);
        }

        VexN operator + (const std::vector<float> & right) const {
            VexN ret;
            int i = 0;
            while( i < (int)size() && i < (int)right.size() ){ ret.push_back( at(i)+right[i] ); i++; }
            while( i < (int)size()                          ){ ret.push_back( at(i)          ); i++; }
            while(                    i < (int)right.size() ){ ret.push_back(       right[i] ); i++; }
            return ret;
        }

        VexN operator - (const std::vector<float> & right) const {
            VexN ret;
            int i = 0;
            while( i < (int)size() && i < (int)right.size() ){ ret.push_back( at(i)-right[i] ); i++; }
            while( i < (int)size()                          ){ ret.push_back( at(i)          ); i++; }
            while(                    i < (int)right.size() ){ ret.push_back(      -right[i] ); i++; }
            return ret;
        }

        VexN operator / ( float right ) const {
            VexN ret;
            for(int i = 0; i < (int)size(); i++){
                ret.push_back( at(i) / right );
            }
            return ret;
        }


        VexN & operator = ( const std::vector<float> & right ){
            clear();
            assign( right.begin(), right.end() );
            return (*this);
        }
    };


    inline float L2Norm( const VexN & v0, const VexN & v1){
        float sum = 0;
        for(int i = 0; i < (int)v0.size() && i < (int)v1.size(); i++){
            sum += (v0[i]-v1[i])*(v0[i]-v1[i]);
        }
        return sqrtf( sum );
    }

    inline VexN fabsf( const VexN vn ){
        VexN ret;
        for(int i = 0; i < (int)vn.size(); i++){
            ret.push_back( ::fabsf(vn[i]) );
        }
        return ret;
    }

    inline double PearsonCorrelation( const VexN & v0, const VexN & v1 ){
        double mean_x = v0.Mean();
        double mean_y = v1.Mean();

        double num  = 0.0;
        double denx = 0.0;
        double deny = 0.0;

        for(int i = 0; i < (int)v0.size() && i < (int)v1.size(); i++ ){
            float x = ( i < (int)v0.size() ) ? v0[i] : 0;
            float y = ( i < (int)v1.size() ) ? v1[i] : 0;
            num  += (x-mean_x)*(y-mean_y);
            denx += (x-mean_x)*(x-mean_x);
            deny += (y-mean_y)*(y-mean_y);
        }

        if( ::fabs(num) < 1.0e-100 || denx < 1.0e-100 || deny < 1.0e-100 ) return 0;

        return num / ( sqrt(denx) * sqrt(deny) );
    }



    inline VexN lerp( const VexN v0, const VexN v1, float t ){
        std::vector<float> ret;
        for(int i = 0; i < (int)v0.size() && i < (int)v1.size(); i++){
            ret.push_back( (v0[i])*(1.0f-t)+(v1[i])*(t) );
        }
        return ret;
    }




}

#endif // SCI_VEXN_H
